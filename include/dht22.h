#pragma once

#include <vxWorks.h>
#include <array>
#include <optional>

constexpr UINT32 DHT = 4;
constexpr UINT32 STATUS_LED = 12;

bool initDht();
void freeDht();
std::optional<std::array<UINT16, 2>> readSensor();
void getTemp();
