#include <windows.h>

HHOOK keyboardHook = NULL;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;

        // Блокируем важные клавиши
        if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN ||
            p->vkCode == VK_MENU ||  // Alt
            p->vkCode == VK_DELETE ||
            p->vkCode == VK_CONTROL ||
            p->vkCode == VK_RETURN) {
            return 1;  // Блокируем
        }

        // Блокируем комбинации Ctrl+Alt+Del, Ctrl+Shift+Esc и т.д.
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000) &&
            p->vkCode == VK_DELETE) {
            return 1;
        }

        // Блокируем Alt+F4
        if ((GetAsyncKeyState(VK_MENU) & 0x8000) && p->vkCode == VK_F4) {
            return 1;
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

void InitializeKeyboardBlockModule() {
    keyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (keyboardHook) {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
