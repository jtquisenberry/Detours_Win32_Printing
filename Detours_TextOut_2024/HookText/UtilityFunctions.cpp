#pragma once
#include "HookText.h"
#include "pch.h"
#include <stdio.h>
#include <string>


void ConvertDecToBin(int IntegerToConvert, char char_Output[])
{
	char char_Temp[90] = "";

	for (int i = 31; i >= 0; i--)
	{
		int bit = ((IntegerToConvert >> i) & 1);
		sprintf(char_Temp, "%s%d", char_Temp, bit);

		if ((i % 4 == 0) & (i != 0))
		{
			sprintf(char_Temp, "%s,", char_Temp);
		}
	}

	sprintf(char_Output, "%s", char_Temp);

	return;
}

std::wstring IntToBinaryStringW(int nFlags)
{
	char strFlagsA[100] = { 0 };
	wchar_t strFlagW[100] = { 0 };
	ConvertDecToBin(nFlags, strFlagsA);
	MultiByteToWideChar(CP_ACP, 0, strFlagsA, strlen(strFlagsA) + 1, strFlagW, sizeof(strFlagW) / sizeof(strFlagW[0]));
	std::wstring strFlag(strFlagW);
	return strFlag;
}
