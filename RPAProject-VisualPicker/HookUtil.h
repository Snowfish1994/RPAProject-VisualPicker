#pragma once
#include"header.h"
class HookUtil
{
private :
	bool msgLooping;
	HHOOK _mouseHook, _keyboardHook;
public:
	HookUtil();
	~HookUtil();

public:
	void MsgLoop();
	void StartHook(HOOKPROC mouseProc, HOOKPROC keyProc);
	void StopHook();

};

