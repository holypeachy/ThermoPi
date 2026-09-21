#include "controller.h"
#include "dht22.h"
#include "relay.h"
#include <timerDev.h>
#include <taskLibCommon.h>
#include "app.h"
#include <tickLib.h>

static bool isAcLocked{false};
static ULONG lastAcOffTick{0};

bool initController(){
    bool ok{};
    ok = initDht();
    if (!ok){
        freeController();
        return false;
    }

    ok = initRelay();
    if (!ok){
        freeController();
        return false;
    }

    return true;
}

void freeController(){
    freeDht();
    freeRelay();
}

void changeActiveMode(Mode newMode){
    semTake(app.stateMutex, WAIT_FOREVER);

    if (app.state.activeMode == Mode::AC && newMode != Mode::AC)
    {
        lastAcOffTick = tickGet();
        isAcLocked = true;
    }

    app.state.activeMode = newMode;

    semGive(app.stateMutex);
}

static bool canStartAc()
{
    if (!isAcLocked) return true;

    const ULONG elapsed = tickGet() - lastAcOffTick;
    const ULONG timeout = AC_LOCKOUT_SECONDS * sysClkRateGet();

    if (elapsed >= timeout)
    {
        isAcLocked = false;
        return true;
    }

    return false;
}

static UINT16 tempCToF(UINT16 tempC10)
{
    return static_cast<UINT16>(
        ((static_cast<UINT32>(tempC10) * 9 + 25) / 50) + 32);
}


void controllerTask(){
    std::array<UINT16, 2> data;
    Mode setMode{};
    Mode activeMode{};
    UINT16 temp{};
    UINT16 setTemp{};

 
    while (true)
    {
        taskDelay(sysClkRateGet() * 4);

        auto res = readSensor();
        if (res){
            data = *res;
        } else{
            printf("sensor read failed\n");
            continue;
        }

        semTake(app.stateMutex, WAIT_FOREVER);
        app.state.currentTemp = tempCToF(data[0]);
        setMode = app.state.setMode;
        activeMode = app.state.activeMode;
        temp = app.state.currentTemp;
        setTemp = app.state.setTemp;
        semGive(app.stateMutex);

        printf(
            "temp=%u set=%u setMode=%u active=%u\n",
            temp,
            setTemp,
            static_cast<unsigned>(setMode),
            static_cast<unsigned>(activeMode));
            
        if (setMode == Mode::AC)
        {
            if (temp > setTemp && activeMode != Mode::AC && canStartAc()){
                changeActiveMode(Mode::AC);
                acOn();
            }
            else if (temp <= setTemp && activeMode != Mode::Off){
                changeActiveMode(Mode::Off);
                allOff();
            }
        } else if (setMode == Mode::Heat){
            if (temp < setTemp && activeMode != Mode::Heat){
                changeActiveMode(Mode::Heat);
                heatOn();
            }
            else if (temp >= setTemp && activeMode != Mode::Off){
                changeActiveMode(Mode::Off);
                allOff();
            }
        } else if (setMode == Mode::FanOnly){
            if (activeMode != Mode::FanOnly){
                changeActiveMode(Mode::FanOnly);
                fanOnly();
            }
        } else if (setMode == Mode::Off){
            if (activeMode != Mode::Off){
                changeActiveMode(Mode::Off);
                allOff();
            }
        }
    }
}
