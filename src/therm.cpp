#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>
#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/timer/vxbTimerLib.h>

#include "display.h"
#include "buttons.h"
#include "controller.h"
#include "app.h"

extern "C" void freeAll();

bool initAll(){
    bool ok{};
    ok = initController();
    if (!ok){
        freeAll();
        return false;
    }
    ok = initDisplay();
    if (!ok){
        freeAll();
        return false;
    }
    ok = initButtons();
    if (!ok){
        freeAll();
        return false;

    }
    printf("good init\n");
    return true;
}

static void deleteTaskIfRunning(TASK_ID &id)
{
    if (id != TASK_ID_ERROR)
    {
        if (taskDelete(id) == OK)
            id = TASK_ID_ERROR;
    }
}

extern "C" void freeAll()
{
    freeButtons();

    deleteTaskIfRunning(app.buttonTaskId);
    deleteTaskIfRunning(app.displayTaskId);
    deleteTaskIfRunning(app.controllerTaskId);

    freeController();
    freeDisplay();

    if (app.buttonEvent != SEM_ID_NULL)
    {
        semDelete(app.buttonEvent);
        app.buttonEvent = SEM_ID_NULL;
    }

    if (app.stateMutex != SEM_ID_NULL)
    {
        semDelete(app.stateMutex);
        app.stateMutex = SEM_ID_NULL;
    }
}

extern "C" int startTherm()
{
    SEM_ID mu = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
    if (mu == SEM_ID_NULL)
    {
        printf("failed to create mutex\n");
        freeAll();
        return ERROR;
    }
    app.stateMutex = mu;

    SEM_ID sem = semBCreate( SEM_Q_PRIORITY, SEM_EMPTY);
    if (sem == SEM_ID_NULL)
    {
        printf("failed to button bin sempahore\n");
        freeAll();
        return ERROR;
    }
    app.buttonEvent = sem;

    bool ok = initAll();
    if (!ok){
        printf("init failed\n");
        return ERROR;
    }
 

    char controllerTaskName[] = "tController";
    app.controllerTaskId = taskSpawn(
        controllerTaskName,
        90,
        0,
        8192,
        reinterpret_cast<FUNCPTR>(controllerTask),
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0);
    if (app.controllerTaskId == TASK_ID_ERROR){
        freeAll();
        return ERROR;
    }

    char dispTaskName[] = "tDisplay";
    app.displayTaskId = taskSpawn(
        dispTaskName,
        150,
        0,
        8192,
        reinterpret_cast<FUNCPTR>(displayTask),
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0);
    if (app.displayTaskId == TASK_ID_ERROR){
        freeAll();
        return ERROR;
    }

    char btnTaskName[] = "tButtons";
    app.buttonTaskId = taskSpawn(
        btnTaskName,
        100,
        0,
        8192,
        reinterpret_cast<FUNCPTR>(buttonTask),
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0);
    if (app.buttonTaskId == TASK_ID_ERROR){
        freeAll();
        return ERROR;
    }
 

    return OK;
}
