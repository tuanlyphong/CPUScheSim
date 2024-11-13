#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
    int baseWidth = 800;
    int baseHeight = 600;
    InitWindow(baseWidth, baseHeight, "CPU Scheduler Simulator");
    bool showMessageBox = false;
    int selectedScheduler = 0;
    bool contextSwitchingEnabled = false;
    int activeItem = -1;
    int scrollIndex = 0;
    float progressValue = 0.0f;
    float currentSize = 100;
    char logContent[256] = "";
    char queueStatus[256] = "";
    char arrivalTimeInput[10] = "";
    char burstTimeInput[10] = "";
    char PInfo[10] = "";
    // Textbox edit states
    bool arrivalTimeEdit = false;
    bool burstTimeEdit = false;

    // Define base text size for scaling
    int baseTextSize = 20;

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

        if (GuiButton((Rectangle){ 20 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Add Process")) {}
        if (GuiButton((Rectangle){ 130 * scaleX, 60 * scaleY, 100 * scaleX, 30 * scaleY }, "Delete Process")) {}

        GuiLabel((Rectangle){ 20 * scaleX, 110 * scaleY, 100 * scaleX, 20 * scaleY }, "Arrival Time:");
        GuiLabel((Rectangle){ 20 * scaleX, 150 * scaleY, 100 * scaleX, 20 * scaleY }, "Burst Time:");

        // Toggle edit mode for arrivalTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            arrivalTimeEdit = true;
            burstTimeEdit = false;
        }

        // Toggle edit mode for burstTimeInput textbox
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleY, 30 * scaleY }) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            burstTimeEdit = true;
            arrivalTimeEdit = false;
        }
        
        // Input from User
        GuiTextBox((Rectangle){ 130 * scaleX, 110 * scaleY, 100 * scaleX, 30 * scaleY }, arrivalTimeInput, sizeof(arrivalTimeInput), arrivalTimeEdit);
        GuiTextBox((Rectangle){ 130 * scaleX, 150 * scaleY, 100 * scaleX, 30 * scaleY }, burstTimeInput, sizeof(burstTimeInput), burstTimeEdit);
        
        // Process Informations
        GuiTextBox((Rectangle){ 20 * scaleX, 190 * scaleY, 200 * scaleX, 60 * scaleY }, PInfo, sizeof(PInfo), false);

        GuiListView((Rectangle){ 20 * scaleX, 260 * scaleY, 200 * scaleX, 200 * scaleY }, 
                    "Process 1;Process 2;Process 3", &scrollIndex, &activeItem);
        
        // Display log
        GuiLabel((Rectangle){ 250 * scaleX, 40 * scaleY, 500 * scaleX, 20 * scaleY }, "Changes: ");
        GuiTextBox((Rectangle){ 250 * scaleX, 60 * scaleY, 500 * scaleX, 200 * scaleY }, logContent, sizeof(logContent), false);
        GuiCheckBox((Rectangle){ 250 * scaleX, 300 * scaleY, 20 * scaleX, 20 * scaleY }, "Enable Random Context Switching", &contextSwitchingEnabled);

        // Label for current process or CPU status
        GuiLabel((Rectangle){ 250 * scaleX, 330 * scaleY, 500 * scaleX, 20 * scaleY }, "Current Process: P1 (Running)");
        
        // Display Queue
        GuiTextBox((Rectangle){ 250 * scaleX, 360 * scaleY, 500 * scaleX, 100 * scaleY }, queueStatus, sizeof(queueStatus), false);
        
        // Start Scheduling
        if (GuiButton((Rectangle){ 250 * scaleX, 270 * scaleY, 200 * scaleX, 20 * scaleY }, "Start")){}
        
        if (GuiButton((Rectangle){ 650 * scaleX, 470 * scaleY, 100 * scaleX, 30 * scaleY }, "Export to .csv")) {}

        if (GuiButton((Rectangle){ 20 * scaleX, 530 * scaleY, 100 * scaleX, 30 * scaleY }, "Informations")) {
        showMessageBox = true;
        }
        if (showMessageBox)
            {
                int result = GuiMessageBox((Rectangle){ 85 * scaleX, 70 * scaleY, 400 * scaleX, 200 * scaleY },
                    "#196#CPU SCHEDULING SIMULATOR", "Devs: Pham Tuan Phong 20214039, Tran Quang Huy 20210428", "OK");
                if (result >= 0) showMessageBox = false;
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

