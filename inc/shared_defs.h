/**
 * @file shared_defs.h
 * @brief Contains shared enumerations and definitions for the CPU Scheduler Simulator.
 *
 * This file defines key enums representing program states, scheduling algorithms, and
 * process states used throughout the simulation. These enums provide a standardized
 * way to manage program behavior and states.
 */

#ifndef SHARED_DEFS_H
#define SHARED_DEFS_H

// Enum representing the possible states of a program during execution
typedef enum {
    PROG_STATE_CPU, // Program is using the CPU
    PROG_STATE_IO   // Program is performing I/O operations
} program_state_e;

// Enum representing supported scheduling algorithms
typedef enum {
    SCHED_ALGO_FCFS,      // First-Come, First-Served
    SCHED_ALGO_RR,        // Round Robin
    SCHED_ALGO_SJF,       // Shortest Job First
    SCHED_ALGO_SRTF,      // Shortest Remaining Time First
    SCHED_ALGO_PRIO_SCHED // Priority Scheduling
} scheduler_algorithm_e;

// Enum representing the lifecycle states of a process
typedef enum {
    PROC_STATE_NEW,       // Process is newly created
    PROC_STATE_READY,     // Process is ready to run
    PROC_STATE_RUNNING,   // Process is currently executing
    PROC_STATE_WAITING,   // Process is waiting for resources
    PROC_STATE_TERMINATED // Process has finished execution
} process_state_e;

#endif
