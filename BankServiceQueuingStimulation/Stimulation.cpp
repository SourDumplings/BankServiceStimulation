#include "stdafx.h"
#include "Stimulation.h"

const int DAY_SECONDS = 3600 * 24;

Stimulation::Stimulation()
{
	_read_open_and_close_time();
	_read_bank_info();
	_read_windows_info();
	print_info();
	_read_customers_info();
}


Stimulation::~Stimulation()
{
}

void Stimulation::_read_open_and_close_time()
{
	string openTime, closeTime;
	while (true)
	{
		printf("When does the bank open?(eg. 08:00:00): ");
		cin >> openTime;
		_openTime = time_to_seconds(openTime);
		printf("When does the bank close?(eg. 21:00:00): ");
		cin >> closeTime;
		_closeTime = time_to_seconds(closeTime);
		if (_closeTime <= _openTime || _closeTime >= DAY_SECONDS ||
			openTime[2] != ':' || openTime[5] != ':' || closeTime[2] != ':' || closeTime[5] != ':')
			printf("Invalid open or close time!\n");
		else
			break;
	}
	return;
}

void Stimulation::_read_bank_info()
{
	printf("How many windows in the bank? ");
	read_integer(_windowNum, 0);
	printf("How many customers will come to the bank today? ");
	read_integer(_customerNum, 0);

	printf("Do you allow VIP service?(y/n): ");
	_VIP = read_order();
	if (_VIP)
	{
		printf("How many VIP windows in this bank today? ");
		read_integer(_VIPWindowNum, 0, _windowNum-1);
	}

	printf("Do you allow familiar cutting in?(y/n): ");
	_cutIn = read_order();

	printf("Does each window has its own queue(yellow line)?(y/n): ");
	_yellowLine = read_order();
	if (_yellowLine)
	{
		printf("How many customers can each queue inside yellow line have? ");
		read_integer(_yellowLineCapacity, 0);
	}

	printf("What is the maximum serving time for each customer(in minutes)?(0 means no limit): ");
	int maxServeTimeM;
	read_integer(maxServeTimeM, 0, (_closeTime - _openTime + 30) / 60);
	_maxServeTime = maxServeTimeM * 60;
	return;
}

bool read_order()
{
	char order;
	bool ret = false;
	while (true)
	{
		cin >> order;
		if (order == 'y' || order == 'Y')
		{
			ret = true;
			break;
		}
		else if (order == 'n' || order == 'N')
		{
			ret = false;
			break;
		}
		else
		{
			printf("Invalid order!\n");
			printf("Please input order again(y/n): ");
		}
	}
	
	return ret;
}

void read_integer(int &res, int minValue, int maxValue)
{
	// 莫名其妙不能这样用，gcc下都没问题
	//cin >> res;
	//while (cin.fail())
	//{
	//	printf("Invalid input! Please input an integer: ");
	//	cin.clear();
	//	cin.sync();
	//	cin >> res;
	//}

	string temp;
	cin >> temp;
	while (true)
	{
		try
		{
			res = stoi(temp);
			if (res < minValue || maxValue < res)
			{
				throw invalid_argument("");
			}
			break;
		}
		catch (const invalid_argument&)
		{
			printf("Invalid input! Please input an valid integer(%d~%d): ", minValue, maxValue);
			cin >> temp;
		}
	}
	return;
}

void Stimulation::_read_customers_info()
{
	printf("For %d customers, please input the information of each customer: \n", _customerNum);
	printf("***********************************************************\n");
	printf("The format is name(no more than 10 characters), arriving time, need time(in minutes) and VIP tag\n");
	printf("if VIP is allowed, 0 represent not VIP). They are separated by one space\n");
	printf("For example: Tom 08:20:00 60\n");
	printf("***********************************************************\n");
	for (int i = 0; i != _customerNum; ++i)
	{
		string name, arriveTimeS;
		int needTimeM;
		cin >> name >> arriveTimeS >> needTimeM;
		if (_VIP)
		{
			int v;
			read_integer(v, 0, 1);
			_customerData[name] = Customer(name, arriveTimeS, needTimeM * 60, v == 1);
			if (v == 1)
			{
				_VIPQ.push_back(name);
			}
		}
		else
		{
			_customerData[name] = Customer(name, arriveTimeS, needTimeM * 60);
		}

		if (_yellowLine)
		{
			if (i < _windowNum)
			{
				_windowData[i]._specialQ.push_back(name);
			}
			else if (i < _windowNum * _yellowLineCapacity)
			{
				_windowData[i % _windowNum]._specialQ.push_back(name);
			}
			else
				_Q.push_back(name);
		}
		else
			_Q.push_back(name);
	}
	if (_cutIn)
	{
		printf("Now input each customer's friends' names.\n");
		string friendName;
		for (auto &cp : _customerData)
		{
			printf("Input names of %s's friends(space separated, 0 means end).\n", cp.first.c_str());
			while (true)
			{
				cin >> friendName;
				if (friendName == "0")
				{
					break;;
				}
				cp.second._friends.insert(friendName);
			}
		}
	}
	printf("Customers information read successfully.\n");
	return;
}

