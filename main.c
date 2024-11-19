#define RAYGUI_IMPLEMENTATION

#include "os_controller.h"

int main()
{
    os_controller_t controller = {
        .model = NULL,
        .view  = { .progress          = 0.0f,
             .currentTime             = 0,
             .pidCounter              = 1,
             .processCount            = 0,
             .baseWidth               = 800,
             .baseHeight              = 630,
             .showMessageBox          = false,
             .messageType             = 0,
             .selectedScheduler       = 0,
             .contextSwitchingEnabled = false,
             .activeItem              = -1,
             .scrollIndex             = 0,
             .currentSize             = 100,
             .logContent              = "",
             .queueStatus             = "",
             .Performance             = "",
             .cpuTimeInput            = 0,
             .ioTimeInput             = 0,
             .arrivalTimeInput        = 0,
             .cpuNumberInput          = 0,
             .PInfo                   = "",
             .listViewContent         = "",
             .cpuTimeEdit             = false,
             .ioTimeEdit              = false,
             .arrivalTimeEdit         = false,
             .cpuNumberEdit           = false,
             .baseTextSize            = 20,
             .schedulerStarted        = false,
             .runningProcessIndex     = -1,
             .processRunning          = false },
    };

    // Call the RunGUI function and pass the reference to the sim_view_t instance
    RunGUI(&controller.view);

    return 0;
}
