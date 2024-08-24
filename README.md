# Windows Printing API Hooking and DLL Injection with Microsoft Detours

This repository demonstrates Windows API hooking and DLL injection using the Microsoft Detours injection framework. Projects focus on hooking the APIs of the Windows printing subsystem. A supporting project demonstrates interaction with the Windows print spooler.

The motivation for this project was a need to detect when there is an attempt to print outside page boundaries, resulting in content missing from the printout. 

## Components

The `Capture_Printer_Text` solution hooks printing APIs used for GDI printing, intercepts their arguments and return values, and writes them to a log file. The `Capture_Printer_Text` solution contains these projects:

* WithDLLEXE
* HookText
* GetSpoolFile

## Why Detours

Detours is a more complex framework than MinHook and NtHookEngine, but the complexity confers advantages. Detours achieves a higher rate of success injecting DLLs and instrumenting APIs. Detours supports 64-bit and 32-bit unmanaged DLLs, as well as .NET DLLs. It has been open-source under the MIT license since version 4.0.1, released on November 7, 2016. These considerations make Detours a good choice for a commercial, production environment.


# Usage

``` shell
> WithDllEXE.exe -d:<dll file> <exe file>
```

* `-d:`: Indicates that the text that follows specifies a DLL file.
* `<dll file>` The fully-qualified filename of a DLL to inject into a target process.
* `<exe file>` The fully-qualified filename of an executable file that will be started and into which a DLL will be injected.

## Examples

__WithDllEXE.exe (the injector)__

``` shell
WithDllEXE.exe -d:D:\Development\git\Detours_Win32_Printing\Capture_Printer_Text\x64\Debug\HookText.dll D:\Development\private\Wordpad\wordpad.exe
```

Sysinternals Process Explorer shows `HookText.dll` has been injected into `Wordpad.exe`.


![Payload DLL in Target](images/hooktext_in_wordpad.png?raw=true "Payload DLL in Target")


# WithDllEXE

The `WithDllEXE` executable injects a 64-bit DLL into a 64-bit target application. It starts an executable file as a new process in suspended mode and injects the DLL.

This executable is a slightly modified version of the `withdll` sample in the Detours source code. https://github.com/microsoft/Detours/tree/main/samples/withdll. The original version compiles to  a DLL. The version in this repository compiles to a command-line executable.

`WithDllEXE` requires that the payload DLL exports ordinal #1. `WithDllEXE` continues running until the target processes terminates.


# HookText

The `HookText` DLL is meant to be injected into a target executable, such as the 64-bit `Wordpad.exe`. It hooks the following functions used to manage print jobs or draw text:

* `PrintDlgW`
* `PrintDlgExW`
* `AddJobA`
* `AddJobW`
* `StartDocA`
* `StartDocW`
* `StartPage`
* `EndPage`
* `EndDoc`
* `ExtTextOutA`
* `ExtTextOutW`
* `TextOutA`
* `TextOutW`
* `DrawTextA`
* `DrawTextW`
* `DrawTextExA`
* `DrawTextExW`
* `PolyTextOutA`
* `PolyTextOutW`

The proxy / detour functions write information about each API call to a log file. The log file is stored in `%USERPROFILE%\TextOutFile.txt`

__Example__

In this example, the log file reports the text that was printed and its coordinates.

```
...
-----------------------------------------------------------
StartDocW
-----------------------------------------------------------
hdc =                  FFFFFFFFE0211DC1
cbSize =               40
fwType =               0
lpszDatatype =         (null)
lpszDocName =          Document
lpszOutput =           (null)
...
-----------------------------------------------------------
ExtTextOutW
-----------------------------------------------------------
lpString:              ZZZZZZZZZZ
x =                    375
y =                    300
lprect =               0000002856BFC0B0
lprect->left =         375
lprect->right =        2175
lprect->top =          300
lprect->bottom =       406
c =                    10
lpDx =                 000001B3E62E2B60
...
-----------------------------------------------------------
TextOutW
-----------------------------------------------------------
lpString =             1
...
```

Here is the corresponding printout, printed using PDF printer "Bullzip PDF Printer".

![Printout from Wordpad](images/printout_as_pdf.png?raw=true "Printout from Wordpad")


# Supporting Projects

# GetSpoolFile.exe

This executable makes two copies of the spool file associated with a given print queue and job ID. The copies are acquired in different ways:

1. Copies the .SPL file from the spool file directory (typically `C:\Windows\System32\spool\PRINTERS`).
2. Reads data from the printer using `ReadPrinter`.

Both copies are written to a specified location in the filesystem for comparison. Using printer "Bullzip PDF Printer", the two versions are identical.

## Configuration

The program reads configuration from a file named `GetSpoolFile.cfg`, which must be in the same directory as the executable. If the configuration file is missing, it will be created with these values.

```
Printer = Bullzip PDF Printer
Spool File Source Path = C:\Windows\System32\spool\PRINTERS
Spool File Destination Path = D:\Projects\spool
Log File Location = D:\Projects\GetSpoolerFiles.txt
Job ID = 3
```

## Usage

``` shell
> GetSpoolFile.exe

nFileSizeLow: 275482, nFileSizeHigh: 0
Filename: \Windows\System32\spool\PRINTERS\00003.SPL

Error code:        0
Number of bytes:   275482

Job ID: 2, Driver: Bullzip PDF Printer
Job ID: 3, Driver: Bullzip PDF Printer

Reading Bullzip PDF Printer, Job 00003%!PS-Adobe-3.0
```

__Filesystem__

This is an example of the destination directory after copying the spool files for four jobs.

```
08/19/2024  05:09 PM         3,081,304 00002.bin
08/19/2024  05:09 PM         3,081,304 00002.spl
08/24/2024  01:22 PM           275,482 00003.bin
08/24/2024  01:22 PM           275,482 00003.spl
08/19/2024  08:59 PM           102,867 00004.bin
08/19/2024  08:59 PM           102,867 00004.spl
```

The .spl file is the version copied from the spool directory, and the .bin file is the version read from the printer.

# Credits

## Detours
* Brubacher, Doug. "Detours: Binary interception of Win32 functions." Windows NT 3rd symposium (windows NT 3rd symposium). 1999.
* https://github.com/microsoft/Detours/
* https://www.microsoft.com/en-us/research/project/detours/

