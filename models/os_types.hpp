#pragma once

#include <string>

using namespace std;

// Describes one process and the scheduling information collected for it.
class Process {
public:
    int pid = 0;
    int arrival_time = 0;
    int cpu_burst_time = 0;
    int io_burst_time = 0;
    int priority = 1;

    int remaining_time = 0;
    int completion_time = 0;
    int turnaround_time = 0;
    int waiting_time = 0;
    int response_time = -1;
};

// Records when a process used the CPU and what it was doing.
class ProcessEvent {
public:
    int pid = 0;
    int start_tick = 0;
    int end_tick = 0;
    string state;
    string details;
};

// Holds summary measurements calculated after a scheduling run.
class SchedulingResult {
public:
    float avg_waiting_time = 0.0f;
    float avg_turnaround_time = 0.0f;
    float avg_response_time = 0.0f;
    float cpu_utilization = 100.0f;
    float throughput = 0.0f;
    int total_execution_ticks = 0;
};
