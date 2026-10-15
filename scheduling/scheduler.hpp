#pragma once

#include <string>
#include <vector>

#include "../database/db_manager.hpp"
#include "../models/os_types.hpp"

using namespace std;

// Shared behavior for scheduling algorithms such as FCFS and round robin.
class Scheduler {
protected:
    // Compare total CPU and I/O burst times to label the experiment workload.
    string classifyWorkload(const vector<Process>& processes) const {
        long long total_cpu = 0;
        long long total_io = 0;
        // Save each finished process, and add its measurements to the totals.
        for (const auto& process : processes) {
            total_cpu += process.cpu_burst_time;
            total_io += process.io_burst_time;
        }
        if (total_io == 0 || total_cpu > total_io * 2) return "CPU_BOUND";
        if (total_io > total_cpu * 2) return "IO_BOUND";
        return "MIXED";
    }

    // Save every process and calculate averages for the completed run.
    void finalizeAndSave(DatabaseManager& db, int experiment_id,
                         vector<Process>& processes,
                         int total_ticks, int busy_ticks) {
        float waiting_sum = 0.0f;
        float turnaround_sum = 0.0f;
        float response_sum = 0.0f;

        for (const auto& process : processes) {
            db.insertProcess(experiment_id, process);
            waiting_sum += process.waiting_time;
            turnaround_sum += process.turnaround_time;
            response_sum += process.response_time < 0 ? 0 : process.response_time;
        }

        // Divide totals by the process count to get averages; avoid dividing by zero.
        SchedulingResult result;
        if (!processes.empty()) {
            const float count = static_cast<float>(processes.size());
            result.avg_waiting_time = waiting_sum / count;
            result.avg_turnaround_time = turnaround_sum / count;
            result.avg_response_time = response_sum / count;
            result.throughput = total_ticks > 0 ? count / total_ticks : 0.0f;
        }
        // CPU utilization is the share of elapsed time when the CPU was working.
        result.total_execution_ticks = total_ticks;
        result.cpu_utilization = total_ticks > 0
            ? static_cast<float>(busy_ticks) * 100.0f / total_ticks
            : 100.0f;
        db.insertSchedulingResult(experiment_id, result);
    }

public:
    // Each scheduling algorithm implements run() with its own scheduling rules.
    virtual int run(DatabaseManager& db, vector<Process> processes) = 0;
    virtual ~Scheduler() = default;
};
