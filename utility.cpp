/*******************************************
A Docile Sloth 2016 (adocilesloth@gmail.com)
*******************************************/

#include "utility.hpp"
#if _WIN32
	#include <Windows.h>
#else
#endif

//Returns count of non-overlapping occurrences of 'sub' in 'str'
int countSubstring(const std::string& str, const std::string& sub)
{
	if (sub.length() == 0 || str.length() < sub.length())
	{
		return 0;
	}

	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset + sub.length()))
	{
		count++;
	}
	return count;
}
int wcountSubstring(const std::wstring& str, const std::wstring& sub)
{
	if (sub.length() == 0 || str.length() < sub.length())
	{
		return 0;
	}

	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset + sub.length()))
	{
		count++;
	}
	return count;
}

//Replace instances of 'from' in 'str' to 'to' 
void ReplaceAll(std::string &str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return;
}
void wReplaceAll(std::wstring &str, const std::wstring& from, const std::wstring& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return;
}

//Convert string to wstring and vice versa
#if _WIN32
	std::wstring s2ws(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo( size_needed, 0 );
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::string ws2s(const std::wstring& wstr)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo( size_needed, 0 );
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
#else
	#include <cstring>

	std::wstring s2ws(const std::string& str)
	{
		std::string curLocale = setlocale(LC_ALL, ""); 
		const char* _Source = str.c_str();
		size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
		wchar_t *_Dest = new wchar_t[_Dsize];
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		std::wstring strTo = _Dest;
		delete []_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return strTo;
	}

	std::string ws2s(const std::wstring& wstr)
	{
		std::string curLocale = setlocale(LC_ALL, "");
		const wchar_t* _Source = wstr.c_str();
		size_t _Dsize = wcstombs(NULL, _Source, 0) + 1;
		char *_Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest,_Source,_Dsize);
		std::string strTo = _Dest;
		delete []_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return strTo;
	}
#endif

//Sleep function
#ifdef _WIN32
    void psleep(unsigned milliseconds)
    {
        Sleep(milliseconds);
    }
#else
    #include <unistd.h>

    void psleep(unsigned milliseconds)
    {
        usleep(milliseconds * 1000); // takes microseconds
    }
#endif
