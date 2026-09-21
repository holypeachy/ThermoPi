#include "buttons.h"
#include <subsys/gpio/vxbGpioLib.h>
#include <semLibCommon.h>
#include "app.h"
#include <cstdio>
#include <timerDev.h>

static void buttonIsr(void *)
{
    semGive(app.buttonEvent);
}

bool initButtons(){
    vxbGpioAlloc(MODE_BTN);
    if (vxbGpioSetDir(MODE_BTN, GPIO_DIR_INPUT) != OK)
        return false;

    if (vxbGpioIntConnect(MODE_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio connect failed\n");
            return false;
        }
    if (vxbGpioIntConfig(MODE_BTN,
        INTR_TRIGGER_EDGE,
        INTR_POLARITY_HIGH) != OK){
            printf("gpio config failed\n");
            return false;
        }
    if (vxbGpioIntEnable(MODE_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio enable failed\n");
            return false;
        }

    vxbGpioAlloc(INC_BTN);
    if (vxbGpioSetDir(INC_BTN, GPIO_DIR_INPUT) != OK)
        return false;
    if (vxbGpioIntConnect(INC_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio connect failed\n");
            return false;
        }
    if (vxbGpioIntConfig(INC_BTN,
        INTR_TRIGGER_EDGE,
        INTR_POLARITY_HIGH) != OK){
            printf("gpio config failed\n");
            return false;
        }
    if (vxbGpioIntEnable(INC_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio enable failed\n");
            return false;
        }

    vxbGpioAlloc(DEC_BTN);
    if (vxbGpioSetDir(DEC_BTN, GPIO_DIR_INPUT) != OK)
        return false;
    if (vxbGpioIntConnect(DEC_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio connect failed\n");
            return false;
        }
    if (vxbGpioIntConfig(DEC_BTN,
        INTR_TRIGGER_EDGE,
        INTR_POLARITY_HIGH) != OK){
            printf("gpio config failed\n");
            return false;
        }
    if (vxbGpioIntEnable(DEC_BTN,
        reinterpret_cast<VOIDFUNCPTR>(buttonIsr),
        nullptr) != OK){
            printf("gpio enable failed\n");
            return false;
        }

    return true;
}

void freeButtons(){
    vxbGpioIntDisable(MODE_BTN,reinterpret_cast<VOIDFUNCPTR>(buttonIsr),nullptr);
    vxbGpioIntDisconnect(MODE_BTN, reinterpret_cast<VOIDFUNCPTR>(buttonIsr), nullptr);
    vxbGpioFree(MODE_BTN);
    vxbGpioIntDisable(INC_BTN,reinterpret_cast<VOIDFUNCPTR>(buttonIsr),nullptr);
    vxbGpioIntDisconnect(INC_BTN, reinterpret_cast<VOIDFUNCPTR>(buttonIsr), nullptr);
    vxbGpioFree(INC_BTN);
    vxbGpioIntDisable(DEC_BTN,reinterpret_cast<VOIDFUNCPTR>(buttonIsr),nullptr);
    vxbGpioIntDisconnect(DEC_BTN, reinterpret_cast<VOIDFUNCPTR>(buttonIsr), nullptr);
    vxbGpioFree(DEC_BTN);
}

static Mode nextMode(Mode mode)
{
    switch (mode)
    {
    case Mode::Off:
        return Mode::FanOnly;
    case Mode::FanOnly:
        return Mode::AC;
    case Mode::AC:
        return Mode::Heat;
    case Mode::Heat:
        return Mode::Off;
    }

    return Mode::Off;
}

void buttonTask(){
    while (true)
    {
        semTake(app.buttonEvent, WAIT_FOREVER);

        semTake(app.stateMutex, WAIT_FOREVER);
        app.state.pendingMode = app.state.setMode;
        app.state.pendingTemp = app.state.setTemp;
        semGive(app.stateMutex);

        while (true)
        {
            // debounce, for button contacts
            taskDelay(1);

            semTake(app.stateMutex, WAIT_FOREVER);

            if (vxbGpioGetValue(MODE_BTN) == GPIO_VALUE_HIGH)
            {
                app.state.pendingMode = nextMode(app.state.pendingMode);
                app.state.displayMode = DisplayMode::SelectMode;
            } else if (vxbGpioGetValue(INC_BTN) == GPIO_VALUE_HIGH)
            {
                if (app.state.pendingTemp < MAX_TEMP){
                    app.state.pendingTemp++;
                }
                app.state.displayMode = DisplayMode::SelectTemp;
            } else if (vxbGpioGetValue(DEC_BTN) == GPIO_VALUE_HIGH)
            {
                if (app.state.pendingTemp > MIN_TEMP) {
                    app.state.pendingTemp--;
                }
                app.state.displayMode = DisplayMode::SelectTemp;
            }

            semGive(app.stateMutex);

            if (semTake(app.buttonEvent, sysClkRateGet() * INPUT_TIMEOUT) == ERROR)
            {
                semTake(app.stateMutex, WAIT_FOREVER);
                app.state.setMode = app.state.pendingMode;
                app.state.setTemp = app.state.pendingTemp;
                app.state.displayMode = DisplayMode::CurrentTemp;
                semGive(app.stateMutex);

                break;
            }
        }
    }
}