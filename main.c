#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct {
    int pid;
    int requestTime;
    int ioBurstTime;
    int cpuBurstTime;
    int cpuNumber;
} Process;

#define MAX_PROCESSES 10
Process processList[MAX_PROCESSES];
    int processCount = 0;
    int baseWidth = 800;
    int baseHeight = 630;
    bool showMessageBox = false;
    int selectedScheduler = 0;
    bool contextSwitchingEnabled = false;
    int activeItem = -1;
    int scrollIndex = 0;
    float progressValue = 0.0f;
    float currentSize = 100;
    char logContent[256] = "";
    char queueStatus[256] = "";
    int cpuTimeInput = 0;
    int ioTimeInput = 0;
    int requestTimeInput = 0;
    int cpuNumberInput = 0;
    char PInfo[128] = "";
    char listViewContent[256] = "";
    // Textbox edit states
    bool cpuTimeEdit = false;
    bool ioTimeEdit = false;
    bool requestTimeEdit = false;
    bool cpuNumberEdit = false;
    // Define base text size for scaling
    int baseTextSize = 20;

void UpdateListViewContent() {
    listViewContent[0] = '\0';  // Clear the content
    for (int i = 0; i < processCount; i++) {
        char processName[32];
        snprintf(processName, sizeof(processName), "Process %d;", processList[i].pid);
        strncat(listViewContent, processName, sizeof(listViewContent) - strlen(listViewContent) - 1);
    }
}

void AddProcess(int pid, int requestTime, int ioBurstTime, int cpuBurstTime, int cpuNumber) {
    if (processCount < MAX_PROCESSES) {
        processList[processCount++] = (Process){ pid, requestTime, ioBurstTime, cpuBurstTime, cpuNumber };
        UpdateListViewContent();
    }
}

void UpdateProcessInfo(int index) {
    if (index >= 0 && index < processCount) {
        snprintf(PInfo, sizeof(PInfo), "PID: %d\nRequest Time: %d\nIO Burst Time: %d\nCPU Burst Time: %d\nCPU Burst Num: %d",
                 processList[index].pid, processList[index].requestTime, 
                 processList[index].ioBurstTime, processList[index].cpuBurstTime, processList[index].cpuNumber);
    }
}

int main() {
    int pidCounter = 1;
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
        AddProcess(pidCounter++, requestTimeInput, ioTimeInput, cpuTimeInput, cpuNumberInput);
        }
        
        // Delete process function
        if (GuiButton((Rectangle){ 130 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Delete Process")) {



      
        }

        // Toggle edit mode for cpuTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            cpuTimeEdit = true;
            ioTimeEdit = false;
            requestTimeEdit = false;
            cpuNumberEdit = false;
        }

        // Toggle edit mode for ioTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = true;
            cpuTimeEdit = false;
            requestTimeEdit = false;
            cpuNumberEdit = false;
        }
        
        // toggle edit mode for requestTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 190 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = false;
            cpuTimeEdit = false;
            requestTimeEdit = true;
            cpuNumberEdit = false;
        }

        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 230 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ioTimeEdit = false;
            cpuTimeEdit = false;
            requestTimeEdit = false;
            cpuNumberEdit = true;
        }    

        // Input from User
        GuiValueBox((Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU time\t", &cpuTimeInput, 0, 300, cpuTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleX, 30 * scaleY }, "IO time\t", &ioTimeInput, 0, 300, ioTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 190 * scaleY, 100 * scaleX, 30 * scaleY }, "Request time\t", &requestTimeInput, 0, 10, requestTimeEdit);
        GuiValueBox((Rectangle){ 130 * scaleX, 230 * scaleY, 100 * scaleX, 30 * scaleY }, "CPU burst Num\t", &cpuNumberInput, 0, 10, cpuNumberEdit);

        // Process Informations
        GuiTextBox((Rectangle){ 20 * scaleX, 280 * scaleY, 200 * scaleX, 90 * scaleY }, PInfo, sizeof(PInfo), false);
        
        // Process list 
        GuiListView((Rectangle){ 20 * scaleX, 360 * scaleY, 200 * scaleX, 200 * scaleY }, listViewContent, &scrollIndex, &activeItem);        
        
        // Display log
        GuiLabel((Rectangle){ 250 * scaleX, 70 * scaleY, 500 * scaleX, 20 * scaleY }, "Changes: ");
        GuiTextBox((Rectangle){ 250 * scaleX, 90 * scaleY, 500 * scaleX, 200 * scaleY }, logContent, sizeof(logContent), false);
        GuiCheckBox((Rectangle){ 250 * scaleX, 330 * scaleY, 20 * scaleX, 20 * scaleY }, "Enable Random Context Switching", &contextSwitchingEnabled);

        // Label for current process or CPU status
        GuiLabel((Rectangle){ 250 * scaleX, 360 * scaleY, 500 * scaleX, 20 * scaleY }, "Current Process: P1 (Running)");
        
        // Display Queue
        GuiTextBox((Rectangle){ 250 * scaleX, 390 * scaleY, 500 * scaleX, 100 * scaleY }, queueStatus, sizeof(queueStatus), false);
        
        // Start Scheduling
        if (GuiButton((Rectangle){ 250 * scaleX, 300 * scaleY, 200 * scaleX, 20 * scaleY }, "Start")){}
        
        if (GuiButton((Rectangle){ 650 * scaleX, 500 * scaleY, 100 * scaleX, 30 * scaleY }, "Export to .csv")) {}

        if (GuiButton((Rectangle){ 20 * scaleX, 580 * scaleY, 100 * scaleX, 30 * scaleY }, "Informations")) {
        showMessageBox = true;
        }
        
        // Display MessageBox
        if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                    "#196#CPU SCHEDULING SIMULATOR", "Devs: Pham Tuan Phong 20214039, Tran Quang Huy 20210428", "OK");
                if (result >= 0) showMessageBox = false;
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

