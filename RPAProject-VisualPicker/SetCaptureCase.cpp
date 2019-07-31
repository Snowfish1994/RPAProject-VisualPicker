#include "SetCaptureCase.h"
#include"UIADrawerLibrary.h"
#include "UIAObj.h";

HWND pickerDlgHwnd;
HANDLE pickerDlgHandle;
int DIALOG_WIDTH = 160;
int DIALOG_HEIGHT = 200;
COLORREF penColor = 0xf0f;
UIADrawerLibrary drawer = UIADrawerLibrary();
UIAObj uiaObj = UIAObj();

/**Function Defs**/
DWORD WINAPI DlgThreadProc(LPVOID lpParameter);
/**Defs End**/

SetCaptureCase::SetCaptureCase()
{
}

SetCaptureCase::~SetCaptureCase()
{
}

void SetCaptureCase::OpenPickerDlg()
{
	pickerDlgHandle = GetModuleHandle(NULL);
	HANDLE thread = CreateThread(NULL, 0, DlgThreadProc, NULL, 0, NULL);
}

void SetCaptureCase::EndPick()
{
	DestroyWindow(pickerDlgHwnd);
	drawer.EndDraw();
	drawer.~UIADrawerLibrary();
}


BOOL PointAroundRect(CONST RECT *lprc, POINT pt)
{
	RECT rect;
	rect.top = lprc->top - 5;
	rect.left = lprc->left - 5;
	rect.bottom = lprc->bottom + 5;
	rect.right = lprc->right + 5;
	return PtInRect(&rect, pt);
}

void GetPoint(LPARAM lParam, POINT *point)
{
	point->x = (WORD)LOWORD(lParam);
	point->y = (WORD)HIWORD(lParam);

	ClientToScreen(pickerDlgHwnd, point);
	point->x = (WORD)point->x;
	point->y = (WORD)point->y;
}

void SetInput_MOUSE_LEFTDOWN()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));
	ZeroMemory(&input, sizeof(INPUT));
}

void MoveWindow2_LEFT_TOP()
{
	MoveWindow(pickerDlgHwnd, 0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, FALSE);
}

void MoveWindow2_RIGHT_BOTTOM()
{
	int x = GetSystemMetrics(SM_CXFULLSCREEN) - DIALOG_WIDTH;
	int y = GetSystemMetrics(SM_CYFULLSCREEN) - DIALOG_HEIGHT;
	MoveWindow(pickerDlgHwnd, x, y, DIALOG_WIDTH, DIALOG_HEIGHT, FALSE);
}

void TextOutInfo(POINT point)
{

	HDC hdc = GetDC(pickerDlgHwnd);
	char cursorPos[32];
	sprintf_s(cursorPos, "Position = %d X %d      ", (WORD)point.x, (WORD)point.y);
	TextOut(hdc, 5, 5, cursorPos, lstrlen(cursorPos));
	UpdateWindow(pickerDlgHwnd);
	ReleaseDC(pickerDlgHwnd, hdc);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
			if (GetCapture() != hwnd) {
				SetInput_MOUSE_LEFTDOWN();
			}
			else {
				POINT point;
				GetPoint(lParam, &point);
				RECT rect;
				GetWindowRect(hwnd, &rect);

				//Move Dlg if Mouseover
				if (PointAroundRect(&rect, point)) {
					if (rect.left == 0 && rect.top == 0)
						MoveWindow2_RIGHT_BOTTOM();
					else
						MoveWindow2_LEFT_TOP();
				}
				TextOutInfo(point);
				//Get UIAElement from Point , UIA Area
				IUIAutomation* automation = uiaObj.GetAutomation();
				IUIAutomationElement *element = NULL;
				HRESULT result = automation->ElementFromPoint(point, &element);
				if (FAILED(result) || element == NULL) {
					cout << "ElementFromPoint Failed" << endl;
					break;
				}
				RECT elementRect;
				element->get_CurrentBoundingRectangle(&elementRect);
				drawer.StartDraw(elementRect, penColor);
			}
			break;
		}
		case WM_LBUTTONDOWN: {
			//Warn:
			/*
				SetCapture can keep mouse message in even your mouse is out of window
				Howere,you must let your mouse down-drag
				Have a try :
				1.No SetCapture,but do leftdown-drag out of window,what've you seen?
				  no info print to window .It means no mouse message handled by window .
				2.SetCapture , move mouse out of window within leftdown.Is it the same as first case?
				  It means we need our mouse to do leftdown .
				3.SetCapture , do leftdown-drag out of window .It will run as we wish.

				Now ,can you guess if there any case not suits this method?
				I give you one:

				Find a comboBox control ,click it and make options list expand ,
				pick it .The list already collapse cause of the leftdown.

				It is not the perfect one .
			*/
			SetCapture(pickerDlgHwnd);
			break;
		}
		case WM_LBUTTONUP: {
			DestroyWindow(hwnd);
			drawer.EndDraw();
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			ReleaseCapture();
			PostQuitMessage(0);
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	return 0;
}


DWORD WINAPI DlgThreadProc(LPVOID lpParameter)
{
	WNDCLASS wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndcls.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndcls.hIcon = NULL;
	wndcls.hInstance = (HINSTANCE)pickerDlgHandle;
	wndcls.lpfnWndProc = WndProc;
	wndcls.lpszClassName = "PickerDlg";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndcls);

	//create dlg with config:top£¬out of switch list
	pickerDlgHwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "PickerDlg", "Picker", WS_THICKFRAME, 0, 0, 0, 0, NULL, NULL, (HINSTANCE)pickerDlgHandle, NULL);

	//show Window under mouse
	POINT cursorPoint;
	GetCursorPos(&cursorPoint);
	int x = cursorPoint.x - DIALOG_WIDTH / 2;
	int y = cursorPoint.y - DIALOG_HEIGHT / 2;
	SetWindowPos(pickerDlgHwnd, 0, x, y, DIALOG_WIDTH, DIALOG_HEIGHT, SWP_NOZORDER);
	ShowWindow(pickerDlgHwnd, SW_SHOWNORMAL);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
