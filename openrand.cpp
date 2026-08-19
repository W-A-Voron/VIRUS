#include <windows.h>
#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <shellapi.h>

std::mt19937 openRng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

void OpenRandomAppsAndSites() {
    while (true) {
        try {
            int action = std::uniform_int_distribution<int>(0, 5)(openRng);

            switch (action) {
                case 0: {
                    std::vector<std::string> apps = {"calc", "notepad", "mspaint", "write", "wordpad", "explorer", "taskmgr"};
                    std::string app = apps[std::uniform_int_distribution<int>(0, (int)apps.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", app.c_str(), NULL, NULL, SW_SHOW);
                    break;
                }
                case 1: {
                    std::vector<std::string> sites = {
                        "https://youtube.com", "https://google.com", "https://reddit.com",
                        "https://ebay.com", "https://amazon.com", "https://wikipedia.org",
                        "https://twitch.tv", "https://github.com", "https://stackoverflow.com"
                    };
                    std::string site = sites[std::uniform_int_distribution<int>(0, (int)sites.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", site.c_str(), NULL, NULL, SW_SHOW);
                    break;
                }
                case 2: {
                    ShellExecuteA(NULL, "open", "mspaint.exe", NULL, NULL, SW_SHOW);
                    break;
                }
                case 3: {
                    system("start cmd /k echo YOUR PC IS COMPROMISED");
                    break;
                }
                case 4: {
                    std::vector<std::string> folders = {"C:\\Windows", "C:\\Windows\\System32", "C:\\", "C:\\Program Files"};
                    std::string folder = folders[std::uniform_int_distribution<int>(0, (int)folders.size() - 1)(openRng)];
                    ShellExecuteA(NULL, "open", "explorer.exe", folder.c_str(), NULL, SW_SHOW);
                    break;
                }
                case 5: {
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
