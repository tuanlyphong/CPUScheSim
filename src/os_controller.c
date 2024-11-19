#include "os_controller.h"

void OS_CTRL_Start(os_controller_t* controller)
{
    sim_view_t* sim = &controller->view;
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
            OS_CTRL_AddProcess(controller);
        }

        // Delete process function
        if (GuiButton((Rectangle) { 130 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Delete Process")) {
            OS_CTRL_DeleteProcess(controller);
        }

        // Toggle edit mode for cpuTimeInput textbox
        if (CheckCollisionPointRec(
                GetMousePosition(), (Rectangle) { 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->cpuTimeEdit     = true;
            sim->ioTimeEdit      = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit   = false;
        }

        // Toggle edit mode for ioTimeInput textbox
        if (CheckCollisionPointRec(
                GetMousePosition(), (Rectangle) { 130 * scaleX, 150 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit      = true;
            sim->cpuTimeEdit     = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit   = false;
        }

        // Toggle edit mode for arrivalTimeInput textbox
        if (CheckCollisionPointRec(
                GetMousePosition(), (Rectangle) { 130 * scaleX, 190 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit      = false;
            sim->cpuTimeEdit     = false;
            sim->arrivalTimeEdit = true;
            sim->cpuNumberEdit   = false;
        }

        // Toggle edit mode for cpuNumberInput textbox
        if (CheckCollisionPointRec(
                GetMousePosition(), (Rectangle) { 130 * scaleX, 230 * scaleY, 100 * scaleY, 30 * scaleY })
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sim->ioTimeEdit      = false;
            sim->cpuTimeEdit     = false;
            sim->arrivalTimeEdit = false;
            sim->cpuNumberEdit   = true;
        }

        // Input from User
        GuiValueBox((Rectangle) { 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU time\t",
            &sim->cpuTimeInput, 0, 300, sim->cpuTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 150 * scaleY, 100 * scaleX, 30 * scaleY }, "IO time\t",
            &sim->ioTimeInput, 0, 300, sim->ioTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 190 * scaleY, 100 * scaleX, 30 * scaleY }, "Arrival time\t",
            &sim->arrivalTimeInput, 0, 300, sim->arrivalTimeEdit);
        GuiValueBox((Rectangle) { 130 * scaleX, 230 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU burst Num\t",
            &sim->cpuNumberInput, 0, 10, sim->cpuNumberEdit);

        // Process Information
        GuiTextBox((Rectangle) { 20 * scaleX, 280 * scaleY, 200 * scaleX, 90 * scaleY }, sim->PInfo, sizeof(sim->PInfo),
            false);

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
            strncat(sim->logContent, "Timestamp - Pid - Changes - State\n",
                sizeof(sim->logContent) - strlen(sim->logContent) - 1);
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
                    sim->processList[i].state        = NEW;
                    sim->processList[i].cpuBurstTime = sim->processList[i].originalCpuBurstTime;
                }
                sim->currentTime = 0;
            }
        }

        if (GuiButton((Rectangle) { 650 * scaleX, 570 * scaleY, 100 * scaleX, 30 * scaleY }, "Export to .csv")) { }

        if (GuiButton((Rectangle) { 20 * scaleX, 580 * scaleY, 100 * scaleX, 30 * scaleY }, "Informations")) {
            sim->showMessageBox = true;
            sim->messageType    = 1;
        }

        // Display MessageBox
        if (sim->showMessageBox && sim->messageType == 1) {
            int result = GuiMessageBox((Rectangle) { 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                "#196#CPU SCHEDULING SIMULATOR", "Devs: Pham Tuan Phong 20214039, Tran Quang Huy 20210428", "OK");
            if (result >= 0) {
                sim->showMessageBox = false;
                sim->messageType    = 0;
            }
        }
        if (sim->showMessageBox && sim->messageType == 2) {
            int result = GuiMessageBox((Rectangle) { 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                "#196#CPU SCHEDULING SIMULATOR", "Please select a process to delete", "OK");
            if (result >= 0) {
                sim->showMessageBox = false;
                sim->messageType    = 0;
            }
        }

        // Display Selected Process Information
        if (sim->activeItem != -1) {
            UpdateProcessInfo(sim, sim->activeItem);
        }

        EndDrawing();
    }

    CloseWindow();
}

void OS_CTRL_AddProcess(os_controller_t* controller)
{
    sim_view_t* sim = &controller->view;
    if (sim->processCount < MAX_PROCESSES) {
        sim->processList[sim->processCount++] = (Process) { sim->pidCounter++, sim->arrivalTimeInput, sim->ioTimeInput,
            sim->cpuTimeInput, sim->cpuTimeInput, sim->cpuNumberInput, NEW };
        UpdateListViewContent(sim);
    }
}

void OS_CTRL_DeleteProcess(os_controller_t* controller)
{
    sim_view_t* sim = &controller->view;

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
        sim->messageType    = 2;
    }
}
