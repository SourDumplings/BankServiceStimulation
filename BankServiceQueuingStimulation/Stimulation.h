#pragma once

#include <string>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

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
	bool _cutIn = false, _VIP = false, _yellowLine = false;
	int _windowNum, _customerNum, _VIPWindowNum, _yellowLineCapacity;
	int _openTime, _closeTime, _nowTime, _maxServeTime;
	deque<int> _Q, _VIPQ;
	vector<Window> _windowData;
	vector<Customer> _customerData;

	void _read_open_and_close_time();
	void _read_bank_info();
	void _read_customers_info();
	void _read_windows_info();

	int _get_avail_window();
	int _get_avail_non_empty_window();
	bool _has_VIP_in_Q();
	void _pre_treatment();
	void _cut_in(int newCustomerNo);

	void _exec_VIP();
	void _exec_yellowLine();
	void _exec_cutIn();

	void _show_results();
	void _serving_info_of_customers();
	void _serving_info_of_windows();
};

void read_integer(int &res, int minValue, int maxValue = INT_MAX);

