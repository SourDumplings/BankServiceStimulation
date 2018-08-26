#pragma once

#include <string>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <stdexcept>
#include <cstdint>

#include "Customer.h"
#include "Window.h"

using namespace std;

class Stimulation
{
public:
	Stimulation();
	~Stimulation();

	void exec();
	void print_info() const;
private:
	bool _cutIn, _VIP, _yellowLine;
	int _windowNum, _customerNum, _VIPWindowNum, _yellowLineCapacity;
	int _openTime, _closeTime, _nowTime, _maxServeTime;
	deque<string> _Q, _VIPQ;
	map<string, Customer> _customerData;
	vector<Window> _windowData;

	void _read_open_and_close_time();
	void _read_bank_info();
	void _read_customers_info();
	void _read_windows_info();
	int _get_avail_window();
	bool _has_VIP_in_Q();
};

bool read_order();
void read_integer(int &res, int minValue, int maxValue = INT_MAX);

