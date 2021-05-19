// Lab_3_moto.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Lab_3_moto.h"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Resource.h"
#include <mmsystem.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib, "Winmm.lib")

#define MAX_LOADSTRING 100

// Глобальные переменные:
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;

int step = 0;
float distance = 0.f;
float angle = 0.f;
float t = 0.f;
UINT frameIndex;
UINT frameCount;

Image* image; // задний фон
Image* protectorImg;
Image* metalImg;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void Display(HDC);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB3MOTO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3MOTO));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3MOTO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB3MOTO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), nullptr, nullptr, hInstance, nullptr);

   protectorImg = Image::FromFile(L"protector.jpg");
   metalImg = Image::FromFile(L"metall.jpg");   
   PlaySoundA("sound.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        image = Image::FromFile(L"background.gif");
        if (image == NULL) return -1;

        frameIndex = 0;
        frameCount = image->GetFrameCount(&FrameDimensionTime);

        SetTimer(hWnd, 1, 10, NULL);
        return 0;
    }
    case WM_TIMER:
    {
        frameIndex = (frameIndex + 1) % frameCount;
        image->SelectActiveFrame(&FrameDimensionTime, frameIndex);
        step++;
        t += 0.005f;
        InvalidateRect(hWnd, NULL, FALSE);
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            Display(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (protectorImg != NULL)
        {
            delete protectorImg;
        }
        if (metalImg != NULL)
        {
            delete metalImg;
        }
        if (image != NULL)
        {
            delete image;
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


PointF Tween(const PointF& A, const PointF& B, float t)
{
    return PointF(A.X * (1.f - t) + B.X * t, A.Y * (1.f - t) + B.Y * t);
}

void Tween(const PointF* A, const PointF* B, PointF* P, int count, float t)
{
    for (int i = 0; i < count; i++)
    {
        P[i] = Tween(A[i], B[i], t);
    }
}

void DrawMoto(Graphics& g)
{
    Rect rect;
    g.GetVisibleClipBounds(&rect);

    Bitmap backBuffer(rect.Width, rect.Height, &g);

    Graphics temp(&backBuffer);

    temp.SetSmoothingMode(SmoothingModeHighQuality);

    TextureBrush tb(protectorImg);

    TextureBrush tbm(metalImg);

    SolidBrush sbBlack(Color::Black);
    SolidBrush sbGray(Color::Gray);
    SolidBrush sbBlue(Color::Blue);
    Pen blackPen(Color::Black);

    //линейный градиент
    LinearGradientBrush lgb(Rect(100, 100, 700, 700), Color::White, Color::Black, 45.f);
    Color colors[6] =
    {
        Color(255, 225, 240, 225),
        Color(255, 210, 105, 30),
        Color(255, 255, 140, 0)
    };

    //ширина областей градиента
    float pos[3] = {
    0.0f,
    0.7f,
    1.0f };

    //текстурное перо (метал)
    Pen TexturePen(&tbm, 8.f);
    TexturePen.SetStartCap(LineCapRound);
    TexturePen.SetLineJoin(LineJoinRound);
    TexturePen.SetEndCap(LineCapRound);

    g.DrawEllipse(&blackPen, 350, 420, 120, 120);    //заднее колесо    
    g.DrawEllipse(&blackPen, 630, 420, 120, 120);    //переднее колесо

    //заливка протектором
    g.FillEllipse(&tb, 350, 420, 120, 120);    //заднее колесо    
    g.FillEllipse(&tb, 630, 420, 120, 120);    //переднее колесо

    HatchBrush hatchBrush(HatchStyleOutlinedDiamond, Color::Gray, Color::White);//спици
    g.FillEllipse(&hatchBrush, 375, 445, 70, 70);    //заднее колесо    
    g.FillEllipse(&hatchBrush, 655, 445, 70, 70);    //переднее колесо

    // корпус
    const int i = 5;
    Point framePoints[i] = {
        Point(510, 460),
        Point(600, 460),
        Point(700, 360),
        Point(410, 360),
        Point(510, 460),
    };

    g.DrawLines(&blackPen, framePoints, i);

    lgb.SetInterpolationColors(colors, pos, 3);
    g.FillPolygon(&lgb, framePoints, i);

    // Руль
    Point wheelPoints[3] = {
        Point(690, 480),
        Point(627, 320),
        Point(600, 320)
    };
    g.DrawLines(&TexturePen, wheelPoints, 3);

    // задняя балка
    Point balcPoints[3] = {
        Point(450, 400),
        Point(410, 480),
        Point(510, 460),
    };
    g.DrawLines(&TexturePen, balcPoints, 3);

    // сидушка
    Point sitsPoints[4] = {
        Point(450, 360),
        Point(470, 390),
        Point(580, 360),
        Point(450, 360),
    };

    g.FillPolygon(&sbBlack, sitsPoints, 4);
}

void DrawRocket(Graphics& g)
{
    Rect rect;
    g.GetVisibleClipBounds(&rect);
    Bitmap backbuffer(rect.Width, rect.Height, &g);

    Graphics temp(&backbuffer);
    temp.SetSmoothingMode(SmoothingModeHighQuality);
    Pen blackpen(Color::Black, 2.f);
    SolidBrush nose_brush(Color::Red);
    SolidBrush body_brush(Color::Black);

    PointF start_noseRocket_Points[] = {
        PointF(1300.f, 500.f),
        PointF(1350.f, 480.f),
        PointF(1350.f, 520.f)
    };
    PointF start_bodyRocket_Points[] = {
        PointF(1350.f, 490.f),
        PointF(1450.f, 490.f),
        PointF(1450.f, 510.f),
        PointF(1350.f, 510.f),
    };

    PointF end_noseRocket_Points[] = {
        PointF(0.f, 500.f),
        PointF(50.f, 480.f),
        PointF(50.f, 520.f)
    };
    PointF end_bodyRocket_Points[] = {
        PointF(50.f, 490.f),
        PointF(150.f, 490.f),
        PointF(150.f, 510.f),
        PointF(50.f, 510.f),
    };

    PointF int_noseRocket_Points[3];
    PointF int_bodyRocket_Points[4];

    Tween(start_noseRocket_Points, end_noseRocket_Points, int_noseRocket_Points, 3, t);
    Tween(start_bodyRocket_Points, end_bodyRocket_Points, int_bodyRocket_Points, 4, t);

    g.FillPolygon(&nose_brush, int_noseRocket_Points, 3);
    g.FillPolygon(&body_brush, int_bodyRocket_Points, 4);
}

void Display(HDC hdc)
{
    Graphics g(hdc);

    Rect rect;
    g.GetVisibleClipBounds(&rect);

    Bitmap backBuffer(rect.Width, rect.Height, &g);

    Graphics temp(&backBuffer);
    temp.DrawImage(image, 0, 0, rect.Width, rect.Height);
    //temp.TranslateTransform(-350.f, 0.f);
    distance = 5.f * (step - 50);
    angle = -90.f * step / (frameCount+400);
    if (t < 1)
    {
        DrawRocket(temp);
    }
    else
    {
        t = 0;
    }
    if (step < 50)
    {

        temp.TranslateTransform(0.f + distance, 0.f);
        DrawMoto(temp);
    }
    else if (step < 150)
    {
        temp.TranslateTransform(0.f + distance, 0.f);
        temp.RotateTransform(angle+10);
        DrawMoto(temp);
    }
    else
    {
        step = -1;
        temp.ResetTransform();
    }

    g.DrawImage(&backBuffer, rect);

}
