#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define MAX_PROCESSES 10

#include "sim_view.h"

void calculateMetrics(sim_view_t *sim)
{
    int totalCpuTimeUsed    = 0;
    int totalWaitingTime    = 0;
    int totalTurnaroundTime = 0;

    for (int i = 0; i < sim->processCount; i++) {
        // Calculate individual process waiting and turnaround times
        sim->processList[i].waitingTime
            = sim->processList[i].completionTime - sim->processList[i].arrivalTime - sim->processList[i].originalCpuBurstTime;
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


void FCFS_Scheduler(sim_view_t *sim)
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
            sim->runningProcessIndex                    = nextProcessIndex;
            sim->processList[sim->runningProcessIndex].state = RUNNING;
            sim->processRunning                         = true;

            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - started running - RUNNING\n", sim->currentTime,
                sim->processList[sim->runningProcessIndex].pid);
            strncat(sim->logContent, logMessage, sizeof(sim->logContent) - strlen(sim->logContent) - 1);
        }
    }
}

void TerminateProcessIfComplete(sim_view_t *sim)
{
    if (sim->processRunning && sim->runningProcessIndex != -1) {
        // Decrement CPU burst time
        sim->processList[sim->runningProcessIndex].cpuBurstTime--;

        // Check if the process has completed
        if (sim->processList[sim->runningProcessIndex].cpuBurstTime <= 0) {
            sim->processList[sim->runningProcessIndex].state          = TERMINATED;
            sim->processRunning                                  = false;
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


void StartScheduler(sim_view_t *sim)
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


void JobQueue(sim_view_t *sim)
{
    for (int i = 0; i < sim->processCount; i++) {
        if (sim->processList[i].arrivalTime == sim->currentTime && sim->processList[i].state == NEW) {
            sim->processList[i].state = READY;
            char logMessage[64];
            snprintf(logMessage, sizeof(logMessage), "%d - P%d - entered the ready queue - READY\n", sim->currentTime,
                sim->processList[i].pid);
            strncat(sim->logContent, logMessage, sizeof(sim->logContent) - strlen(sim->logContent) - 1); // Append log message
        }
    }

    // Immediately check for the next process to run if CPU is free
    if (!sim->processRunning) {
        StartScheduler(sim);
    }
}



void UpdateListViewContent(sim_view_t *sim)
{
    sim->listViewContent[0] = '\0';
    for (int i = 0; i < sim->processCount; i++) {
        char processName[32];
        snprintf(processName, sizeof(processName), "Process %d;", sim->processList[i].pid);
        strncat(sim->listViewContent, processName, sizeof(sim->listViewContent) - strlen(sim->listViewContent) - 1);
    }
}


void UpdateQueueStatus(sim_view_t *sim)
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


void AddProcess(sim_view_t *sim, int pid, int arrivalTime, int ioBurstTime, int cpuBurstTime, int originalCpuBurstTime, int cpuNumber)
{
    if (sim->processCount < MAX_PROCESSES) {
        sim->processList[sim->processCount++] = (Process) { pid, arrivalTime, ioBurstTime, cpuBurstTime,
            originalCpuBurstTime, // Set original burst time
            cpuNumber, NEW };
        UpdateListViewContent(sim);
    }
}


void UpdateProcessInfo(sim_view_t *sim, int index)
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


bool AllProcessesTerminated(sim_view_t *sim)
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




void RunGUI(sim_view_t *sim)
{
    InitWindow(sim->baseWidth, sim->baseHeight, "CPU Scheduler Simulator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        int   width  = GetScreenWidth();
        int   height = GetScreenHeight();
        float scaleX = (float) width / sim->baseWidth;
        float scaleY = (float) height / sim->baseHeight;

        // Calculate the scaled text size based on window height
        int scaledTextSize = (int) (sim->baseTextSize * scaleY * 0.6);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Set the text size for the title label
        GuiSetStyle(DEFAULT, TEXT_SIZE, scaledTextSize + 10); // Add a bit more for the title
        GuiLabel((Rectangle) { 250 * scaleX, 20 * scaleY, 500 * scaleX, 30 * scaleY }, "CPU SCHEDULING SIMULATOR");

        // Set the text size for other labels
        GuiSetStyle(DEFAULT, TEXT_SIZE, scaledTextSize); // Reset text size for normal labels

        // Scheduler combo box
        if (GuiComboBox((Rectangle) { 20 * scaleX, 20 * scaleY, 210 * scaleX, 30 * scaleY },
                "FCFS;RR;SJF;SRTF;Priority Scheduling", &sim->selectedScheduler)) { }

        // Add process function
        if (GuiButton((Rectangle) { 20 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Add Process")) {
            AddProcess(sim, sim->pidCounter++, sim->arrivalTimeInput, sim->ioTimeInput, sim->cpuTimeInput, sim->cpuTimeInput, sim->cpuNumberInput);
        }

        // Delete process function
        if (GuiButton((Rectangle) { 130 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Delete Process")) {
            if (sim->activeItem >= 0 && sim->activeItem < sim->processCount) {
                for (int i = sim->activeItem; i < sim->processCount - 1; i++) {
                    sim->processList[i] = sim->processList[i + 1];
                }
                sim->processCount--;
                sim->activeItem = -1;
                UpdateListViewContent(sim);
                sim->PInfo[0] = '\0';
            } else {
                sim->showMessageBox = true;
                sim->messageType = 2;
            }
        }

        // Toggle edit mode for cpuTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->cpuTimeEdit = true;
            sim->ioTimeEdit = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit = false;
        }

        // Toggle edit mode for ioTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 130 * scaleX, 150 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit = true;
            sim->cpuTimeEdit = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit = false;
        }

        // Toggle edit mode for arrivalTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 130 * scaleX, 190 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit = false;
            sim->cpuTimeEdit = false;
            sim->arrivalTimeEdit = true;
            sim->cpuNumberEdit = false;
        }

        // Toggle edit mode for cpuNumberInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle) { 130 * scaleX, 230 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit = false;
            sim->cpuTimeEdit = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit = true;
        }

        // Input from User
        GuiValueBox((Rectangle) { 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU time\t", &sim->cpuTimeInput,
            0, 300, sim->cpuTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 150 * scaleY, 100 * scaleX, 30 * scaleY }, "IO time\t", &sim->ioTimeInput, 0,
            300, sim->ioTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 190 * scaleY, 100 * scaleX, 30 * scaleY }, "Arrival time\t",
            &sim->arrivalTimeInput, 0, 300, sim->arrivalTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 230 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU burst Num\t",
            &sim->cpuNumberInput, 0, 10, sim->cpuNumberEdit);

        // Process Information
        GuiTextBox((Rectangle) { 20 * scaleX, 280 * scaleY, 200 * scaleX, 90 * scaleY }, sim->PInfo, sizeof(sim->PInfo), false);

        // Process list
        GuiListView((Rectangle) { 20 * scaleX, 360 * scaleY, 200 * scaleX, 200 * scaleY }, sim->listViewContent,
            &sim->scrollIndex, &sim->activeItem);

        // Display log
        GuiLabel((Rectangle) { 250 * scaleX, 70 * scaleY, 500 * scaleX, 20 * scaleY }, "Changes: ");
        GuiTextBox((Rectangle) { 250 * scaleX, 90 * scaleY, 355 * scaleX, 270 * scaleY }, sim->logContent,
            sizeof(sim->logContent), false);
        GuiCheckBox((Rectangle) { 250 * scaleX, 400 * scaleY, 20 * scaleX, 20 * scaleY },
            "Enable Random Context Switching", &sim->contextSwitchingEnabled);

        // Progress Bar for each process
        for (int i = 0; i < sim->processCount; i++) {
            float progress = 1.0f - (float) sim->processList[i].cpuBurstTime / sim->processList[i].originalCpuBurstTime;
            GuiProgressBar((Rectangle) { 615 * scaleX, (90 + 33 * i) * scaleY, 100 * scaleX, 30 * scaleY }, NULL, NULL,
                &progress, 0.0f, 1.0f);
            char label[20];
            sprintf(label, "P%d", i); // Label with process index
            DrawText(label, 730 * scaleX, (95 + 33 * i) * scaleY, 20 * scaleX, DARKGRAY);
        }

        // Label for current process or CPU status
        if (sim->processRunning && sim->runningProcessIndex != -1) {
            char cpuStatus[64];
            snprintf(cpuStatus, sizeof(cpuStatus), "CPU status: P%d ", sim->processList[sim->runningProcessIndex].pid);
            GuiLabel((Rectangle) { 250 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, cpuStatus);
        } else {
            GuiLabel((Rectangle) { 250 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, "CPU status: Idle");
        }

        // Display Queue
        GuiTextBox((Rectangle) { 250 * scaleX, 450 * scaleY, 260 * scaleX, 110 * scaleY }, sim->queueStatus,
            sizeof(sim->queueStatus), false);

        // Display calculated result
        GuiTextBox((Rectangle) { 520 * scaleX, 450 * scaleY, 260 * scaleX, 110 * scaleY }, sim->Performance,
            sizeof(sim->Performance), false);
        GuiLabel((Rectangle) { 520 * scaleX, 430 * scaleY, 260 * scaleX, 20 * scaleY }, "Performance");

        // Start Scheduling
        if (GuiButton((Rectangle) { 250 * scaleX, 370 * scaleY, 200 * scaleX, 20 * scaleY }, "Start")) {
            snprintf(sim->logContent, sizeof(sim->logContent), "Start!\n");
            strncat(sim->logContent, "Timestamp - Pid - Changes - State\n", sizeof(sim->logContent) - strlen(sim->logContent) - 1);
            sim->schedulerStarted = true;
        }

        if (sim->schedulerStarted) {
            JobQueue(sim);
            StartScheduler(sim);
            TerminateProcessIfComplete(sim);
            UpdateQueueStatus(sim);
            sim->currentTime++;
            if (AllProcessesTerminated(sim)) {
                sim->schedulerStarted = false;
                char logMessage[64];
                snprintf(logMessage, sizeof(logMessage), "All processes completed at time %d\n", sim->currentTime);
                strncat(sim->logContent, logMessage, sizeof(sim->logContent) - strlen(sim->logContent) - 1);
                for (int i = 0; i < sim->processCount; i++) {
                    sim->processList[i].state = NEW;
                    sim->processList[i].cpuBurstTime = sim->processList[i].originalCpuBurstTime;
                }
                sim->currentTime = 0;
            }
        }

        if (GuiButton((Rectangle) { 650 * scaleX, 570 * scaleY, 100 * scaleX, 30 * scaleY }, "Export to .csv")) { }

        if (GuiButton((Rectangle) { 20 * scaleX, 580 * scaleY, 100 * scaleX, 30 * scaleY }, "Informations")) {
            sim->showMessageBox = true;
            sim->messageType = 1;
        }

        // Display MessageBox
        if (sim->showMessageBox && sim->messageType == 1) {
            int result = GuiMessageBox((Rectangle) { 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                "#196#CPU SCHEDULING SIMULATOR", "Devs: Pham Tuan Phong 20214039, Tran Quang Huy 20210428", "OK");
            if (result >= 0) {
                sim->showMessageBox = false;
                sim->messageType = 0;
            }
        }
        if (sim->showMessageBox && sim->messageType == 2) {
            int result = GuiMessageBox((Rectangle) { 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                "#196#CPU SCHEDULING SIMULATOR", "Please select a process to delete", "OK");
            if (result >= 0) {
                sim->showMessageBox = false;
                sim->messageType = 0;
            }
        }

        // Display Selected Process Information
        if (sim->activeItem != -1) {
            UpdateProcessInfo(sim, sim->activeItem);
        }

        EndDrawing();
    }

    CloseWindow();
    return;
}


int main()
{
    // Initialize the simulation view structure
    sim_view_t sim = {
        .progress = 0.0f,
        .currentTime = 0,
        .pidCounter = 1,
        .processCount = 0,
        .baseWidth = 800,
        .baseHeight = 630,
        .showMessageBox = false,
        .messageType = 0,
        .selectedScheduler = 0,
        .contextSwitchingEnabled = false,
        .activeItem = -1,
        .scrollIndex = 0,
        .currentSize = 100,
        .logContent = "",
        .queueStatus = "",
        .Performance = "",
        .cpuTimeInput = 0,
        .ioTimeInput = 0,
        .arrivalTimeInput = 0,
        .cpuNumberInput = 0,
        .PInfo = "",
        .listViewContent = "",
        .cpuTimeEdit = false,
        .ioTimeEdit = false,
        .arrivalTimeEdit = false,
        .cpuNumberEdit = false,
        .baseTextSize = 20,
        .schedulerStarted = false,
        .runningProcessIndex = -1,
        .processRunning = false
    };

    // Call the RunGUI function and pass the reference to the sim_view_t instance
    RunGUI(&sim);

    return 0;
}