void Stimulation::_read_windows_info()
{
	_windowData.resize(_windowNum);
	if (_VIP)
	{
		printf("Pleas input the seq number of VIP window?(from %d to %d)\n", 0, _windowNum-1);
		for (int i = 0; i != _VIPWindowNum; ++i)
		{
			int VIPW;
			read_integer(VIPW, 0, _windowNum-1);
			_windowData[VIPW]._isVIPWindow = true;
		}
	}
	if (_yellowLine)
	{
		for (auto &w : _windowData)
		{
			w._yellowLineCapacity = _yellowLineCapacity;
		}
	}
	return;
}

int Stimulation::_get_avail_window()
{
	int minRes = INT_MAX, availW = -1;
	for (int i = 0; i != _windowNum; ++i)
	{
		if (_windowData[i]._resTime < minRes)
		{
			availW = i;
			minRes = _windowData[i]._resTime;
		}
	}

	_nowTime += minRes;
	for (auto &w : _windowData)
	{
		w._resTime -= minRes;
	}

	if (_VIP && _has_VIP_in_Q())
	{
		for (int i = 0; i != _windowNum; ++i)
		{
			if (_windowData[i]._isVIPWindow && _windowData[i]._resTime == 0)
			{
				availW = i;
				break;
			}
		}
	}

	return availW;
}

bool Stimulation::_has_VIP_in_Q()
{
	bool ret = false;
	if (_VIP)
	{
		while (_customerData[_VIPQ.front()]._served)
		{
			_VIPQ.pop_front();
		}
		Customer &frontVIPC = _customerData[_VIPQ.front()];
		if (frontVIPC._arriveTime <= _nowTime)
		{
			ret = true;
		}
	}
	return ret;
}

void Stimulation::exec()
{
	printf("Stimulation begins.\n");
	_/*nowTime = 0;

	while (true)
	{
		int availW = _get_avail_window();
		if (_VIP)
		{
			while (_customerData[_Q.front()]._served)
			{
				_Q.pop_front();
			}
		}

		string nextC = _Q.front();

	}*/
	return;
}

void Stimulation::print_info() const
{
	printf("---------------------------------------------------------------\n");
	printf("The bank opens at %s, and closes at %s.\n", seconds_to_time(_openTime).c_str(),
		seconds_to_time(_closeTime).c_str());
	if (_maxServeTime != 0)
	{
		printf("Each window can only serve for each customer for %d minutes.\n", _maxServeTime / 60);
	}
	printf("There are %d windows in the bank.\n", _windowNum);
	if (_VIP)
	{
		printf("There is VIP service in this bank\n");
		printf("The bank has %d VIP windows, which are:\n", _VIPWindowNum);
		int output = 0;
		for (int i = 0; i != _windowNum; ++i)
		{
			if (_windowData[i]._isVIPWindow)
			{
				if (output++)
					putchar(' ');
				printf("%d", i);
			}
		}
		putchar('\n');
	}
	if (_yellowLine)
	{
		printf("This bank has yellow line in front of each window.\n");
		printf("Each queue inside the yellow line can only have %d customers.\n", _yellowLineCapacity);
	}
	if (_cutIn)
	{
		printf("There will be \"cut in by friends\" in this simulation\n");
	}
	printf("There will be %d customers in this simulation.\n", _customerNum);
	printf("---------------------------------------------------------------\n");
	return;
}
