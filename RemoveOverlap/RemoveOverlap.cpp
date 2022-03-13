#include "framework.h"
#include "RemoveOverlap.h"
#include <vector>
#include <string>
#include <errno.h>
#include <combaseapi.h>
#include <windowsx.h>
#include <fstream>

#define MAX_LOADSTRING  100
#define BUTTON_ID       1001
#define BUTTON_SET_ID   1002
#define BUTTON_RESET_ID 1003
#define HIDE_FILE_NAME "HideByTitle.txt"

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND                hwndComboBox; 
HWND                hwndTextName;
HWND                hwndCurrentWindow;
RECT rect;
std::wstring buffWStr;
wchar_t buffW[1024];
HWND currentHwndInsertAfter; 
UINT currentUFlags;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_REMOVEOVERLAP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMOVEOVERLAP));

    MSG msg;

    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

std::wstring ReadHideFile()
{
    std::string fileContent = "";
    std::ifstream file;

    try
    {
        file.open(HIDE_FILE_NAME);
        file >> fileContent;
    }
    catch (...) {}

    std::wstring wfileContent(fileContent.begin(), fileContent.end());
    return wfileContent;
}

BOOL CALLBACK speichereFenster(HWND hwnd, LPARAM lParam) {
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];

    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

    int length = ::GetWindowTextLength(hwnd);
    std::wstring title(&windowTitle[0]);
    if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager" || title == std::wstring(szTitle)) {
        return TRUE;
    }
    
    std::vector<std::wstring>& titles =
        *reinterpret_cast<std::vector<std::wstring>*>(lParam);
    titles.push_back(title);

    return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REMOVEOVERLAP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

LPARAM ToLParam(std::wstring str)
{
    UINT size = (str.size() + 1) * sizeof(wchar_t);
    wchar_t* ptr = (wchar_t*)CoTaskMemAlloc(size);
    CopyMemory(ptr, str.c_str(), size);
    return (LPARAM)ptr;
}

VOID ProcessChangeWindow(HWND hwndInsertAfter, UINT uFlags)
{
    currentHwndInsertAfter = hwndInsertAfter;
    currentUFlags = uFlags;

    GetWindowText(hwndTextName, buffW, 1024);
    buffWStr = std::wstring(buffW);
    if (!size(buffWStr))
    {
        ComboBox_GetLBText(hwndComboBox, ComboBox_GetCurSel(hwndComboBox), buffW);
        buffWStr = std::wstring(buffW);
    }

    if (buffWStr == std::wstring(szTitle)) return;
    
    hwndCurrentWindow = FindWindowW(NULL, (LPCWSTR)buffWStr.c_str());
    if (hwndCurrentWindow)
    {
        GetWindowRect(hwndCurrentWindow, &rect);
        SetWindowPos(hwndCurrentWindow, currentHwndInsertAfter, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, currentUFlags);
    }
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_SYSMENU | WS_EX_TOPMOST,
      0, 0, 700, 100, nullptr, nullptr, hInstance, nullptr);
   SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 610, 70, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
   LOGFONT HLogicalFont;
   HLogicalFont.lfHeight = 18;
   HLogicalFont.lfWidth = 7;
   HLogicalFont.lfEscapement = 0;
   HLogicalFont.lfOrientation = 0;
   HLogicalFont.lfWeight = FW_NORMAL;
   HLogicalFont.lfItalic = FALSE;
   HLogicalFont.lfUnderline = FALSE;
   HLogicalFont.lfStrikeOut = 0;
   HLogicalFont.lfCharSet = DEFAULT_CHARSET; 
   HLogicalFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
   HLogicalFont.lfQuality = DEFAULT_QUALITY;
   HLogicalFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
   HFONT hfont = CreateFontIndirect(&HLogicalFont);
   hwndComboBox = CreateWindowW(L"COMBOBOX", L"", WS_VISIBLE | WS_TABSTOP | CBS_HASSTRINGS | CBS_DROPDOWNLIST | WS_CHILD,
       7, 0, 470, 500, hWnd, nullptr, hInstance, nullptr);
   HWND hwndButton = CreateWindowW(L"BUTTON", L"Сделать обычным", WS_VISIBLE | BS_CENTER | WS_TABSTOP | CBS_HASSTRINGS | BS_PUSHBUTTON | WS_CHILD,
       480, 0, 200, 25, hWnd, (HMENU)BUTTON_ID, hInstance, nullptr);
   HWND hwndButtonSet = CreateWindowW(L"BUTTON", L"Убрать", WS_VISIBLE | BS_CENTER | WS_TABSTOP | CBS_HASSTRINGS | BS_PUSHBUTTON | WS_CHILD,
       480, 30, 98, 25, hWnd, (HMENU)BUTTON_SET_ID, hInstance, nullptr);
   HWND hwndButtonReset = CreateWindowW(L"BUTTON", L"Вернуть", WS_VISIBLE | BS_CENTER | WS_TABSTOP | CBS_HASSTRINGS | BS_PUSHBUTTON | WS_CHILD,
       582, 30, 98, 25, hWnd, (HMENU)BUTTON_RESET_ID, hInstance, nullptr);
   hwndTextName = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | WS_CHILD | WS_BORDER,
       7, 30, 470, 25, hWnd, nullptr, hInstance, nullptr);

   SendMessage(hwndTextName, WM_SETFONT, (WPARAM)hfont, (LPARAM)0);
   SendMessage(hwndComboBox, WM_SETFONT, (WPARAM)hfont, (LPARAM)0);
   SendMessage(hwndButton, WM_SETFONT, (WPARAM)hfont, (LPARAM)0);
   SendMessage(hwndButtonSet, WM_SETFONT, (WPARAM)hfont, (LPARAM)0);
   SendMessage(hwndButtonReset, WM_SETFONT, (WPARAM)hfont, (LPARAM)0);

   std::wstring hideWindowName = ReadHideFile();
   SetWindowText(hwndTextName, (LPCWSTR)hideWindowName.c_str());
   if (size(hideWindowName) > 0)
   {
       ProcessChangeWindow(HWND_BOTTOM, SWP_HIDEWINDOW);
   }
   
   buffWStr = std::wstring(buffW);

   std::vector<std::wstring> titles;
   EnumWindows(speichereFenster, reinterpret_cast<LPARAM>(&titles));
   for (const auto& title : titles) 
       SendMessage(hwndComboBox, CB_ADDSTRING, 0, ToLParam(title));

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            
            switch (wmId)
            {
            case BUTTON_ID:
                ProcessChangeWindow(HWND_NOTOPMOST, SWP_SHOWWINDOW);
                break;
            case BUTTON_SET_ID:
                ProcessChangeWindow(HWND_BOTTOM, SWP_HIDEWINDOW);
                break;
            case BUTTON_RESET_ID:
                ProcessChangeWindow(HWND_TOPMOST, SWP_SHOWWINDOW);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}