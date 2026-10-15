#pragma once

#include "../config/db_config.hpp"
#include "../models/os_types.hpp"
#include <occi.h>

using namespace std;
using namespace oracle::occi;

// Owns the Oracle connection and writes simulator data to database tables.
class DatabaseManager {
private:
    Environment* env = nullptr;
    Connection* conn = nullptr;

    // Runs a SQL statement that does not need to return rows.
    void executeQuery(const string& query);

public:
    DatabaseManager();
    ~DatabaseManager();

    // A connection has one owner, so DatabaseManager objects cannot be copied.
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool connect(const DBConfig& cfg);
    void initSchema();

    // Saves an experiment and returns the ID Oracle assigned to it.
    int createExperiment(const string& type, const string& algo,
                         int quantum, const string& workload_type);
    void insertProcess(int exp_id, const Process& p);
    void insertEvent(int exp_id, const ProcessEvent& e);
    void insertSchedulingResult(int exp_id, const SchedulingResult& res);
    void printExperimentSummary(int exp_id);
};
