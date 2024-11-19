#include "sim_view.h"
#include "os_controller.h"
#include <stdio.h>
#include <string.h>

void calculateMetrics(sim_view_t* sim)
{
    int totalCpuTimeUsed    = 0;
    int totalWaitingTime    = 0;
    int totalTurnaroundTime = 0;

    for (int i = 0; i < sim->processCount; i++) {
        // Calculate individual process waiting and turnaround times
        sim->processList[i].waitingTime = sim->processList[i].completionTime - sim->processList[i].arrivalTime
                                          - sim->processList[i].originalCpuBurstTime;
        sim->processList[i].turnaroundTime = sim->processList[i].completionTime - sim->processList[i].arrivalTime;

        // Sum up CPU time, waiting time, and turnaround time
        totalCpuTimeUsed += sim->processList[i].originalCpuBurstTime;
        totalWaitingTime += sim->processList[i].waitingTime;
        totalTurnaroundTime += sim->processList[i].turnaroundTime;
    }

    // Calculate CPU Utilization, Throughput, Average Waiting Time, and Average Turnaround Time
    float cpuUtilization        = ((float) totalCpuTimeUsed / sim->currentTime) * 100.0f;
    float throughput            = (float) sim->processCount / sim->currentTime;
    float averageWaitingTime    = (float) totalWaitingTime / sim->processCount;
    float averageTurnaroundTime = (float) totalTurnaroundTime / sim->processCount;

    // Display the calculated metrics
    sprintf(sim->Performance,
        "CPU Utilization: %.2f%%\n"
        "Throughput: %.2f processes/unit time\n"
        "Average Waiting Time: %.2f units\n"
        "Average Turnaround Time: %.2f units",
        cpuUtilization, throughput, averageWaitingTime, averageTurnaroundTime);
}


void FCFS_Scheduler(sim_view_t* sim)
{
    if (!sim->processRunning) {
        int earliestArrival  = __INT_MAX__;
        int nextProcessIndex = -1;

        // Find the READY process with the earliest arrival time
        for (int i = 0; i < sim->processCount; i++) {
            if (sim->processList[i].state == READY && sim->processList[i].arrivalTime <= sim->currentTime) {
                if (sim->processList[i].arrivalTime < earliestArrival) {
                    earliestArrival  = sim->processList[i].arrivalTime;
                    nextProcessIndex = i;
                }
            }
        }

        if (nextProcessIndex != -1) {
            sim->runningProcessIndex                         = nextProcessIndex;
            sim->processList[sim->runningProcessIndex].state = RUNNING;
            sim->processRunning                              = true;

            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - started running - RUNNING\n", sim->currentTime,
                sim->processList[sim->runningProcessIndex].pid);
            strncat(sim->logContent, logMessage, sizeof(sim->logContent) - strlen(sim->logContent) - 1);
        }
    }
}

void TerminateProcessIfComplete(sim_view_t* sim)
{
    if (sim->processRunning && sim->runningProcessIndex != -1) {
        // Decrement CPU burst time
        sim->processList[sim->runningProcessIndex].cpuBurstTime--;

        // Check if the process has completed
        if (sim->processList[sim->runningProcessIndex].cpuBurstTime <= 0) {
            sim->processList[sim->runningProcessIndex].state          = TERMINATED;
            sim->processRunning                                       = false;
            sim->processList[sim->runningProcessIndex].completionTime = sim->currentTime + 1;

            // Log the termination with the correct current time
            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - terminated - TERMINATED\n", sim->currentTime + 1,
                sim->processList[sim->runningProcessIndex].pid);
            strncat(sim->logContent, logMessage, sizeof(sim->logContent) - strlen(sim->logContent) - 1);

            sim->runningProcessIndex = -1; // Reset the running process index
        }
    }
}


void StartScheduler(sim_view_t* sim)
{
    switch (sim->selectedScheduler) {
    case 0:
        FCFS_Scheduler(sim);
        break;
    case 1:
        // Implement Round-Robin scheduling logic here
        break;
    case 2:
        // Implement SJF scheduling logic here
        break;
    case 3:
        // Implement SRTF scheduling logic here
        break;
    case 4:
        // Implement Priority scheduling logic here
        break;
    default:
        break;
    }
}


void JobQueue(sim_view_t* sim)
{
    for (int i = 0; i < sim->processCount; i++) {
        if (sim->processList[i].arrivalTime == sim->currentTime && sim->processList[i].state == NEW) {
            sim->processList[i].state = READY;
            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - entered the ready queue - READY\n", sim->currentTime,
                sim->processList[i].pid);
            strncat(sim->logContent, logMessage,
                sizeof(sim->logContent) - strlen(sim->logContent) - 1); // Append log message
        }
    }

    // Immediately check for the next process to run if CPU is free
    if (!sim->processRunning) {
        StartScheduler(sim);
    }
}



void UpdateListViewContent(sim_view_t* sim)
{
    sim->listViewContent[0] = '\0';
    for (int i = 0; i < sim->processCount; i++) {
        char processName[32];
        snprintf(processName, sizeof(processName), "Process %d;", sim->processList[i].pid);
        strncat(sim->listViewContent, processName, sizeof(sim->listViewContent) - strlen(sim->listViewContent) - 1);
    }
}


void UpdateQueueStatus(sim_view_t* sim)
{
    sim->queueStatus[0] = '\0'; // Clear the queue status

    // Add Job Queue Information
    strncat(sim->queueStatus, "Job Queue: ", sizeof(sim->queueStatus) - strlen(sim->queueStatus) - 1);
    for (int i = 0; i < sim->processCount; i++) {
        if (sim->processList[i].state == NEW) {
            char job[16];
            snprintf(job, sizeof(job), "P%d ", sim->processList[i].pid);
            strncat(sim->queueStatus, job, sizeof(sim->queueStatus) - strlen(sim->queueStatus) - 1);
        }
    }
    strncat(sim->queueStatus, "\n", sizeof(sim->queueStatus) - strlen(sim->queueStatus) - 1);

    // Add Ready Queue Information
    strncat(sim->queueStatus, "Ready Queue: ", sizeof(sim->queueStatus) - strlen(sim->queueStatus) - 1);
    for (int i = 0; i < sim->processCount; i++) {
        if (sim->processList[i].state == READY) {
            char ready[16];
            snprintf(ready, sizeof(ready), "P%d ", sim->processList[i].pid);
            strncat(sim->queueStatus, ready, sizeof(sim->queueStatus) - strlen(sim->queueStatus) - 1);
        }
    }
}


void UpdateProcessInfo(sim_view_t* sim, int index)
{
    if (index >= 0 && index < sim->processCount) {
        snprintf(sim->PInfo, sizeof(sim->PInfo),
            "PID: %d\nArrival Time: %d\nIO Burst Time: %d\nCPU Burst Time: %d\nCPU Burst Num: %d",
            sim->processList[index].pid, sim->processList[index].arrivalTime, sim->processList[index].ioBurstTime,
            sim->processList[index].originalCpuBurstTime, // Use original burst time
            sim->processList[index].cpuNumber);
    } else {
        sim->PInfo[0] = '\0'; // Clear the info box if no valid process is selected
    }
}


bool AllProcessesTerminated(sim_view_t* sim)
{
    for (int i = 0; i < sim->processCount; i++) {
        if (sim->processList[i].state != TERMINATED) {
            return false; // Found a process not terminated
        }
    }
    int totalSimulationTime = sim->currentTime;
    calculateMetrics(sim);
    return true; // All processes terminated
}
