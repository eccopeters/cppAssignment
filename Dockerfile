FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files and dependencies
COPY main.cpp .
COPY sqlite3.c .
COPY sqlite3.h .
COPY include/ include/

# Compile SQLite
RUN gcc -c sqlite3.c -o sqlite3.o

# Compile the application
RUN g++ -std=c++17 main.cpp sqlite3.o -I/app/include -pthread -DASIO_STANDALONE -o lasu_hall_management

# Expose port
EXPOSE 8080

# Command to run the application
CMD ["./lasu_hall_management"]