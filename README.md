# Hall Management System Backend

## Overview

The ** Hall Management System Backend ** is RESTful API designed to manage lecture halls and their bookings at Lagos State University (LASU)

### Key Features

- Create and list lecute halls with details (name, capacity, facilties, location).
- Create bookings with validation to prevent time slot overlaps.
- Retrieve hall availability and all confirmed bookings.
- Persistent SQLite database storage.
- Lightweight and effiecient, leveraging c++ for performance.

### Architecture

- **Language**: C++.
- **Framework**: Crow (RESTful Application Interface).
- **Database**: SQLite
- **Libraries**: nlohmann/json, ASIO
- **Deployment**: Docker, Render



## API Endpoints

The API IS hosted on http://localhost:8000

### GET /halls

- **Description**: Retrieves all list of all halls from the "halls" table .
- **Method**: GET .
- **Response**: {
- **Status**: 200 .

```json
[
  {
    "hall_id": 1,
    "name": "Science Hall 1",
    "capacity": 450,
    "facilities": "Smart board, WIFI, AC",
    "location": "Bola Ahmed Tinubu, New Science Complex"
  },
  {
    "hall_id": 2,
    "name": "SL Edu",
    "capacity": 200,
    "facilities": "AC",
    "location": "SL Edu Hall, Opposite Benson Hall, Science"
  }
]
```

}

- **Empty Case**:

```json
[]
```

- **Error**: {
- **status**: "500 Internal Server Error"
- **message**: "Database Error"
  }

### POST /halls

- **Description**: Creates a new hall in the "halls" table.
- **Method**: POST.

- **Request**: {

```json
{
  "name": "Science Hall 1",
  "capacity": 450,
  "facilities": "Smart board, WIFI, AC",
  "location": "Bola Ahmed Tinubu, New Science Complex"
}
```

}

- **Response**: { - **Status**: 201

```json
{
  "Body": "Hall Created Successfully"
}
```

}

- **Error**: {
- **status**: "500 Internal Server Error"
- **message**: "Database Error"
  }

### POST /bookings

- **Description**: Creates a new hall in the "halls" table.
- **Method**: POST.

- **Request**: {

```json
     {
           "hall_id": integer,
           "name": integer,
           "start_time": "string", //Required, format: "YYYY-MM-DD HH:MM:SS"
           "end_time": "string", //Required, format: "YYYY-MM-DD HH:MM:SS"
           "purpose": "string" //Required
     }
```

}

- **Response**: { - **Status**: 201

```json
{
  "Body": "Hall Created Successfully"
}
```

}

- **Error**: {
- **status**: "500 Internal Server Error"
- **message**: "Database Error"
  }

### GET /halls/:id/availability

- **Description**: Creates a new hall in the "halls" table.
- **Method**: GET.

- **Path**: id (integer)

- **Response**: { - **Status**: 201

```json
[
  {
    "start_time": "2025-07-15 09:00:00", //Required, format: "YYYY-MM-DD HH:MM:SS"
    "end_time": "2025-07-15 09:00:00" //Required, format: "YYYY-MM-DD HH:MM:SS"
  },
  {
    "start_time": "2025-07-15 09:00:00", //Required, format: "YYYY-MM-DD HH:MM:SS"
    "end_time": "2025-07-15 09:00:00" //Required, format: "YYYY-MM-DD HH:MM:SS"
  }
]
```

}

- **Error**: {
- **status**: "500 Internal Server Error"
- **message**: "Database Error"
  }

### Authors

-**Ekojikoko Oghenemudia Peter**: Primary Developer.
-Github: https://github.com/eccopeters 
