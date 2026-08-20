#include <windows.h>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <thread>

std::mt19937 rng((unsigned)std::chrono::steady_clock::now().time_since_epoch().count());

void FadeScreenEffect() {
    HDC hdc = GetDC(NULL);
    if (!hdc) return;
    
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
    
    // Плавное затухание к красному
    for (int i = 0; i < 255; i += 5) {
        HBRUSH brush = CreateSolidBrush(RGB(i, 0, 0));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        Sleep(10);
    }
    
    // Резкая вспышка
    for (int flash = 0; flash < 10; flash++) {
        HBRUSH white = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &rect, white);
        DeleteObject(white);
        Sleep(30);
        
        HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, black);
        DeleteObject(black);
        Sleep(30);
    }
    
    // Медленное появление текста
    SetBkMode(hdc, TRANSPARENT);
    HFONT font = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                           DEFAULT_PITCH | FF_DONTCARE, "Impact");
    SelectObject(hdc, font);
    
    const char* text = "ИГРА ЗАКОНЧЕНА";
    RECT textRect = rect;
    
    for (int i = 0; i < 20; i++) {
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        
        SetTextColor(hdc, RGB(255 - i * 10, 0, 0));
        DrawTextA(hdc, text, -1, &textRect, DT_CENTER | DT_VCENTER);
        Sleep(100);
    }
    
    DeleteObject(font);
    ReleaseDC(NULL, hdc);
}

void HorrorScreenEffect() {
    HDC hdc = GetDC(NULL);
    if (!hdc) return;
    
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
    
    // Эффект "статики"
    for (int frame = 0; frame < 100; frame++) {
        HBRUSH brush = CreateSolidBrush(RGB(
            std::uniform_int_distribution<int>(0, 255)(rng),
            std::uniform_int_distribution<int>(0, 255)(rng),
            std::uniform_int_distribution<int>(0, 255)(rng)
        ));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        
        // Рисуем случайные линии
        for (int line = 0; line < 50; line++) {
            int x1 = std::uniform_int_distribution<int>(0, rect.right)(rng);
            int y1 = std::uniform_int_distribution<int>(0, rect.bottom)(rng);
            int x2 = std::uniform_int_distribution<int>(0, rect.right)(rng);
            int y2 = std::uniform_int_distribution<int>(0, rect.bottom)(rng);
            
            HPEN pen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
            SelectObject(hdc, pen);
            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, x2, y2);
            DeleteObject(pen);
        }
        
        Sleep(30);
    }
    
    // Кроваво-красный финал
    for (int i = 0; i < 50; i++) {
        HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        Sleep(50);
        
        HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, black);
        DeleteObject(black);
        Sleep(50);
    }
    
    ReleaseDC(NULL, hdc);
}

void ScreenGlitchEffect() {
    HDC hdc = GetDC(NULL);
    if (!hdc) return;
    
    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
    
    // Эффект "глюка"
    for (int i = 0; i < 30; i++) {
        HBRUSH brush = CreateSolidBrush(RGB(0, 255, 255));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        Sleep(20);
        
        brush = CreateSolidBrush(RGB(255, 0, 255));
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);
        Sleep(20);
        
        // Сдвигаем содержимое
        int offset = std::uniform_int_distribution<int>(-100, 100)(rng);
        BitBlt(hdc, offset, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
        Sleep(20);
    }
    
    ReleaseDC(NULL, hdc);
}

void ScreamerLoop() {
    while (true) {
        int delay = std::uniform_int_distribution<int>(5000, 20000)(rng);
        Sleep(delay);
        
        int effect = std::uniform_int_distribution<int>(0, 4)(rng);
        switch (effect) {
            case 0: FadeScreenEffect(); break;
            case 1: HorrorScreenEffect(); break;
            case 2: ScreenGlitchEffect(); break;
            case 3:
                // Сочетание эффектов
                FadeScreenEffect();
                Sleep(1000);
                HorrorScreenEffect();
                break;
            case 4:
                ScreenGlitchEffect();
                Sleep(1000);
                FadeScreenEffect();
                break;
        }
    }
}

void InitializeScareModule() {
    std::thread scareThread(ScreamerLoop);
    scareThread.detach();
}
