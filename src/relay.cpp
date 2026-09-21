#include "relay.h"

bool initRelay(){
    vxbGpioAlloc(FAN);
    if (vxbGpioSetDir(FAN, GPIO_DIR_OUTPUT) != OK)
        return false;
    vxbGpioSetValue(FAN, RELAY_OFF);

    vxbGpioAlloc(AC);
    if (vxbGpioSetDir(AC, GPIO_DIR_OUTPUT) != OK)
        return false;
    vxbGpioSetValue(AC, RELAY_OFF);

    vxbGpioAlloc(HEAT);
    if (vxbGpioSetDir(HEAT, GPIO_DIR_OUTPUT) != OK)
        return false;
    vxbGpioSetValue(HEAT, RELAY_OFF);

    return true;
}

void freeRelay(){
    vxbGpioFree(FAN);
    vxbGpioFree(AC);
    vxbGpioFree(HEAT);
}

void fanOnly(){
    vxbGpioSetValue(AC, RELAY_OFF);
    vxbGpioSetValue(HEAT, RELAY_OFF);

    vxbGpioSetValue(FAN, RELAY_ON);
}

void acOn(){
    vxbGpioSetValue(HEAT, RELAY_OFF);

    vxbGpioSetValue(AC, RELAY_ON);
    vxbGpioSetValue(FAN, RELAY_ON);
}

void heatOn(){
    vxbGpioSetValue(AC, RELAY_OFF);

    vxbGpioSetValue(HEAT, RELAY_ON);
    vxbGpioSetValue(FAN, RELAY_ON);
}

void allOff(){
    vxbGpioSetValue(FAN, RELAY_OFF);
    vxbGpioSetValue(AC, RELAY_OFF);
    vxbGpioSetValue(HEAT, RELAY_OFF);
}