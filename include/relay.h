#pragma once

#include <vxWorks.h>
#include <subsys/gpio/vxbGpioLib.h>

constexpr UINT32 FAN = 16;
constexpr UINT32 AC = 20;
constexpr UINT32 HEAT = 21;

constexpr UINT32 RELAY_ON = GPIO_VALUE_LOW;
constexpr UINT32 RELAY_OFF = GPIO_VALUE_HIGH;

bool initRelay();
void freeRelay();

void fanOnly();
void acOn();
void heatOn();
void allOff();