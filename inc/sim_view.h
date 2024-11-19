#ifndef SIM_VIEW_H
#define SIM_VIEW_H

#include "stdbool.h"

#define MAX_PROCESSES 100  // Define the maximum number of processes (if it's not already defined)

typedef enum { NEW, READY, RUNNING, WAITING, TERMINATED } ProcessState;

typedef struct {
    int          pid;
    int          arrivalTime;
    int          ioBurstTime;
    int          cpuBurstTime;
    int          originalCpuBurstTime;
    int          cpuNumber;
    int          completionTime;
    int          waitingTime;
    int          turnaroundTime;
    ProcessState state;
} Process;


typedef struct {
    float   progress;               // Progress of the simulation
    int     currentTime;            // Current time in the simulation
    int     pidCounter;             // Process ID counter
    Process processList[MAX_PROCESSES];  // List of processes
    int     processCount;           // Number of processes in the list
    int     baseWidth;              // Base window width
    int     baseHeight;             // Base window height
    bool    showMessageBox;         // Flag for displaying message box
    int     messageType;            // Message type (for the message box)
    int     selectedScheduler;      // The selected scheduler type
    bool    contextSwitchingEnabled; // Flag for enabling context switching
    int     activeItem;             // The currently active item in the simulation
    int     scrollIndex;            // Index for scrolling
    float   currentSize;            // Current size scaling factor
    char    logContent[5000];       // Content of the simulation log
    char    queueStatus[256];       // Status of the queue
    char    Performance[256];       // Performance metrics
    int     cpuTimeInput;           // CPU time input for new processes
    int     ioTimeInput;            // I/O time input for new processes
    int     arrivalTimeInput;       // Arrival time input for new processes
    int     cpuNumberInput;         // CPU number input (if applicable)
    char    PInfo[128];             // Process information
    char    listViewContent[256];   // List view content (for process details)
    // Textbox edit states
    bool cpuTimeEdit;              // Flag for editing CPU time
    bool ioTimeEdit;               // Flag for editing I/O time
    bool arrivalTimeEdit;          // Flag for editing arrival time
    bool cpuNumberEdit;            // Flag for editing CPU number
    // Define base text size for scaling
    int  baseTextSize;             // Base text size for scaling
    bool schedulerStarted;         // Flag indicating if the scheduler has started
    int  runningProcessIndex;      // Index of the currently running process
    bool processRunning;           // Flag indicating if a process is running
} sim_view_t;

#endif