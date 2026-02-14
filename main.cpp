/*
 * TofuMental - Todo App for Sharp Brain PW-SH2 (Windows Embedded CE 6.0)
 * Philosophy: Nothing OS Industrial Minimalism + Apple HIG Precision.
 * Features: 8pt Grid, Dot-Matrix Headers, LiquidGlass Simulation.
 */

#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vector>
#include <string>
#include <ctime>
#include <math.h>
#include <stdio.h>

// --- Windows CE Boilerplate ---
#ifndef WS_OVERLAPPEDWINDOW
#define WS_OVERLAPPEDWINDOW \
  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
#endif

#ifdef UNDER_CE
#if SW_MAXIMIZE != 12
#undef SW_MAXIMIZE
#define SW_MAXIMIZE 12
#endif
#ifndef _tWinMain
#define _tWinMain WinMain
#endif
#endif

#include <tchar.h>

// --- Design Constants (8pt Grid) ---
#define GRID_UNIT 8
#define MARGIN_X (GRID_UNIT * 3) // 24pt
#define MARGIN_Y (GRID_UNIT * 4) // 32pt
#define ITEM_HEIGHT (GRID_UNIT * 6) // 48pt
#define CORNER_RADIUS GRID_UNIT

// Colors
#define CLR_BG          RGB(0, 0, 0)
#define CLR_TEXT_PRI    RGB(255, 255, 255)
#define CLR_TEXT_SEC    RGB(160, 160, 160)
#define CLR_ACCENT      RGB(255, 255, 255)
#define CLR_GLASS_BORDER RGB(60, 60, 60)

struct Task {
    std::wstring title;
    bool completed;
    Task() : title(L""), completed(false) {}
    Task(const std::wstring& t) : title(t), completed(false) {}
};

enum AppMode { MODE_LIST, MODE_ADD };

// Globals
std::vector<Task> tasks;
int selectedIndex = 0;
AppMode currentMode = MODE_LIST;
RECT clientRect;
HFONT hFontMain = NULL;
HFONT hFontDot = NULL;

// --- Persistence ---

void SaveTasks() {
    FILE* fp = _wfopen(L"tasks.txt", L"w, ccs=UTF-16LE");
    if (!fp) return;
    for (size_t i = 0; i < tasks.size(); ++i) {
        fwprintf(fp, L"%ls|%d\n", tasks[i].title.c_str(), tasks[i].completed ? 1 : 0);
    }
    fclose(fp);
}

void LoadTasks() {
    FILE* fp = _wfopen(L"tasks.txt", L"r, ccs=UTF-16LE");
    if (!fp) {
        // Default tasks if file doesn't exist
        tasks.push_back(Task(L"Eat Tofu"));
        tasks.push_back(Task(L"Stay Mental"));
        tasks.push_back(Task(L"Build PW-SH2 Apps"));
        return;
    }
    tasks.clear();
    wchar_t line[256];
    while (fgetws(line, 256, fp)) {
        std::wstring ws(line);
        size_t sep = ws.find_last_of(L'|');
        if (sep != std::wstring::npos) {
            std::wstring title = ws.substr(0, sep);
            bool completed = (ws.substr(sep + 1, 1) == L"1");
            tasks.push_back(Task(title));
            tasks.back().completed = completed;
        }
    }
    fclose(fp);
}

// --- UI Helpers ---

void DrawDotMatrixChar(HDC hdc, int x, int y, TCHAR c, COLORREF color) {
    UNREFERENCED_PARAMETER(hdc);
    UNREFERENCED_PARAMETER(x);
    UNREFERENCED_PARAMETER(y);
    UNREFERENCED_PARAMETER(c);
    UNREFERENCED_PARAMETER(color);
}

void DrawRoundedRect(HDC hdc, RECT r, int radius, COLORREF borderCol, COLORREF fillCol) {
    HBRUSH hBrush = CreateSolidBrush(fillCol);
    HPEN hPen = CreatePen(PS_SOLID, 1, borderCol);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius * 2, radius * 2);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void InitApp() {
    LoadTasks();
}

