#pragma once

#include "controller.h"
#include <semLibCommon.h>
#include <taskLibCommon.h>

struct AppContext
{
    SystemState state;

    SEM_ID stateMutex{SEM_ID_NULL};
    SEM_ID buttonEvent{SEM_ID_NULL};

    TASK_ID controllerTaskId{TASK_ID_ERROR};
    TASK_ID buttonTaskId{TASK_ID_ERROR};
    TASK_ID displayTaskId{TASK_ID_ERROR};
};

extern AppContext app;
