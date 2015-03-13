/* Copyright (c) <2013-2015> <Aaron Springstroh, Min() Game Engine>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgement in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

This file is part of Min() Game Engine.

Min() Game Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Min() Game Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Min() Game Engine.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "StdAfx.h"
#include "OpenGLWindow.h"
#include "Camera.h"
#include "SceneManager.h"

size_t OpenGLWindow::MAX_LOADSTRING = 100;

OpenGLWindow::OpenGLWindow(HINSTANCE hInstance, int nCmdShow, const std::wstring& title, const std::wstring& windowclass, size_t height, size_t width)
{
	_title = title;
	_windowClass = windowclass;
	_hInstance = hInstance;

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow, height, width))
	{
		return;
	}
	_context->setupScene();

	_hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENGL));
}

OpenGLWindow::~OpenGLWindow(void)
{

}

int OpenGLWindow::Invoke(void (*f)(void))
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, _hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		_context->renderScene();
		f();
	}
	return (int) msg.wParam;
}

OpenGLContext* OpenGLWindow::GetContext()
{
	return _context;
}

ATOM OpenGLWindow::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= OpenGLWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENGL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OPENGL);
	wcex.lpszClassName	= _windowClass.c_str();
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL OpenGLWindow::InitInstance(HINSTANCE hInstance, int nCmdShow, int width, int height)
{
   HWND hWnd;

   hWnd = CreateWindow(_windowClass.c_str(), _title.c_str(), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, this);

   if (!hWnd)
   {
      return FALSE;
   }

   _context = new OpenGLContext(hWnd, &_sysClock);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK  OpenGLWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	OpenGLWindow* window;

	switch (message)
	{
		case WM_CREATE:
		{
			CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<OpenGLWindow*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
			break;
		}
		case WM_COMMAND:
		{
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_ABOUT:
				DialogBox(window->_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		case WM_SIZE:
		{
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			window->_context->reshapeWindow(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd,&ps);
			EndPaint(hWnd,&ps);
			break;
		}
		case WM_KEYDOWN:
		{
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			window->_context->KeyMap.Keydown(wParam);
			break;
		}
		case WM_KEYUP:
		{
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			window->_context->KeyMap.Keyup(wParam);
		}
		case WM_LBUTTONDOWN:
		{
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			window->_context->KeyMap.Call(0x01);
			break;
		}
		case WM_DESTROY:
		{
			window = (OpenGLWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			delete window->_context;
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK  OpenGLWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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