// --- Window Procedure ---

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            InitApp();
            GetClientRect(hWnd, &clientRect);
            LOGFONT lfMain;
            memset(&lfMain, 0, sizeof(lfMain));
            lfMain.lfHeight = 22;
            lfMain.lfWeight = FW_SEMIBOLD;
            lfMain.lfQuality = ANTIALIASED_QUALITY;
            lstrcpy(lfMain.lfFaceName, TEXT("MS PGothic"));
            hFontMain = CreateFontIndirect(&lfMain);

            LOGFONT lfDot;
            memset(&lfDot, 0, sizeof(lfDot));
            lfDot.lfHeight = 14;
            lfDot.lfWeight = FW_NORMAL;
            lfDot.lfQuality = NONANTIALIASED_QUALITY;
            lfDot.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
            lstrcpy(lfDot.lfFaceName, TEXT("Courier New"));
            hFontDot = CreateFontIndirect(&lfDot);
            break;
        }

        case WM_SIZE:
            GetClientRect(hWnd, &clientRect);
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case WM_LBUTTONDOWN: {
            int x = (int)(short)LOWORD(lParam);
            int y = (int)(short)HIWORD(lParam);
            
            int currentY = MARGIN_Y;
            for (int i = 0; i < (int)tasks.size(); ++i) {
                RECT itemRect = { MARGIN_X, currentY, clientRect.right - MARGIN_X, currentY + ITEM_HEIGHT };
                if (x >= itemRect.left && x <= itemRect.right && y >= itemRect.top && y <= itemRect.bottom) {
                    selectedIndex = i;
                    // Check if tapped on the left indicator or title
                    if (x < itemRect.left + GRID_UNIT * 4) {
                        tasks[i].completed = !tasks[i].completed;
                        SaveTasks();
                    }
                    InvalidateRect(hWnd, NULL, FALSE);
                    return 0;
                }
                currentY += ITEM_HEIGHT + 2;
            }
            break;
        }

        case WM_CHAR:
            if (currentMode == MODE_ADD) {
                if (wParam == VK_RETURN) {
                    currentMode = MODE_LIST;
                    SaveTasks();
                } else if (wParam == VK_BACK) {
                    if (!tasks[selectedIndex].title.empty()) {
                        tasks[selectedIndex].title.erase(tasks[selectedIndex].title.size() - 1);
                    }
                } else if (wParam >= 32) { // Printable characters
                    tasks[selectedIndex].title += (wchar_t)wParam;
                }
                InvalidateRect(hWnd, NULL, FALSE);
                return 0;
            }
            break;

        case WM_KEYDOWN:
            if (currentMode == MODE_LIST) {
                switch (wParam) {
                    case VK_UP:
                        selectedIndex = (selectedIndex <= 0) ? (int)tasks.size() - 1 : selectedIndex - 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                        break;
                    case VK_DOWN:
                        selectedIndex = (selectedIndex >= (int)tasks.size() - 1) ? 0 : selectedIndex + 1;
                        InvalidateRect(hWnd, NULL, FALSE);
                        break;
                    case VK_RETURN:
                        if (selectedIndex >= 0 && selectedIndex < (int)tasks.size()) {
                            tasks[selectedIndex].completed = !tasks[selectedIndex].completed;
                            SaveTasks();
                        }
                        InvalidateRect(hWnd, NULL, FALSE);
                        break;
                    case 'A': // Add
                        tasks.push_back(Task(L""));
                        selectedIndex = (int)tasks.size() - 1;
                        currentMode = MODE_ADD;
                        InvalidateRect(hWnd, NULL, FALSE);
                        break;
                    case 'D': // Delete
                    case VK_BACK:
                        if (selectedIndex >= 0 && selectedIndex < (int)tasks.size()) {
                            tasks.erase(tasks.begin() + selectedIndex);
                            if (selectedIndex >= (int)tasks.size()) selectedIndex = (int)tasks.size() - 1;
                            if (selectedIndex < 0) selectedIndex = 0; // Handle case where all tasks are deleted
                            SaveTasks();
                            InvalidateRect(hWnd, NULL, TRUE);
                        }
                        break;
                    case VK_ESCAPE:
                        PostQuitMessage(0);
                        break;
                }
            } else if (currentMode == MODE_ADD) {
                if (wParam == VK_ESCAPE) {
                    currentMode = MODE_LIST;
                    if (tasks[selectedIndex].title.empty()) {
                        tasks.erase(tasks.begin() + selectedIndex);
                        selectedIndex = (int)tasks.size() - 1;
                        if (selectedIndex < 0) selectedIndex = 0;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            break;

        case WM_ERASEBKGND:
            return 1; // Handled

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            // Re-fetch client rect to ensure we never have 0 size if possible
            RECT rect;
            GetClientRect(hWnd, &rect);
            if (rect.right == 0 || rect.bottom == 0) {
                EndPaint(hWnd, &ps);
                return 0;
            }

            // Double Buffering
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

            // 1. Background (Pure Nothing Black)
            HBRUSH bgBrush = CreateSolidBrush(CLR_BG);
            FillRect(hdcMem, &rect, bgBrush);
            DeleteObject(bgBrush);

            // 2. Header (Nothing OS Dot-Matrix Style)
            SelectObject(hdcMem, hFontDot);
            SetTextColor(hdcMem, CLR_TEXT_PRI);
            SetBkMode(hdcMem, TRANSPARENT);
            RECT headerRect = { MARGIN_X, MARGIN_Y / 2, rect.right - MARGIN_X, MARGIN_Y };
            DrawText(hdcMem, (currentMode == MODE_ADD) ? TEXT("::: NEW TASK :::") : TEXT("::: TOFU MENTAL :::"), -1, &headerRect, DT_LEFT | DT_BOTTOM);

            // 3. Task List (Apple HIG Layout)
            int currentY = MARGIN_Y;
            for (int i = 0; i < (int)tasks.size(); ++i) {
                RECT itemRect = { MARGIN_X, currentY, rect.right - MARGIN_X, currentY + ITEM_HEIGHT };
                
                // Selection Highlight (LiquidGlass Simulation)
                if (i == selectedIndex) {
                    // Draw a subtle rounded rect for selection
                    COLORREF highlightCol = (currentMode == MODE_ADD) ? RGB(60,20,20) : RGB(30,30,30);
                    DrawRoundedRect(hdcMem, itemRect, CORNER_RADIUS, CLR_GLASS_BORDER, highlightCol);
                }

                // Checkbox / Dot Indicator
                RECT checkRect = { itemRect.left + GRID_UNIT, itemRect.top + GRID_UNIT, 
                                   itemRect.left + GRID_UNIT * 3, itemRect.top + ITEM_HEIGHT - GRID_UNIT };
                
                if (tasks[i].completed) {
                    HBRUSH hBr = CreateSolidBrush(CLR_TEXT_PRI);
                    HBRUSH hOldB = (HBRUSH)SelectObject(hdcMem, hBr);
                    Ellipse(hdcMem, checkRect.left + 4, checkRect.top + 12, checkRect.right - 4, checkRect.bottom - 12);
                    SelectObject(hdcMem, hOldB);
                    DeleteObject(hBr);
                } else {
                    HPEN hPen = CreatePen(PS_SOLID, 1, CLR_TEXT_SEC);
                    HPEN hOldP = (HPEN)SelectObject(hdcMem, hPen);
                    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
                    Ellipse(hdcMem, checkRect.left + 4, checkRect.top + 12, checkRect.right - 4, checkRect.bottom - 12);
                    SelectObject(hdcMem, hOldP);
                    DeleteObject(hPen);
                }

                // Task Text
                SelectObject(hdcMem, hFontMain);
                SetTextColor(hdcMem, tasks[i].completed ? CLR_TEXT_SEC : CLR_TEXT_PRI);
                std::wstring displayText = tasks[i].title;
                if (i == selectedIndex && currentMode == MODE_ADD && (GetTickCount() / 500) % 2 == 0) {
                    displayText += L"_"; // Blinking cursor
                }
                RECT textRect = { itemRect.left + GRID_UNIT * 5, itemRect.top, itemRect.right, itemRect.bottom };
                DrawText(hdcMem, displayText.c_str(), -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

                currentY += ITEM_HEIGHT + 2; // Subtle gap
            }

            // 4. Footer / Meta (Nothing OS)
            SelectObject(hdcMem, hFontDot);
            SetTextColor(hdcMem, CLR_TEXT_SEC);
            TCHAR footerText[64];
            wsprintf(footerText, TEXT("%s | ITEMS: %02d"), 
                (currentMode == MODE_ADD) ? TEXT("INPUT") : TEXT("DEFAULT"), tasks.size());
            RECT footerRect = { MARGIN_X, rect.bottom - 20, rect.right - MARGIN_X, rect.bottom - 5 };
            DrawText(hdcMem, footerText, -1, &footerRect, DT_RIGHT | DT_SINGLELINE);

            // Blit to screen
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

            // Cleanup
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hbmMem);
            DeleteDC(hdcMem);
            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
            if (hFontMain) DeleteObject(hFontMain);
            if (hFontDot) DeleteObject(hFontDot);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    WNDCLASS wcl;
    wcl.hInstance = hInstance;
    wcl.lpszClassName = TEXT("TofuMental");
    wcl.lpfnWndProc = WindowProc;
    wcl.style = CS_HREDRAW | CS_VREDRAW;
    wcl.hIcon = NULL;
    wcl.hCursor = NULL;
    wcl.lpszMenuName = NULL;
    wcl.cbClsExtra = 0;
    wcl.cbWndExtra = 0;
    wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    if (!RegisterClass(&wcl)) return FALSE;

    HWND hWnd = CreateWindowEx(0, TEXT("TofuMental"), TEXT("TofuMental"), WS_POPUP | WS_VISIBLE,
                               0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 
                               NULL, NULL, hInstance, NULL);

    if (!hWnd) return FALSE;

#ifdef UNDER_CE
    ShowWindow(hWnd, SW_MAXIMIZE);
#else
    ShowWindow(hWnd, nShowCmd);
#endif
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
