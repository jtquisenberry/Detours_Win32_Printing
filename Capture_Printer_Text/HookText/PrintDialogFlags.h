#pragma once


#include "StringsAndDirectories.h"
#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>
//#include <unordered_map> 
#include <map>


class PrintDialogFlags
{
private:
	int xxxxxxx = 0;

public:

	std::map<unsigned long, std::wstring> mapPrintDialogFlags;

	PrintDialogFlags()
	{
		mapPrintDialogFlags[0x00000000] = L"PD_ALLPAGES";
		mapPrintDialogFlags[0x00000001] = L"PD_SELECTION";
		mapPrintDialogFlags[0x00000002] = L"PD_PAGENUMS";
		mapPrintDialogFlags[0x00000004] = L"PD_NOSELECTION";
		mapPrintDialogFlags[0x00000008] = L"PD_NOPAGENUMS";
		mapPrintDialogFlags[0x00000010] = L"PD_COLLATE";
		mapPrintDialogFlags[0x00000020] = L"PD_PRINTTOFILE";
		mapPrintDialogFlags[0x00000080] = L"PD_NOWARNING";
		mapPrintDialogFlags[0x00000100] = L"PD_RETURNDC";
		mapPrintDialogFlags[0x00000200] = L"PD_RETURNIC";
		mapPrintDialogFlags[0x00000400] = L"PD_RETURNDEFAULT";
		mapPrintDialogFlags[0x00004000] = L"PD_ENABLEPRINTTEMPLATE";
		mapPrintDialogFlags[0x00010000] = L"PD_ENABLEPRINTTEMPLATEHANDLE";
		mapPrintDialogFlags[0x00040000] = L"PD_USEDEVMODECOPIES";
		mapPrintDialogFlags[0x00040000] = L"PD_USEDEVMODECOPIESANDCOLLATE";
		mapPrintDialogFlags[0x00080000] = L"PD_DISABLEPRINTTOFILE";
		mapPrintDialogFlags[0x00100000] = L"PD_HIDEPRINTTOFILE";
		mapPrintDialogFlags[0x00400000] = L"PD_CURRENTPAGE";
		mapPrintDialogFlags[0x00800000] = L"PD_NOCURRENTPAGE";
		mapPrintDialogFlags[0x01000000] = L"PD_EXCLUSIONFLAGS";
		mapPrintDialogFlags[0x10000000] = L"PD_USELARGETEMPLATE";
	}

	std::wstring GetNames(unsigned long number)
	{
		std::wstring names = L"";
		
		// Iterating the map and 
		// printing ordered values
		std::map<unsigned long, std::wstring>::iterator it;
		for (it = mapPrintDialogFlags.begin(); it != mapPrintDialogFlags.end(); it++) {
			unsigned long key = it->first;
			std::wstring value = it->second;

			if ((key & number) == key)
			{
				if (names == L"")
				{
					names += value;
				}
				else
				{
					names += L", " + value;
				}
			}
		}

		return names;
	}

};