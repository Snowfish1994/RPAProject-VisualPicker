#include "HookCase.h"
#include "HookUtil.h"
#include "UIADrawerLibrary.h"
#include "UIAObj.h";

HookUtil hookUtil = HookUtil();

HWND pickerDlgHwnd_hook;
HANDLE pickerDlgHandle_hook;
int DIALOG_WIDTH_hook = 160;
int DIALOG_HEIGHT_hook = 200;
COLORREF penColor_hook = 0xf0f;
UIADrawerLibrary drawer_hook = UIADrawerLibrary();
UIAObj uiaObj_hook = UIAObj();

/**Function Defs**/
DWORD WINAPI DlgThreadProc_hook(LPVOID lpParameter);
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
/**Defs End**/


HookCase::HookCase()
{
}


HookCase::~HookCase()
{
}

void HookCase::StartPick()
{
	pickerDlgHandle_hook = GetModuleHandle(NULL);
	HANDLE thread = CreateThread(NULL, 0, DlgThreadProc_hook, NULL, 0, NULL);
}

void HookCase::EndPick()
{
	hookUtil.StopHook();
	DestroyWindow(pickerDlgHwnd_hook);
	drawer_hook.EndDraw();
	drawer_hook.~UIADrawerLibrary();
}

BOOL PointAroundRect_hook(CONST RECT *lprc, POINT pt)
{
	RECT rect;
	rect.top = lprc->top - 5;
	rect.left = lprc->left - 5;
	rect.bottom = lprc->bottom + 5;
	rect.right = lprc->right + 5;
	return PtInRect(&rect, pt);
}

void TextOutInfo_hookcase(POINT point)
{

	HDC hdc = GetDC(pickerDlgHwnd_hook);
	char cursorPos[32];
	sprintf_s(cursorPos, "Position = %d X %d      ", (WORD)point.x, (WORD)point.y);
	TextOut(hdc, 5, 5, cursorPos, lstrlen(cursorPos));
	UpdateWindow(pickerDlgHwnd_hook);
	ReleaseDC(pickerDlgHwnd_hook, hdc);
}

LRESULT CALLBACK WndProc_hook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		{
	
				POINT point;
				point.x = (WORD)LOWORD(lParam);
				point.y = (WORD)HIWORD(lParam);

				ClientToScreen(pickerDlgHwnd_hook, &point);
				point.x = (WORD)point.x;
				point.y = (WORD)point.y;
				RECT rect;
				GetWindowRect(hwnd, &rect);

				//Move Dlg if Mouseover
				if (PointAroundRect_hook(&rect, point)) {
					if (rect.left == 0 && rect.top == 0) {
						int x = GetSystemMetrics(SM_CXFULLSCREEN) - DIALOG_WIDTH_hook;
						int y = GetSystemMetrics(SM_CYFULLSCREEN) - DIALOG_HEIGHT_hook;
						MoveWindow(pickerDlgHwnd_hook, x, y, DIALOG_WIDTH_hook, DIALOG_HEIGHT_hook, FALSE);
					}
					else
						MoveWindow(pickerDlgHwnd_hook, 0, 0, DIALOG_WIDTH_hook, DIALOG_HEIGHT_hook, FALSE);
				}
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
			drawer_hook.EndDraw();
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


DWORD WINAPI DlgThreadProc_hook(LPVOID lpParameter)
{
	WNDCLASS wndcls;
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wndcls.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndcls.hIcon = NULL;
	wndcls.hInstance = (HINSTANCE)pickerDlgHandle_hook;
	wndcls.lpfnWndProc = WndProc_hook;
	wndcls.lpszClassName = "PickerDlg";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wndcls);

	//create dlg with config:top£¬out of switch list
	pickerDlgHwnd_hook = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "PickerDlg", "Picker", WS_THICKFRAME, 0, 0, 0, 0, NULL, NULL, (HINSTANCE)pickerDlgHandle_hook, NULL);

	int x = GetSystemMetrics(SM_CXFULLSCREEN) - DIALOG_WIDTH_hook;
	int y = GetSystemMetrics(SM_CYFULLSCREEN) - DIALOG_HEIGHT_hook;
	SetWindowPos(pickerDlgHwnd_hook, 0, x, y, DIALOG_WIDTH_hook, DIALOG_HEIGHT_hook, SWP_NOZORDER);
	ShowWindow(pickerDlgHwnd_hook, SW_SHOWNORMAL);
	hookUtil.StartHook(&mouseProc, &keyboardProc);
	return 0;
}

HHOOK _mouseHook, _keyboardHook;
DWORD preMouseTimeStamp = 0;
DWORD preKeyTimeStamp = 0;

LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		LPMOUSEHOOKSTRUCT mStruct = (LPMOUSEHOOKSTRUCT)lParam;
		if (wParam == WM_MOUSEMOVE) {
			
			DWORD crt = GetTickCount();
			cout << crt - preMouseTimeStamp << endl;
			/*
				Notice: 
				At there , I do a if-else .
				With mouse hook method, we can do the same as SetCapture.
				Differently, when the prog is running into this proc,
				it will block next mouse input until current one have done;
				Have a think ,if it will cost one second in mouseProc , what a terrible experience!
				In this case you can feel it definitly .
				This way is unfriendly to user .
			*/
			if (crt - preMouseTimeStamp > 300) {
				preMouseTimeStamp = crt;
				POINT point = mStruct->pt;
				RECT rect;
				GetWindowRect(pickerDlgHwnd_hook, &rect);

				if (PointAroundRect_hook(&rect, point)) {
					if (rect.left == 0 && rect.top == 0) {
						int x = GetSystemMetrics(SM_CXFULLSCREEN) - DIALOG_WIDTH_hook;
						int y = GetSystemMetrics(SM_CYFULLSCREEN) - DIALOG_HEIGHT_hook;
						MoveWindow(pickerDlgHwnd_hook, x, y, DIALOG_WIDTH_hook, DIALOG_HEIGHT_hook, FALSE);
					}
					else
						MoveWindow(pickerDlgHwnd_hook, 0, 0, DIALOG_WIDTH_hook, DIALOG_HEIGHT_hook, FALSE);
				}

				//Get UIAElement from Point , UIA Area
				IUIAutomation* automation = uiaObj_hook.GetAutomation();
				IUIAutomationElement *element = NULL;
				HRESULT result = automation->ElementFromPoint(point, &element);
				if (FAILED(result) || element == NULL) {
					cout << "ElementFromPoint Failed" << endl;
					return TRUE;
				}
				RECT elementRect;
				element->get_CurrentBoundingRectangle(&elementRect);

				drawer_hook.StartDraw(elementRect, penColor_hook);
				TextOutInfo_hookcase(point);
			}
		}
		else if (wParam == WM_LBUTTONDOWN)
			return TRUE;
		else if (wParam == WM_LBUTTONUP) {
			drawer_hook.EndDraw();
			hookUtil.StopHook();
			DestroyWindow(pickerDlgHwnd_hook);
			return TRUE;
		}
	}
	return CallNextHookEx(_mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION && wParam == WM_KEYUP) {
		LPKBDLLHOOKSTRUCT kStruct = (LPKBDLLHOOKSTRUCT)lParam;
		if (kStruct->vkCode == VK_ESCAPE) {
			hookUtil.StopHook();
			DestroyWindow(pickerDlgHwnd_hook);
			return TRUE;
		}
	}
	return CallNextHookEx(_keyboardHook, nCode, wParam, lParam);
}
