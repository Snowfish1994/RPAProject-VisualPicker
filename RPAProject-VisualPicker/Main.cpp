#include "HookCase.h"
#include "SetCaptureCase.h"
#include "TimerCase.h"
#include "UIADrawerLibrary.h"
#include <iostream>
#include <Windows.h>

using namespace std;

void main() {
	/*
	SetCaptureCase scCase = SetCaptureCase();
	scCase.OpenPickerDlg();*/
	HookCase hCase = HookCase();
	hCase.StartPick();
	//no exit
	while (1) {
		Sleep(1000);
	}
}