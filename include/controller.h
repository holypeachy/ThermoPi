#pragma once

#include <vxWorks.h>

constexpr ULONG AC_LOCKOUT_SECONDS = 20; // test value, increase to 5 min for normal use

enum class Mode : UINT8{
    Off = 0,
    FanOnly,
    AC,
    Heat
};

enum class DisplayMode : UINT8{
    CurrentTemp = 0,
    SelectMode,
    SelectTemp
};

struct SystemState
{
    UINT16 currentTemp{0};

    Mode activeMode{Mode::Off};
    Mode setMode{Mode::Off};
    UINT16 setTemp{70};

    Mode pendingMode{Mode::Off};
    UINT16 pendingTemp{70};

    DisplayMode displayMode{DisplayMode::CurrentTemp};
};

bool initController();
void freeController();

void controllerTask();
