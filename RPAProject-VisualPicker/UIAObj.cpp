#include <map>
#include <string>
#include <comdef.h>
#include "UIAObj.h"

UIAObj::UIAObj() {
	CoInitialize(NULL);
	//Warn:First Parameter value :
	//CUIAutomation supported minimum :Windows XP
	//CUIAutomation8 is over Windows8
	HRESULT hResult = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_automation));
	if (FAILED(hResult))
		throw string("Failed to create UIAutomation Object");
}

UIAObj::~UIAObj() {
	_automation->Release();
	CoUninitialize();
}

IUIAutomation * UIAObj::GetAutomation() {
	return _automation;
}

