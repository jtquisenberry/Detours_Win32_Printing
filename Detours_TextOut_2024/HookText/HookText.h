#pragma once

#include <stdio.h>
#include <Windows.h>
#include <string>
#include "detours.h"


#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

void ConvertDecToBin(int IntegerToConvert, char char_Output[]);  // Function prototype, its declaration
std::wstring IntToBinaryStringW(int nFlags);

#endif


