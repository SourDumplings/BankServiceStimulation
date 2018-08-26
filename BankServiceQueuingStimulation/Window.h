#pragma once

#include <deque>
#include <vector>

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
	vector<int> _servedSeq;
	deque<int> _specialQ;
};

