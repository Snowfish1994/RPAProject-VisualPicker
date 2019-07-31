#pragma once
#include <map>
#include <string>
#include <vector>
#include "uiautomation.h"
using namespace std;

class UIAObj {
private:
	IUIAutomation *_automation = NULL;
public:
	UIAObj();
	~UIAObj();
public:
	IUIAutomation *GetAutomation();
};
