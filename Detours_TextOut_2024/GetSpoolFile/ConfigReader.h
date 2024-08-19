#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <iomanip>

class ConfigReader
{
public: 
	std::wstring m_strFilename;
    // Declare a map to store configuration data.
    std::unordered_map<std::wstring, std::wstring> m_configData;


	ConfigReader(std::wstring strFilename=L"")
	{
        if (strFilename != L"")
        {
            m_strFilename = strFilename;
        }
        else
        {
            m_strFilename = L"Detours_Printing_config.txt";
        }
	}

    std::wstring trim(std::wstring& orig_string)
    {
        orig_string.erase(orig_string.find_last_not_of(L" \t") + 1);         //suffixing spaces
        orig_string.erase(0, orig_string.find_first_not_of(L" \t"));       //prefixing spaces
        return orig_string;
    }



	int Read()
	{
        // Open the configuration, but first create it if it does not exist.
        std::wifstream configFileStream(m_strFilename) ;
        //configFileStream.open(m_strFilename);
        if (!configFileStream.is_open())
        {
            return 1;
        }
            
        std::wstring line;
        // Read all lines of the source file
        while (std::getline(configFileStream, line))
        {
            if (line.find(L'=') != std::string::npos)
            {
                std::wistringstream iss(line);
                int a = 1;

                std::wstring value;
                std::wstring key;
                std::getline(iss, key, L'=');
                std::getline(std::getline(iss, value, L'=') >> std::ws, value);
                value = trim(value);
                key = trim(key);
                m_configData[key] = value;
            }
        }
	}

    std::unordered_map<std::wstring, std::wstring> GetConfig()
    {
        return m_configData;
    }

};