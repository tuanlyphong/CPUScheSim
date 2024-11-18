#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define MAX_PROCESSES 10

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int pid;
    int arrivalTime;
    int ioBurstTime;
    int cpuBurstTime;
    int originalCpuBurstTime;
    int cpuNumber;
    ProcessState state;
} Process;

    float progress = 0.0f;
    int currentTime = 0;
    int pidCounter = 1;
    Process processList[MAX_PROCESSES];
    int processCount = 0;
    int baseWidth = 800;
    int baseHeight = 630;
    bool showMessageBox = false;
    int messageType = 0;
    int selectedScheduler = 0;
    bool contextSwitchingEnabled = false;
    int activeItem = -1;
    int scrollIndex = 0;
    float currentSize = 100;
    char logContent[5000] = "";
    char queueStatus[256] = "";
    int cpuTimeInput = 0;
    int ioTimeInput = 0;
    int arrivalTimeInput = 0;
    int cpuNumberInput = 0;
    char PInfo[128] = "";
    char listViewContent[256] = "";
    // Textbox edit states
    bool cpuTimeEdit = false;
    bool ioTimeEdit = false;
    bool arrivalTimeEdit = false;
    bool cpuNumberEdit = false;
    // Define base text size for scaling
    int baseTextSize = 20;
    bool schedulerStarted = false; // Tracks if the scheduler has started
    int runningProcessIndex = -1;
    bool processRunning = false;


void FCFS_Scheduler() {
    if (!processRunning) {
        int earliestArrival = __INT_MAX__;
        int nextProcessIndex = -1;

        // Find the READY process with the earliest arrival time
        for (int i = 0; i < processCount; i++) {
            if (processList[i].state == READY && processList[i].arrivalTime <= currentTime) {
                if (processList[i].arrivalTime < earliestArrival) {
                    earliestArrival = processList[i].arrivalTime;
                    nextProcessIndex = i;
                }
            }
        }

        if (nextProcessIndex != -1) {
            runningProcessIndex = nextProcessIndex;
            processList[runningProcessIndex].state = RUNNING;
            processRunning = true;

            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - started running - RUNNING\n", currentTime, processList[runningProcessIndex].pid);
            strncat(logContent, logMessage, sizeof(logContent) - strlen(logContent) - 1);
        }
    }
}

void TerminateProcessIfComplete() {
    if (processRunning && runningProcessIndex != -1) {
        // Decrement CPU burst time
        processList[runningProcessIndex].cpuBurstTime--;

        // Check if the process has completed
        if (processList[runningProcessIndex].cpuBurstTime <= 0) {
            processList[runningProcessIndex].state = TERMINATED;
            processRunning = false;

            // Log the termination with the correct current time
            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - terminated - TERMINATED\n", currentTime + 1, processList[runningProcessIndex].pid);
            strncat(logContent, logMessage, sizeof(logContent) - strlen(logContent) - 1);

            runningProcessIndex = -1;  // Reset the running process index
        }
    }
}

