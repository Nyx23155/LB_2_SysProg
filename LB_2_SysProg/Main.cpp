
#include <windows.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include "resource.h"

// Глобальні змінні
HINSTANCE hInst;
LPCTSTR szWindowClass = L"Lab2Variant7";
LPCTSTR szTitle = L"Лабораторна робота №2 - Варіант 7";

const int SQUARE_SIZE = 80;

// Структура для зберігання інформації про квадрат
struct Square {
    int originalX;      // Оригінальна координата X в сітці (1-базована)
    int originalY;      // Оригінальна координата Y в сітці (1-базована)
    int currentIndex;   // Поточна позиція в масиві (для відображення)
    COLORREF color;     // Колір квадрата
};

std::vector<Square> squares;
int gridCols = 0;
int gridRows = 0;

// Попередній опис функцій
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
void InitializeSquares(int cols, int rows);
COLORREF GetRandomColor();
COLORREF GetInverseColor(COLORREF color);
void SwapSquares();
void DrawSquares(HDC hdc, int cols, int rows);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    srand((unsigned)time(NULL));

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(ID_FUNCTIONS_SWAP);  // Меню з ресурсів
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    hInst = hInstance;

    hWnd = CreateWindow(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void InitializeSquares(int cols, int rows)
{
    // Зберігаємо старі кольори якщо квадрати вже існують
    std::vector<COLORREF> oldColors;
    std::vector<int> oldOrigX;
    std::vector<int> oldOrigY;

    for (size_t i = 0; i < squares.size(); i++)
    {
        oldColors.push_back(squares[i].color);
        oldOrigX.push_back(squares[i].originalX);
        oldOrigY.push_back(squares[i].originalY);
    }

    squares.clear();

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Square sq;
            sq.originalX = x + 1;
            sq.originalY = y + 1;
            sq.currentIndex = y * cols + x;

            // Шукаємо чи був квадрат з такими координатами раніше
            bool found = false;
            for (size_t i = 0; i < oldColors.size(); i++)
            {
                if (oldOrigX[i] == sq.originalX && oldOrigY[i] == sq.originalY)
                {
                    sq.color = oldColors[i];
                    found = true;
                    break;
                }
            }

            // Якщо не знайшли - створюємо новий випадковий колір
            if (!found)
            {
                sq.color = GetRandomColor();
            }

            squares.push_back(sq);
        }
    }
}

COLORREF GetRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

// Функція обчислення протилежного кольору відносно 122
COLORREF GetInverseColor(COLORREF color)
{
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);

    // Обчислення протилежного кольору відносно 122
    // Формула: новий_колір = 255 - старий_колір + 122
    r = 255 - r + 122;
    g = 255 - g + 122;
    b = 255 - b + 122;

    // Обмеження значень в межах 0-255
    r = max(0, min(255, r));
    g = max(0, min(255, g));
    b = max(0, min(255, b));

    return RGB(r, g, b);
}

void SwapSquares()
{
    if (squares.empty()) return;

    // Створюємо копію поточних індексів
    std::vector<int> indices;
    for (size_t i = 0; i < squares.size(); i++)
    {
        indices.push_back(squares[i].currentIndex);
    }

    // Перемішуємо індекси випадковим чином
    std::random_shuffle(indices.begin(), indices.end());

    // Присвоюємо нові індекси квадратам (змінюємо їх позицію)
    for (size_t i = 0; i < squares.size(); i++)
    {
        squares[i].currentIndex = indices[i];
    }
}

void DrawSquares(HDC hdc, int cols, int rows)
{
    // Створюємо тимчасовий масив для відображення квадратів в правильному порядку
    std::vector<Square*> displaySquares(squares.size(), nullptr);

    for (size_t i = 0; i < squares.size(); i++)
    {
        if (squares[i].currentIndex < (int)displaySquares.size())
        {
            displaySquares[squares[i].currentIndex] = &squares[i];
        }
    }

    // Малюємо квадрати
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            int index = y * cols + x;
            if (index >= (int)displaySquares.size() || displaySquares[index] == nullptr)
                continue;

            Square* sq = displaySquares[index];

            // Обчислюємо позицію квадрата на екрані
            int posX = x * SQUARE_SIZE;
            int posY = y * SQUARE_SIZE;

            // Малюємо квадрат
            HBRUSH hBrush = CreateSolidBrush(sq->color);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            Rectangle(hdc, posX, posY, posX + SQUARE_SIZE, posY + SQUARE_SIZE);

            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);

            // Підготовка тексту
            WCHAR text[100];
            int r = GetRValue(sq->color);
            int g = GetGValue(sq->color);
            int b = GetBValue(sq->color);

            wsprintf(text, L"(%d;%d)\nRGB(%d,%d,%d)",
                sq->originalX, sq->originalY, r, g, b);

            // Встановлюємо протилежний колір тексту відносно 122
            COLORREF textColor = GetInverseColor(sq->color);
            SetTextColor(hdc, textColor);
            SetBkMode(hdc, TRANSPARENT);

            // Малюємо текст
            RECT textRect;
            textRect.left = posX + 5;
            textRect.top = posY + 5;
            textRect.right = posX + SQUARE_SIZE - 5;
            textRect.bottom = posY + SQUARE_SIZE - 5;

            DrawText(hdc, text, -1, &textRect, DT_LEFT | DT_TOP | DT_WORDBREAK);
        }
    }
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
    {
        // Ініціалізація при створенні вікна
        RECT rect;
        GetClientRect(hWnd, &rect);
        gridCols = rect.right / SQUARE_SIZE;
        gridRows = rect.bottom / SQUARE_SIZE;
        InitializeSquares(gridCols, gridRows);
        break;
    }

    case WM_SIZE:
    {
        // Перемальовування при зміні розміру вікна
        RECT rect;
        GetClientRect(hWnd, &rect);
        int newCols = rect.right / SQUARE_SIZE;
        int newRows = rect.bottom / SQUARE_SIZE;

        // Ініціалізуємо квадрати тільки якщо розмір сітки змінився
        if (newCols != gridCols || newRows != gridRows)
        {
            gridCols = newCols;
            gridRows = newRows;
            InitializeSquares(gridCols, gridRows);
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_FUNCTIONS_SWAP:
            // Заміна позицій квадратів
            SwapSquares();
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
        break;
    }

    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);

        // Малюємо квадрати
        DrawSquares(hdc, gridCols, gridRows);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}