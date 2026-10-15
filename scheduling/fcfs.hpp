#pragma once

#include <algorithm>
#include <vector>

#include "scheduler.hpp"

using namespace std;

// First-Come, First-Served runs each process in arrival order until it finishes.
class FCFSScheduler : public Scheduler {
public:
    int run(DatabaseManager& db, vector<Process> processes) override {
        const string workload = classifyWorkload(processes);
        const int experiment_id = db.createExperiment("SCHEDULING", "FCFS", 0, workload);
        if (experiment_id < 0) return experiment_id;

        // Stable sorting keeps the original order when two processes arrive together.
        stable_sort(processes.begin(), processes.end(),
                         [](const Process& lhs, const Process& rhs) {
                             return lhs.arrival_time < rhs.arrival_time;
                         });

        int current_time = 0;
        int busy_ticks = 0;
        for (auto& process : processes) {
            // Negative burst lengths are invalid, so treat them as zero work.
            if (process.cpu_burst_time < 0) {
                process.cpu_burst_time = 0;
            }
            // If the CPU is idle until this process arrives, skip ahead to its arrival.
            current_time = max(current_time, process.arrival_time);
            const int start_time = current_time;
            // A process responds when it first gets the CPU; for FCFS this is its wait.
            if (process.response_time < 0) {
                process.response_time = start_time - process.arrival_time;
            }
            current_time += process.cpu_burst_time;
            busy_ticks += process.cpu_burst_time;

            // Work out the final timing statistics after the process finishes.
            process.remaining_time = 0;
            process.completion_time = current_time;
            process.turnaround_time = process.completion_time - process.arrival_time;
            process.waiting_time = process.turnaround_time - process.cpu_burst_time;

            // Keep an event record so the run can be inspected later.
            ProcessEvent event;
            event.pid = process.pid;
            event.start_tick = start_time;
            event.end_tick = current_time;
            event.state = "RUNNING";
            event.details = "FCFS execution";
            db.insertEvent(experiment_id, event);
        }

        finalizeAndSave(db, experiment_id, processes, current_time, busy_ticks);
        return experiment_id;
    }
};
