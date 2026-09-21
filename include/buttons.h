#pragma once

#include <vxWorks.h>

constexpr UINT32 MODE_BTN = 8;
constexpr UINT32 INC_BTN = 7;
constexpr UINT32 DEC_BTN = 12;

constexpr int INPUT_TIMEOUT = 3;
constexpr UINT16 MAX_TEMP = 90; // F
constexpr UINT16 MIN_TEMP = 50; // F

bool initButtons();
void freeButtons();
void buttonTask();