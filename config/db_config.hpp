#pragma once

#include <string>

using namespace std;

// Stores the information DatabaseManager needs to connect to Oracle.
class DBConfig {
public:
    // Replace these sample credentials with values for your Oracle database.
    string user = "system";
    string password = "your_password";
    string connect_string = "localhost:1521/FREE";
};
