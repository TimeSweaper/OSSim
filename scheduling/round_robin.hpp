#pragma once

#include <algorithm>
#include <queue>
#include <vector>

#include "scheduler.hpp"

using namespace std;

// Round Robin gives each ready process a short, repeating turn on the CPU.
class RoundRobinScheduler : public Scheduler {
private:
    int time_quantum;

public:
    // A quantum is the maximum number of ticks a process can run in one turn.
    explicit RoundRobinScheduler(int quantum = 2) : time_quantum(quantum) {}

    int run(DatabaseManager& db, vector<Process> processes) override {
        if (time_quantum <= 0) {
            return -1;
        }

        const string workload = classifyWorkload(processes);
        const int experiment_id = db.createExperiment(
            "SCHEDULING", "ROUND_ROBIN", time_quantum, workload);
        if (experiment_id < 0) {
            return experiment_id;
        }

        // Sort by arrival so we can add new processes to the ready queue in order.
        stable_sort(processes.begin(), processes.end(),
                    [](const Process& lhs, const Process& rhs) {
                        return lhs.arrival_time < rhs.arrival_time;
                    });

        queue<size_t> ready;
        size_t next_process = 0;
        int current_time = 0;
        int busy_ticks = 0;

        // Initialize the amount of CPU work left for each process.
        for (auto& process : processes) {
            process.cpu_burst_time = max(0, process.cpu_burst_time);
            process.remaining_time = process.cpu_burst_time;
            process.response_time = -1;
        }

        while (next_process < processes.size() || !ready.empty()) {
            // When nobody is ready, advance time until the next process arrives.
            if (ready.empty() && next_process < processes.size()) {
                current_time = max(current_time,
                                   processes[next_process].arrival_time);
            }

            while (next_process < processes.size() &&
                   processes[next_process].arrival_time <= current_time) {
                ready.push(next_process);
                ++next_process;
            }

            if (ready.empty()) {
                continue;
            }

            const size_t process_index = ready.front();
            ready.pop();
            Process& process = processes[process_index];

            // Remember the first time this process received CPU time.
            if (process.response_time < 0) {
                process.response_time = current_time - process.arrival_time;
            }

            const int start_time = current_time;
            const int run_ticks = min(time_quantum, process.remaining_time);
            current_time += run_ticks;
            busy_ticks += run_ticks;
            process.remaining_time -= run_ticks;

            // Store this CPU turn as an event, including partial turns.
            ProcessEvent event;
            event.pid = process.pid;
            event.start_tick = start_time;
            event.end_tick = current_time;
            event.state = "RUNNING";
            event.details = "Round Robin CPU turn";
            db.insertEvent(experiment_id, event);

            // Add processes that arrived while this process used its time slice.
            while (next_process < processes.size() &&
                   processes[next_process].arrival_time <= current_time) {
                ready.push(next_process);
                ++next_process;
            }

            if (process.remaining_time > 0) {
                // The process did not finish, so it waits for its next turn.
                ready.push(process_index);
            } else {
                process.completion_time = current_time;
                process.turnaround_time =
                    process.completion_time - process.arrival_time;
                process.waiting_time =
                    process.turnaround_time - process.cpu_burst_time;
            }
        }

        finalizeAndSave(db, experiment_id, processes, current_time, busy_ticks);
        return experiment_id;
    }
};
