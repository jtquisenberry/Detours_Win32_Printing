#pragma comment(lib, "shell32.lib")
#include <windows.h>
#include <shellapi.h>

int main(void)
{
	ShellAboutW(NULL, L"Some text here", L"More text here", NULL);
	return 0;
}
