#pragma once

#include <deque>

class Stimulation;
class Customer;

class Window
{
	friend class Stimulation;
	friend class Customer;
public:
	Window();
	~Window();
private:
	int _resTime = 0, _servedNum = 0, _yellowLineCapacity = 0;
	bool _isVIPWindow = false;
	deque<string> _specialQ;
};

