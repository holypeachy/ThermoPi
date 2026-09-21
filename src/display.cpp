#include "display.h"
#include "app.h"
#include <subsys/timer/vxbTimerLib.h>

bool initDisplay(){
    vxbGpioAlloc(DATA);
    if (vxbGpioSetDir(DATA, GPIO_DIR_OUTPUT) != OK)
        return false;
    vxbGpioAlloc(CLOCK);
    if (vxbGpioSetDir(CLOCK, GPIO_DIR_OUTPUT) != OK)
        return false;
    vxbGpioAlloc(LATCH);
    if (vxbGpioSetDir(LATCH, GPIO_DIR_OUTPUT) != OK)
        return false;
    for (auto pin : DIGITS) {
        vxbGpioAlloc(pin);
        if (vxbGpioSetDir(pin, GPIO_DIR_OUTPUT) != OK)
            return false;
    }

        return true;
}

void freeDisplay(){
    vxbGpioFree(DATA);
    vxbGpioFree(CLOCK);
    vxbGpioFree(LATCH);
    for (auto pin : DIGITS) vxbGpioFree(pin);
}

void allDigitsOff(){
    for (auto pin : DIGITS) vxbGpioSetValue(pin, GPIO_VALUE_LOW);
}

void sendByte(UINT8 value){
    vxbGpioSetValue(LATCH, GPIO_VALUE_LOW);

    for (int i = 7; i >= 0; --i)
    {
        vxbGpioSetValue(CLOCK, GPIO_VALUE_LOW);
        vxbGpioSetValue(DATA, (value & (1 << i)) ? GPIO_VALUE_HIGH : GPIO_VALUE_LOW);
        vxbGpioSetValue(CLOCK, GPIO_VALUE_HIGH);
    }

    vxbGpioSetValue(LATCH, GPIO_VALUE_HIGH);
 
}

static std::array<UINT8, 2> tempToDisp(UINT16 temp){
    UINT16 tens = temp / 10;
    UINT16 ones = temp % 10;

    std::array<UINT8, 2> result{tens > 0 ? NUMBERS[tens] : BLANK, static_cast<UINT8>(NUMBERS[ones])}; // decimal & 0x7F
    return result;
}

static std::array<UINT8, 2> modeToShortDisp(Mode mode){
    auto index = static_cast<std::size_t>(mode);
    return SHORT_MODES[index];
}

static std::array<UINT8, 4> modeToLongDisp(Mode mode){
    auto index = static_cast<std::size_t>(mode);
    return LONG_MODES[index];
}


void displayTask(){
    UINT16 currentTemp{};
    Mode setMode{};
    UINT16 pendingTemp{};
    Mode pendingMode{};
    DisplayMode dispMode{};

    while (true)
    {
        semTake(app.stateMutex, WAIT_FOREVER);
        currentTemp = app.state.currentTemp;
        setMode = app.state.setMode;
        pendingTemp = app.state.pendingTemp;
        pendingMode = app.state.pendingMode;
        dispMode = app.state.displayMode;
        semGive(app.stateMutex);

        std::array<UINT8, 4> dispData{};

        if (dispMode == DisplayMode::CurrentTemp)
        {
            auto t = tempToDisp(currentTemp);
            auto m = modeToShortDisp(setMode);
            dispData[0] = t[0];
            dispData[1] = t[1];
            dispData[2] = m[0];
            dispData[3] = m[1];
        }
        else if (dispMode == DisplayMode::SelectTemp)
        {
            auto t = tempToDisp(pendingTemp);
            dispData[0] = BLANK;
            dispData[1] = t[0];
            dispData[2] = t[1];
            dispData[3] = BLANK;
        }
        else if (dispMode == DisplayMode::SelectMode)
        {
            dispData = modeToLongDisp(pendingMode);
        }

        for (int i = 0; i < 4; ++i)
        {
            allDigitsOff();
            sendByte(dispData[i]);
            vxbGpioSetValue(DIGITS[i], GPIO_VALUE_HIGH);

            vxbUsDelay(2000);
        }
    }
}