void StartScheduler(int algorithm) {
    switch (algorithm) {
        case 0:
            FCFS_Scheduler();
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

void JobQueue() {
    for (int i = 0; i < processCount; i++) {
        if (processList[i].arrivalTime == currentTime && processList[i].state == NEW) {
            processList[i].state = READY;
            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - entered the ready queue - READY\n", currentTime, processList[i].pid);
            strncat(logContent, logMessage, sizeof(logContent) - strlen(logContent) - 1); // Append log message
        }
    }
    
    // Immediately check for the next process to run if CPU is free
    if (!processRunning) {
        StartScheduler(selectedScheduler);
    }
}


void UpdateListViewContent() {
    listViewContent[0] = '\0';
    for (int i = 0; i < processCount; i++) {
        char processName[32];
        snprintf(processName, sizeof(processName), "Process %d;", processList[i].pid);
        strncat(listViewContent, processName, sizeof(listViewContent) - strlen(listViewContent) - 1);
    }
}

void UpdateQueueStatus() {
    queueStatus[0] = '\0'; // Clear the queue status

    // Add Job Queue Information
    strncat(queueStatus, "Job Queue: ", sizeof(queueStatus) - strlen(queueStatus) - 1);
    for (int i = 0; i < processCount; i++) {
        if (processList[i].state == NEW) {
            char job[16];
            snprintf(job, sizeof(job), "P%d ", processList[i].pid);
            strncat(queueStatus, job, sizeof(queueStatus) - strlen(queueStatus) - 1);
        }
    }
    strncat(queueStatus, "\n", sizeof(queueStatus) - strlen(queueStatus) - 1);

    // Add Ready Queue Information
    strncat(queueStatus, "Ready Queue: ", sizeof(queueStatus) - strlen(queueStatus) - 1);
    for (int i = 0; i < processCount; i++) {
        if (processList[i].state == READY) {
            char ready[16];
            snprintf(ready, sizeof(ready), "P%d ", processList[i].pid);
            strncat(queueStatus, ready, sizeof(queueStatus) - strlen(queueStatus) - 1);
        }
    }
}

void AddProcess(int pid, int arrivalTime, int ioBurstTime, int cpuBurstTime, int originalCpuBurstTime, int cpuNumber) {
    if (processCount < MAX_PROCESSES) {
        processList[processCount++] = (Process){
            pid, 
            arrivalTime, 
            ioBurstTime, 
            cpuBurstTime, 
            originalCpuBurstTime, // Set original burst time
            cpuNumber, 
            NEW
        };
        UpdateListViewContent();
    }
}


void UpdateProcessInfo(int index) {
    if (index >= 0 && index < processCount) {
        snprintf(PInfo, sizeof(PInfo), 
                 "PID: %d\nArrival Time: %d\nIO Burst Time: %d\nCPU Burst Time: %d\nCPU Burst Num: %d",
                 processList[index].pid, 
                 processList[index].arrivalTime, 
                 processList[index].ioBurstTime, 
                 processList[index].originalCpuBurstTime, // Use original burst time
                 processList[index].cpuNumber);
    } else {
        PInfo[0] = '\0'; // Clear the info box if no valid process is selected
    }
}

bool AllProcessesTerminated() {
    for (int i = 0; i < processCount; i++) {
        if (processList[i].state != TERMINATED) {
            return false; // Found a process not terminated
        }
    }
    return true; // All processes terminated
}



int main() {
    InitWindow(baseWidth, baseHeight, "CPU Scheduler Simulator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        int width = GetScreenWidth();
        int height = GetScreenHeight();
        float scaleX = (float)width / baseWidth;
        float scaleY = (float)height / baseHeight;

        // Calculate the scaled text size based on window height
        int scaledTextSize = (int)(baseTextSize * scaleY * 0.6);

        BeginDrawing();
        ClearBackground(RAYWHITE);
         
        // Set the text size for the title label
        GuiSetStyle(DEFAULT, TEXT_SIZE, scaledTextSize + 10);  // Add a bit more for the title
        GuiLabel((Rectangle){ 250 * scaleX, 20 * scaleY, 500 * scaleX, 30 * scaleY }, "CPU SCHEDULING SIMULATOR");

        // Set the text size for other labels
        GuiSetStyle(DEFAULT, TEXT_SIZE, scaledTextSize); // Reset text size for normal labels
        
        if (GuiComboBox((Rectangle){ 20 * scaleX, 20 * scaleY, 210 * scaleX, 30 * scaleY }, 
                "FCFS;RR;SJF;SRTF;Priority Scheduling", &selectedScheduler)) {}
        
        // Add process function
        if (GuiButton((Rectangle){ 20 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Add Process")) {
        AddProcess(pidCounter++, arrivalTimeInput, ioTimeInput, cpuTimeInput, cpuTimeInput, cpuNumberInput);
        }
        
        // Delete process function
        if (GuiButton((Rectangle){ 130 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Delete Process")) {
            if (activeItem >= 0 && activeItem < processCount) {
                for (int i = activeItem; i < processCount - 1; i++) {
                    processList[i] = processList[i + 1];
                }
                processCount--;
                activeItem = -1;
                UpdateListViewContent();
                PInfo[0] = '\0';
            } else {
                showMessageBox = true;
                messageType = 2;
            }
        }

        // Toggle edit mode for cpuTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            cpuTimeEdit = true;
            ioTimeEdit = false;
            arrivalTimeEdit = false;
            cpuNumberEdit = false;
        }

        // Toggle edit mode for ioTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = true;
            cpuTimeEdit = false;
            arrivalTimeEdit = false;
            cpuNumberEdit = false;
        }
        
        // toggle edit mode for arrivalTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 190 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = false;
            cpuTimeEdit = false;
            arrivalTimeEdit = true;
            cpuNumberEdit = false;
        }

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 230 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = false;
            cpuTimeEdit = false;
            arrivalTimeEdit = false;
            cpuNumberEdit = true;
        }    

        // Input from User
        GuiValueBox((Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU time\t", &cpuTimeInput, 0, 300, cpuTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleX, 30 * scaleY }, "IO time\t", &ioTimeInput, 0, 300, ioTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 190 * scaleY, 100 * scaleX, 30 * scaleY }, "Arrival time\t", &arrivalTimeInput, 0, 300, arrivalTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 230 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU burst Num\t", &cpuNumberInput, 0, 10, cpuNumberEdit);

        // Process Informations
        GuiTextBox((Rectangle){ 20 * scaleX, 280 * scaleY, 200 * scaleX, 90 * scaleY }, PInfo, sizeof(PInfo), false);
        
        // Process list 
        GuiListView((Rectangle){ 20 * scaleX, 360 * scaleY, 200 * scaleX, 200 * scaleY }, listViewContent, &scrollIndex, &activeItem);        
        
        // Display log
        GuiLabel((Rectangle){ 250 * scaleX, 70 * scaleY, 500 * scaleX, 20 * scaleY }, "Changes: ");
        GuiTextBox((Rectangle){ 250 * scaleX, 90 * scaleY, 355 * scaleX, 270 * scaleY }, logContent, sizeof(logContent), false);
        GuiCheckBox((Rectangle){ 250 * scaleX, 400 * scaleY, 20 * scaleX, 20 * scaleY }, "Enable Random Context Switching", &contextSwitchingEnabled);
        
        // Progress Bar
        GuiProgressBar((Rectangle){615 * scaleX, 90 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 1",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 123 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 2",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 156 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 3",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 189 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 4",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 222 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 5",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 255 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 6",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 288 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 7",&progress,0.0f,1.0f);
        GuiProgressBar((Rectangle){615 * scaleX, 321 * scaleY, 100 * scaleX, 30 * scaleY},NULL,"Process 8",&progress,0.0f,1.0f);

        // Label for current process or CPU status
        if (processRunning && runningProcessIndex != -1) {
        char cpuStatus[64];
        snprintf(cpuStatus, sizeof(cpuStatus), "CPU status: P%d ", processList[runningProcessIndex].pid);
        GuiLabel((Rectangle){ 250 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, cpuStatus);
        } else {
        GuiLabel((Rectangle){ 250 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, "CPU status: Idle");
        }
        
        // Display Queue
        GuiTextBox((Rectangle){ 250 * scaleX, 450 * scaleY, 260 * scaleX, 110 * scaleY }, queueStatus, sizeof(queueStatus), false);
        
        // Display caculated result
        GuiTextBox((Rectangle){ 520 * scaleX, 450 * scaleY, 260 * scaleX, 110 * scaleY }, queueStatus, sizeof(queueStatus), false);
        GuiLabel((Rectangle){ 520 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, "Result");
                
        // Start Scheduling
        if (GuiButton((Rectangle){ 250 * scaleX, 370 * scaleY, 200 * scaleX, 20 * scaleY }, "Start")){
        snprintf(logContent, sizeof(logContent), "Start!\n");
        strncat(logContent, "Timestamp - Pid - Changes - State\n", sizeof(logContent) - strlen(logContent) - 1);
        schedulerStarted = true;
        }
        
        if (schedulerStarted) {
            JobQueue();
            StartScheduler(selectedScheduler);
            TerminateProcessIfComplete();
            UpdateQueueStatus();
            currentTime++;
                if (AllProcessesTerminated()) {
                schedulerStarted = false;
                char logMessage[64];
                snprintf(logMessage, sizeof(logMessage), "All processes completed at time %d\n", currentTime);
                strncat(logContent, logMessage, sizeof(logContent) - strlen(logContent) - 1);
                for (int i = 0; i < processCount; i++){
                processList[i].state = NEW;
                processList[i].cpuBurstTime = processList[i].originalCpuBurstTime;
                currentTime = 0;
                }
            }
        }
        
        if (GuiButton((Rectangle){ 650 * scaleX, 570 * scaleY, 100 * scaleX, 30 * scaleY }, "Export to .csv")) {}

        if (GuiButton((Rectangle){ 20 * scaleX, 580 * scaleY, 100 * scaleX, 30 * scaleY }, "Informations")) {
        showMessageBox = true;
        messageType = 1;
        }
        
        // Display MessageBox
        if (showMessageBox && messageType == 1)
            {
                int result = GuiMessageBox((Rectangle){ 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                    "#196#CPU SCHEDULING SIMULATOR", "Devs: Pham Tuan Phong 20214039, Tran Quang Huy 20210428", "OK");
                if (result >= 0) {showMessageBox = false; messageType = 0;} 
            }
        if (showMessageBox && messageType == 2)
            {
                int result = GuiMessageBox((Rectangle){ 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                    "#196#CPU SCHEDULING SIMULATOR", "Please select a process to delete", "OK");
                if (result >= 0) {showMessageBox = false; messageType = 0;}
            }
        // Display Selected Process Informations
        if (activeItem != -1) {
            UpdateProcessInfo(activeItem);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

