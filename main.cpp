#include <iostream>
#include <vector>

#include "config/db_config.hpp"
#include "database/db_manager.hpp"
#include "models/os_types.hpp"
#include "scheduling/fcfs.hpp"
#include "scheduling/round_robin.hpp"

using namespace std;

int main() {
    // Create the database configuration and manager, then connect to Oracle.
    DBConfig config;
    DatabaseManager database;
    if (!database.connect(config)) {
        return 1;
    }
    // Create the required tables; existing tables are left in place.
    database.initSchema();

    // Build a small sample workload. Each process starts with zeroed statistics.
    vector<Process> workload;
    Process first;
    first.pid = 1;
    first.cpu_burst_time = 6;
    workload.push_back(first);

    Process second;
    second.pid = 2;
    second.arrival_time = 1;
    second.cpu_burst_time = 4;
    workload.push_back(second);

    Process third;
    third.pid = 3;
    third.arrival_time = 2;
    third.cpu_burst_time = 8;
    workload.push_back(third);

    // Run both algorithms against the same workload and save each experiment.
    FCFSScheduler fcfs;
    const int fcfs_experiment_id = fcfs.run(database, workload);
    if (fcfs_experiment_id < 0) {
        cerr << "Could not save the FCFS experiment.\n";
        return 1;
    }
    database.printExperimentSummary(fcfs_experiment_id);

    RoundRobinScheduler round_robin(2);
    const int rr_experiment_id = round_robin.run(database, workload);
    if (rr_experiment_id < 0) {
        cerr << "Could not save the Round Robin experiment.\n";
        return 1;
    }
    database.printExperimentSummary(rr_experiment_id);
    return 0;
}
