#include "dht22.h"

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/timer/vxbTimerLib.h>
#include <taskLibCommon.h>
#include <timerDev.h>

bool initDht()
{
    if (vxbGpioAlloc(DHT) != OK){
        printf("failed to init dht\n");
        return false;
    }
    if (sysTimestampEnable() != OK)
    {
        printf("failed to enable timestamp timer\n");
        return false;
    }

    taskDelay(sysClkRateGet() * 2); // coarse, scheduler level sleep

    return true;
}

void freeDht(){
    vxbGpioFree(DHT);
}

static bool measurePulse( UINT32 pin, UINT32 level, UINT32 timeoutUs, UINT32 &durationUs) {
    const UINT32 freq = sysTimestampFreq();
    const UINT32 start = sysTimestampLock();

    while (vxbGpioGetValue(pin) == level)
    {
        const UINT32 now = sysTimestampLock();
        const UINT32 ticks = now - start;

        const UINT32 elapsedUs = static_cast<UINT32>( (static_cast<UINT64>(ticks) * 1000000ULL) / freq);

        if (elapsedUs >= timeoutUs) return false;
    }

    const UINT32 end = sysTimestampLock();
    const UINT32 ticks = end - start;

    durationUs = static_cast<UINT32>( (static_cast<UINT64>(ticks) * 1000000ULL) / freq);

    return true;
}

static bool waitForLevel(UINT32 pin, UINT32 level, UINT32 timeoutUs)
{
    const UINT32 freq = sysTimestampFreq(); // very precise system timer
    const UINT32 start = sysTimestampLock();

    while (vxbGpioGetValue(pin) != level)
    {
        const UINT32 now = sysTimestampLock();
        const UINT32 ticks = now - start;

        // s = ticks / ticks per second
        // us = (ticks * 1,000,000) / freq
        const UINT32 elapsedUs = static_cast<UINT32>((static_cast<UINT64>(ticks) * 1000000ULL) / freq);

        if (elapsedUs >= timeoutUs) return false;
    }

    return true;
}

std::optional<std::array<UINT16, 2>> readSensor()
{
    if (vxbGpioSetDir(DHT, GPIO_DIR_OUTPUT) != OK){
        // printf("failed to set dir on gpio pin - out\n");
        return std::nullopt;
    }

    vxbGpioSetValue(DHT, GPIO_VALUE_LOW);
    vxbUsDelay(1000); // microsecond busy wait, ~1ms

    if (vxbGpioSetDir(DHT, GPIO_DIR_INPUT) != OK){
        // printf("failed to set dir on gpio pin - in\n");
        return std::nullopt;
    }

    UINT32 duration = 0;

    if (!waitForLevel(DHT, GPIO_VALUE_LOW, 100)){
        // printf("wait for level timed out\n");
        return std::nullopt;
    }

    if (!measurePulse(DHT, GPIO_VALUE_LOW, 150, duration)){
        // printf("pulse measure timed out\n");
        return std::nullopt;
    }

    if (!measurePulse(DHT, GPIO_VALUE_HIGH, 150, duration)){
        // printf("pulse measure timed out\n");
        return std::nullopt;
    }
 

    std::array<UINT8, 5> data = {};

    for (int i = 0; i < 40; ++i)
    {
        UINT32 lowTime = 0;
        UINT32 highTime = 0;

        if (!measurePulse(DHT, GPIO_VALUE_LOW, 100, lowTime)){
            // printf("pulse measure timed out\n");
            return std::nullopt;
        }

        if (!measurePulse(DHT, GPIO_VALUE_HIGH, 100, highTime)){
            // printf("pulse measure timed out\n");
            return std::nullopt;
        }

        UINT8 bit = highTime > 45 ? 1 : 0;

        data[i / 8] = (data[i / 8] << 1) | bit;
    }


    UINT8 checksum = static_cast<UINT8>( data[0] + data[1] + data[2] + data[3]);
    if (checksum != data[4]){
        // printf("checksum failed\n");
        return std::nullopt;
    }

    UINT16 humidityRaw = (static_cast<UINT16>(data[0]) << 8) | data[1];
    UINT16 tempRaw =  (static_cast<UINT16>(data[2]) << 8) | data[3];
    return std::array<UINT16, 2>{tempRaw, humidityRaw};
}

