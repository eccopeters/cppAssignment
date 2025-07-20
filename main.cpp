#include "crow/crow_all.h"
#include "sqlite3.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <stdexcept>

using json = nlohmann::json;

// Hall class (Encapsulation)
class Hall {
private:
    int hall_id_;
    std::string name_;
    int capacity_;
    std::string facilities_;
    std::string location_;

public:
    Hall(int id, const std::string& name, int capacity, const std::string& facilities, const std::string& location)
        : hall_id_(id), name_(name), capacity_(capacity), facilities_(facilities), location_(location) {}

    json toJson() const {
        return {
            {"hall_id", hall_id_},
            {"name", name_},
            {"capacity", capacity_},
            {"facilities", facilities_},
            {"location", location_}
        };
    }

    int getId() const { return hall_id_; }
};

// Booking class (Encapsulation)
class Booking {
private:
    int booking_id_;
    int hall_id_;
    int user_id_;
    std::string start_time_;
    std::string end_time_;
    std::string purpose_;
    std::string status_;

public:
    Booking(int booking_id, int hall_id, int user_id, const std::string& start_time,
            const std::string& end_time, const std::string& purpose, const std::string& status)
        : booking_id_(booking_id), hall_id_(hall_id), user_id_(user_id), start_time_(start_time),
          end_time_(end_time), purpose_(purpose), status_(status) {}

    json toJson() const {
        return {
            {"booking_id", booking_id_},
            {"hall_id", hall_id_},
            {"user_id", user_id_},
            {"start_time", start_time_},
            {"end_time", end_time_},
            {"purpose", purpose_},
            {"status", status_}
        };
    }
};

// DatabaseManager (Singleton, Abstraction)
class DatabaseManager {
private:
    sqlite3* db_;
    static DatabaseManager* instance_;

    DatabaseManager() {
        if (sqlite3_open("lasu_halls.db", &db_) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database");
        }
    }

public:
    static DatabaseManager& getInstance() {
        if (!instance_) instance_ = new DatabaseManager();
        return *instance_;
    }

    sqlite3* getDb() const { return db_; }

    std::vector<Hall> getAllHalls() {
        std::vector<Hall> halls;
        sqlite3_stmt* stmt;
        const char* sql = "SELECT hall_id, name, capacity, facilities, location FROM halls";
        if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                halls.emplace_back(
                    sqlite3_column_int(stmt, 0),
                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                    sqlite3_column_int(stmt, 2),
                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
                );
            }
            sqlite3_finalize(stmt);
        }
        return halls;
    }

    bool createBooking(int hall_id, int user_id, const std::string& start_time,
                      const std::string& end_time, const std::string& purpose) {
        // Check for overlapping bookings
        sqlite3_stmt* stmt;
        std::string sql = "SELECT COUNT(*) FROM bookings WHERE hall_id = ? AND "
                          "((start_time <= ?) AND (end_time >= ?))";
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        sqlite3_bind_int(stmt, 1, hall_id);
        sqlite3_bind_text(stmt, 2, end_time.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, start_time.c_str(), -1, SQLITE_STATIC);
        int count = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
        if (count > 0) {
            return false; // Overlap detected
        }

        // Insert booking
        sql = "INSERT INTO bookings (hall_id, user_id, start_time, end_time, purpose, status) "
              "VALUES (?, ?, ?, ?, ?, 'confirmed')";
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        sqlite3_bind_int(stmt, 1, hall_id);
        sqlite3_bind_int(stmt, 2, user_id);
        sqlite3_bind_text(stmt, 3, start_time.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, end_time.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, purpose.c_str(), -1, SQLITE_STATIC);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }

    json getHallAvailability(int hall_id) {
        json result = json::array();
        sqlite3_stmt* stmt;
        std::string sql = "SELECT start_time, end_time FROM bookings WHERE hall_id = ? AND status = 'confirmed'";
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, hall_id);
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                result.push_back({
                    {"start_time", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))},
                    {"end_time", reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))}
                });
            }
            sqlite3_finalize(stmt);
        }
        return result;
    }

    bool createHall(const std::string& name, int capacity, const std::string& facilities, const std::string& location) {
        sqlite3_stmt* stmt;
        std::string sql = "INSERT INTO halls (name, capacity, facilities, location) VALUES (?, ?, ?, ?)";
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, capacity);
        sqlite3_bind_text(stmt, 3, facilities.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, location.c_str(), -1, SQLITE_STATIC);
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }

    ~DatabaseManager() { sqlite3_close(db_); }
};

DatabaseManager* DatabaseManager::instance_ = nullptr;

// Main function with API routes
int main() {
    crow::App<crow::CORSHandler> app; // Use App with CORSHandler middleware
    DatabaseManager& db = DatabaseManager::getInstance();

   
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global().prefix("*");

    // GET /halls: List all halls
    CROW_ROUTE(app, "/halls").methods("GET"_method)([&db]() {
        auto halls = db.getAllHalls();
        json result = json::array();
        for (const auto& hall : halls) {
            result.push_back(hall.toJson());
        }
        return crow::response(result.dump());
    });

    // POST /bookings: Create a booking
    CROW_ROUTE(app, "/bookings").methods("POST"_method)([&db](const crow::request& req) {
        try {
            auto data = json::parse(req.body);
            int hall_id = data["hall_id"];
            int user_id = data["user_id"];
            std::string start_time = data["start_time"];
            std::string end_time = data["end_time"];
            std::string purpose = data["purpose"];
            if (db.createBooking(hall_id, user_id, start_time, end_time, purpose)) {
                return crow::response(201, "Booking created");
            }
            return crow::response(400, "Booking failed: Overlapping time slot");
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid request: " + std::string(e.what()));
        }
    });

    // GET /halls/<id>/availability: Get hall availability
    CROW_ROUTE(app, "/halls/<int>/availability").methods("GET"_method)([&db](int hall_id) {
        try {
            json result = db.getHallAvailability(hall_id);
            return crow::response(result.dump());
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid request: " + std::string(e.what()));
        }
    });

    // POST /halls: Add a new hall
    CROW_ROUTE(app, "/halls").methods("POST"_method)([&db](const crow::request& req) {
        try {
            auto data = json::parse(req.body);
            std::string name = data["name"];
            int capacity = data["capacity"];
            std::string facilities = data.value("facilities", "");
            std::string location = data.value("location", "");
            if (name.empty() || capacity <= 0) {
                return crow::response(400, "Invalid request: Name and capacity are required");
            }
            if (db.createHall(name, capacity, facilities, location)) {
                return crow::response(201, "Hall created");
            }
            return crow::response(500, "Failed to create hall");
        } catch (const std::exception& e) {
            return crow::response(400, "Invalid request: " + std::string(e.what()));
        }
    });

    app.port(8080).run();
    return 0;
}
