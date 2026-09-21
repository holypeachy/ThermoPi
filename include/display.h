#pragma once

#include <vxWorks.h>
#include <array>
#include <subsys/gpio/vxbGpioLib.h>

// 74HC595
constexpr UINT32 DATA = 17;
constexpr UINT32 CLOCK = 27;
constexpr UINT32 LATCH = 22;

constexpr std::array<unsigned, 4> DIGITS = {
    5,  // D1
    6,  // D2
    13, // D3
    19  // D4
};
constexpr UINT8 BLANK = 0xFF;
constexpr std::array<UINT8, 10> NUMBERS = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};

constexpr UINT8 LETTER_O = 0xA3;
constexpr UINT8 LETTER_F = 0x8E;
constexpr UINT8 LETTER_A = 0x88;
constexpr UINT8 LETTER_C = 0xC6;
constexpr UINT8 LETTER_H = 0x89;
constexpr UINT8 LETTER_E = 0x86;
constexpr UINT8 LETTER_N = 0xAB;
constexpr UINT8 LETTER_T = 0x87;

constexpr std::array<std::array<UINT8, 2>, 4> SHORT_MODES = {{
    {LETTER_O, LETTER_F},
    {LETTER_F, LETTER_A},
    {LETTER_A, LETTER_C},
    {LETTER_H, LETTER_E}
}};

constexpr std::array<std::array<UINT8, 4>, 4> LONG_MODES= {{
    {LETTER_O, LETTER_F, LETTER_F, BLANK},
    {LETTER_F, LETTER_A, LETTER_N, BLANK},
    {LETTER_A, LETTER_C, BLANK, BLANK},
    {LETTER_H, LETTER_E, LETTER_A, LETTER_T}
}};


bool initDisplay();
void freeDisplay();
void allDigitsOff();
void sendByte(UINT8 value);

void displayTask();