// test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "test.h"

#include "..\..\source\api.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//checka();
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }


   char * sKey1 = "-8_kV]T~";
   char * sEncode1 = "OThkZDY5ZjE0ZWE2MDM3Njg4Yzg5NjI4ZjRhNzhkMzczZjZhYjIyOWQ0MjQxYThmZWM5OTdjYTIxOGI0YmQ0OGZiMzNjNmZiMGEyMTNlMTFkZTU2ZTMzZjUxNGQwNWQ4ZDg3MzU3NmRiZGMyYjliMWJjYzZjNmY0NmFkMjVhY2EyM2U0MTRkYjNkN2I4OTg5NGU0MWZhZmQxNDQxZTIwOTY1NTg2OGYxMjljOWU1NDFhODNlY2FhNjJkMWU5MjdhMWRhYzZhZGYxNGUzYjE0YWQ1MmZhM2E2NDg2MjViMmVkMzZkNzhmZTI5NTUwMTUyMTMxZDM3NWZkOGE5NTExZA==";
	//"98dd69f14ea6037688c89628f4a78d373f6ab229d4241a8fec997ca218b4bd48fb33c6fb0a213e11de56e33f514d05d8d873576dbdc2b9b1bcc6c6f46ad25aca23e414db3d7b89894e41fafd1441e209655868f129c9e541a83ecaa62d1e927a1dac6adf14e3b14ad52fa3a648625b2ed36d78fe29550152131d375fd8a9511d"
	//"ODM2MzAwNmNlN2RmYjFiZWRmYTRmOWM0OTZjMTJjY2YyZTVjNjU4OTJjM2U4ZDk5MWEyMDNiNzVlNjA0MGE2YWJjNjZhOGY0YWY4YzljNTQ1YjI0YTZjODFkY2Q1MTU3"

	char * sKey2 = "f9#2;D%=";
	char * sEncode2 = "ODM2MzAwNmNlN2RmYjFiZWRmYTRmOWM0OTZjMTJjY2YyZTVjNjU4OTJjM2U4ZDk5MWEyMDNiNzVlNjA0MGE2YWJjNjZhOGY0YWY4YzljNTQ1YjI0YTZjODFkY2Q1MTU3";
	//"8363006ce7dfb1bedfa4f9c496c12ccf2e5c65892c3e8d991a203b75e6040a6abc66a8f4af8c9c545b24a6c81dcd5157"
	//"{fd#2D%YN(x/kop[4q)aP*o!Gk;-8_CV]T~cZnm.H;l=}"


	char * sKey3 = "f%/4*;VZ";
	char * sEncode3 = "OWJkOTdlYmY4MDdlZmY4NDA3MTVlMzg1ZTAyNTViZjlmYTU1ZWVhYzc0YzM0NjNkYmJmYTQxZjNkNDA0ODQ5MDZjNTdkZWViOGY5YTk5YTYxN2QzOWNjMjA2YjFlMGIxZDVkZDFkMWExOTMwZDhlNGQzOGU4YmIwYzZjOGRiM2RmMGI5Yzk0Nzc4ODk1N2I0MTI1OGEyNzY1NTllZDI5MWNiNzZlOTM3YTJhMjEwNmI1YTc5YzM2NTI3MjZmZGY2YjZmMzc5N2RlNDdiOGUwZGYwNzY0ZDAzYzIwYTg2MTZlNzFmYzAxZDk0OTcyYjU2NTIxMzJmZmY1NDc2MDllMGJiMWExYjUyMjYwMjBkYjJkMzE3MDExNjJkN2EyOGQ3Y2U0MzVkZWQwYWI0YmNlMQ==";
	// "9bd97ebf807eff840715e385e0255bf9fa55eeac74c3463dbbfa41f3d40484906c57deeb8f9a99a617d39cc206b1e0b1d5dd1d1a1930d8e4d38e8bb0c6c8db3df0b9c947788957b41258a276559ed291cb76e937a2a2106b5a79c3652726fdf6b6f3797de47b8e0df0764d03c20a8616e71fc01d94972b5652132fff547609e0bb1a1b5226020db2d31701162d7a28d7ce435ded0ab4bce1"
	// "YjAzNGUyYWY4YTI3NjY2ZTA4OWU0NTE0ZjI5OWU5NmNmOTc0YjA1MDQ5OTUyMDczNjVhMDJlMjM0NWQyOTA1NWYwZmYzNTg0N2E5MjZiZWRmOTA4YjViMWQwNzQ0OTEyODkxZDQwOGIxMGU4Y2MxMw=="


	char * sKey4 = "dDN/pqP!";
	char * sEncode4 = "YjAzNGUyYWY4YTI3NjY2ZTA4OWU0NTE0ZjI5OWU5NmNmOTc0YjA1MDQ5OTUyMDczNjVhMDJlMjM0NWQyOTA1NWYwZmYzNTg0N2E5MjZiZWRmOTA4YjViMWQwNzQ0OTEyODkxZDQwOGIxMGU4Y2MxMw==";
	//"b034e2af8a27666e089e4514f299e96cf974b0504995207365a02e2345d29055f0ff35847a926bedf908b5b1d0744912891d408b10e8cc13"
	char * pSrc = "trial_null_2019-02-15 15:01:17_2019-05-17 15:01:17_9999";


   char sbuf[1024] = {0};
   //sub_1001F960((int)sbuf, sEncode4, sKey4);

   encodeData(sbuf, pSrc, sKey4);

   char sbuf1[1024] = {0};
   encodeData(sbuf1, sbuf, sKey3);

   bool be = strcmp(sEncode3, sbuf1);

   decodeData(sbuf1, sbuf, sKey4);







   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
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
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
