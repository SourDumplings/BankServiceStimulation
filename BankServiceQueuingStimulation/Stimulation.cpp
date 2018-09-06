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

	int caseNo;
	printf("Please choose the case of the simulation:\n");
	printf("***********************************************************\n");
	printf("	case 0 : no special case\n");
	printf("	case 1 : the bank has VIP service\n");
	printf("	case 2 : the bank has yellow line\n");
	printf("	case 3 : \"friends cut in\" exists\n");
	printf("***********************************************************\n");
	printf("I choose case ");
	read_integer(caseNo, 0, 3);
	switch (caseNo)
	{
	case 0 :
	{
		_VIP = _yellowLine = _cutIn = false;
		break;
	}
	case 1 :
	{
		printf("How many VIP windows in this bank today? ");
		read_integer(_VIPWindowNum, 0, _windowNum);
		_VIP = true;
		break;
	}
	case 2 :
	{
		printf("How many customers can each queue inside yellow line have? ");
		read_integer(_yellowLineCapacity, 1);
		_yellowLine = true;
		break;
	}
	case 3 :
	{
		_cutIn = true;
		break;
	}
	}

	printf("What is the maximum serving time for each customer(in minutes)?(0 means no limit): ");
	int maxServeTimeM;
	read_integer(maxServeTimeM, 0, (_closeTime - _openTime + 30) / 60);
	_maxServeTime = maxServeTimeM * 60;
	return;
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
	printf("The format is name(no more than 10 characters), arriving time, need time(in minutes) and VIP tag");
	printf("(if VIP is allowed, 0 represent not VIP). \nThey are separated by one space\n");
	printf("For example: Tom 08:20:00 60 1\n");
	printf("***********************************************************\n");
	_customerData.resize(_customerNum);
	for (int i = 0; i != _customerNum; ++i)
	{
		string name, arriveTimeS;
		int needTimeM;
		cin >> name >> arriveTimeS >> needTimeM;
		if (_VIP)
		{
			int v;
			read_integer(v, 0, 1);
			_customerData[i] = Customer(name, arriveTimeS, needTimeM * 60, v == 1);
		}
		else
		{
			_customerData[i] = Customer(name, arriveTimeS, needTimeM * 60);
		}
	}

	if (_cutIn)
	{
		printf("Now input each customer's friends' names.\n");
		string friendName;
		for (auto &c : _customerData)
		{
			printf("Input names of %s's friends(space separated, 0 means end).\n", c._name.c_str());
			while (true)
			{
				cin >> friendName;
				if (friendName == "0")
				{
					break;;
				}
				c._friends.insert(friendName);
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
	int minRes = INT_MAX, availW = 0;
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
		while (!_VIPQ.empty() && _customerData[_VIPQ.front()]._served)
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

void Stimulation::_exec_VIP()
{
	while (true)
	{
		while (!_Q.empty() && _customerData[_Q.front()]._served)
		{
			_Q.pop_front();
		}
		if (_Q.empty())
		{
			break;
		}
		int availW = _get_avail_window();
		int nextC = _Q.front();
		if (_nowTime < _customerData[nextC]._arriveTime)
		{
			for (int i = 0; i != _windowNum; ++i)
			{
				_windowData[i]._resTime -= _customerData[nextC]._arriveTime - _nowTime;
				if (_windowData[i]._resTime < 0)
				{
					_windowData[i]._resTime = 0;
				}
			}
			_nowTime = _customerData[nextC]._arriveTime;
			availW = _get_avail_window();
		}
		if (_closeTime < _customerData[nextC]._arriveTime)
		{
			break;
		}

		if (_windowData[availW]._isVIPWindow && _has_VIP_in_Q())
		{
			nextC = _VIPQ.front();
			_VIPQ.pop_front();
		}
		else
			_Q.pop_front();

		_customerData[nextC]._served = true;
		if (_maxServeTime != 0 && _maxServeTime < _customerData[nextC]._needTime)
		{
			_windowData[availW]._resTime = _maxServeTime;
		}
		else
			_windowData[availW]._resTime = _customerData[nextC]._needTime;

		_customerData[nextC]._serveTime = _nowTime;
		++_windowData[availW]._servedNum;
		_windowData[availW]._servedSeq.push_back(nextC);
	}
	return;
}

int Stimulation::_get_avail_non_empty_window()
{
	int availW = -1, minRes = INT_MAX;
	for (int i = 0; i != _windowNum; ++i)
	{
		if (!_windowData[i]._specialQ.empty() && _windowData[i]._resTime < minRes)
		{
			availW = i;
			minRes = _windowData[i]._resTime;
		}
	}
	if (availW != -1)
	{
		_nowTime += minRes;
		for (auto &w : _windowData)
		{
			w._resTime -= minRes;
			if (w._resTime < 0)
			{
				w._resTime = 0;
			}
		}
	}
	else
	{
		int nextCArriveT = _customerData[_Q.front()]._arriveTime;
		for (auto &w : _windowData)
		{
			w._resTime -= nextCArriveT - _nowTime;
			if (w._resTime < 0)
			{
				 w._resTime = 0;
			}
		}
		_nowTime = nextCArriveT;
	}
	return availW;
}

void Stimulation::_exec_yellowLine()
{
	int servedNum = 0;
	if (_customerData[_windowData[0]._specialQ.front()]._arriveTime > _nowTime)
	{
		_nowTime = _customerData[_windowData[0]._specialQ.front()]._arriveTime;
	}
	while (servedNum < _customerNum)
	{
		int availW = _get_avail_window();
		if (_windowData[availW]._specialQ.empty())
		{
			availW = _get_avail_non_empty_window();
		}
		if (availW == -1)
		{
			// 所有窗口的黄线以内队列全空
			if (_Q.empty())
			{
				break;
			}
			else
			{
				// 此时一定是在黄线外的顾客来得很晚
				availW = _get_avail_window();
				_windowData[availW]._specialQ.push_back(_Q.front());
				_Q.pop_front();
			}
		}
		int nextC = _windowData[availW]._specialQ.front();
		_windowData[availW]._specialQ.pop_front();
		if (_closeTime < _customerData[nextC]._arriveTime)
		{
			break;
		}
		if (_nowTime < _customerData[nextC]._arriveTime)
		{
			for (auto &w : _windowData)
			{
				w._resTime -= _customerData[nextC]._arriveTime - _nowTime;
				if (w._resTime < 0)
				{
					w._resTime = 0;
				}
			}
			_nowTime = _customerData[nextC]._arriveTime;
		}

		_customerData[nextC]._served = true;
		if (_maxServeTime != 0 && _maxServeTime < _customerData[nextC]._needTime)
		{
			_windowData[availW]._resTime = _maxServeTime;
		}
		else
			_windowData[availW]._resTime = _customerData[nextC]._needTime;

		_customerData[nextC]._serveTime = _nowTime;
		++_windowData[availW]._servedNum;
		_windowData[availW]._servedSeq.push_back(nextC);
		++servedNum;

		if (!_Q.empty() && _customerData[_Q.front()]._arriveTime < _nowTime)
		{
			// 如果黄线外此时有顾客已经来了，则进入黄线内
			_windowData[availW]._specialQ.push_back(_Q.front());
			_Q.pop_front();
		}
	}
	return;
}

void Stimulation::_exec_cutIn()
{

}

void Stimulation::_cut_in(int newCustomerNo)
{
	bool cutInSuccess = false;
	for (auto it = _Q.begin(); it != _Q.end(); ++it)
	{
		const string &newCustomerName = _customerData[newCustomerNo]._name, &friendName = _customerData[*it]._name;
		if (_customerData[newCustomerNo]._friends.find(friendName) != _customerData[newCustomerNo]._friends.end() &&
			_customerData[*it]._friends.find(newCustomerName) != _customerData[*it]._friends.end())
		{
			_Q.insert(it + 1, newCustomerNo);
			cutInSuccess = true;
			break;
		}
	}
	if (!cutInSuccess)
	{
		_Q.push_back(newCustomerNo);
	}
	return;
}

void Stimulation::_pre_treatment()
{
	_nowTime = _openTime;

	sort(_customerData.begin(), _customerData.end(), [] (const Customer &c1, const Customer &c2)
	{ return c1._arriveTime < c2._arriveTime; });

	int yellowC = _windowNum * _yellowLineCapacity;
	
	for (int i = 0; i != _customerNum; ++i)
	{
		if (_yellowLine)
		{
			if (i < _windowNum)
			{
				_windowData[i]._specialQ.push_back(i);
			}
			else if (i < yellowC)
			{
				_windowData[i % _windowNum]._specialQ.push_back(i);
			}
			else
			{
				_Q.push_back(i);
			}
		}
		else if (_cutIn)
		{
			_cut_in(i);
		}
		else
		{
			_Q.push_back(i);
			if (_VIP && _customerData[i]._isVIP)
			{
				_VIPQ.push_back(i);
			}
		}
	}
	return;
}


void Stimulation::_serving_info_of_customers()
{
	printf("For all %d customers:\n", _customerNum);
	printf("***********************************************************\n");
	printf("The sequence of customers' serving information is as follow:\n");
	printf("Customer's name, arriving time, serving time, waiting minutes\n");
	printf("and VIP tag(if VIP service exists) are in a line\n");
	printf("They are sorted by serving time.\n");
	printf("For example: Bob 08:00:05 08:10:00 10 VIP\n");
	printf("If a customer is not served(arrived too late), his serving time and waiting minutes will be -\n");
	printf("***********************************************************\n");
	
	double totalWaitTime = 0;
	int servedNum = 0;

	vector<Customer*> servingSeq(_customerNum);
	for (int i = 0; i != _customerNum; ++i)
	{
		servingSeq[i] = &(_customerData[i]);
	}
	sort(servingSeq.begin(), servingSeq.end(), [](Customer * const pc1, Customer * const pc2)
	{
		if (pc1->_served && pc2->_served)
		{
			return pc1->_serveTime < pc2->_serveTime;
		}
		else
			return pc1->_served;
	});

	for (Customer *pc: servingSeq)
	{
		cout << pc->_name << "	" << seconds_to_time(pc->_arriveTime) << "	";
		if (pc->_served)
		{
			cout << seconds_to_time(pc->_serveTime) << "	" << (pc->_serveTime - pc->_arriveTime + 30) / 60;
			totalWaitTime += pc->_serveTime - pc->_arriveTime;
			++servedNum;
		}
		else
			cout << "   -" << "   -";

		if (_VIP)
		{
			cout << "	" << (pc->_isVIP ? "VIP" : "not VIP") << endl;
		}
		else
		{
			cout << endl;
		}
	}
	printf("The average waiting time is %.2f\n", totalWaitTime / (60 * servedNum));
	return;
}

void Stimulation::_serving_info_of_windows()
{
	printf("For all %d windows:\n", _windowNum);
	printf("***********************************************************\n");
	printf("The sequence of windows' serving information is as follow:\n");
	printf("Window's number, VIP tag(if VIP service exists), served number, served sequence\n");
	printf("For example: 0 VIP 3 Bob Jim Alice\n");
	printf("If a windows served no one, its serving sequence will be -\n");
	printf("***********************************************************\n");

	for (int i = 0; i != _windowNum; ++i)
	{
		cout << i << "	";
		if (_VIP)
		{
			cout << (_windowData[i]._isVIPWindow ? "VIP" : "ORD") << "	";
		}
		cout << _windowData[i]._servedNum << "	";

		if (_windowData[i]._servedNum == 0)
		{
			cout << "-" << endl;
		}
		else
		{
			int output = 0;
			for (int cNo : _windowData[i]._servedSeq)
			{
				if (output++)
				{
					cout << " ";
				}
				cout << _customerData[cNo]._name;
			}
			cout << endl;
		}
	}
	return;
}

void Stimulation::_show_results()
{
	printf("Stimulation successes!\n");
	while (true)
	{
		printf("Which kind of results you want to see?\n");
		printf("***********************************************************\n");
		printf("	0: exit\n");
		printf("	1: serving sequence of customers\n");
		printf("	2: serving information of each window\n");
		printf("***********************************************************\n");
		printf("I choose ");
		int order;
		read_integer(order, 0, 2);
		switch (order)
		{
		case 0 : break;
		case 1 : _serving_info_of_customers(); break;
		case 2 : _serving_info_of_windows(); break;
		}
		if (order == 0)
		{
			printf("Simulation Ends\n");
			break;
		}
	}
	return;
}

void Stimulation::exec()
{
	printf("Stimulation begins.\n");
	_pre_treatment();

	if (_VIP)
	{
		_exec_VIP();
	}
	else if (_yellowLine)
	{
		_exec_yellowLine();
	}
	else if (_cutIn)
	{
		_exec_cutIn();
	}
	else
	{
		while (!_Q.empty())
		{
			int availW = _get_avail_window();
			int nextC = _Q.front();
			_Q.pop_front();
			if (_closeTime < _customerData[nextC]._arriveTime)
			{
				break;
			}
			if (_nowTime < _customerData[nextC]._arriveTime)
			{
				for (int i = 0; i != _windowNum; ++i)
				{
					_windowData[i]._resTime -= _customerData[nextC]._arriveTime - _nowTime;
					if (_windowData[i]._resTime < 0)
					{
						_windowData[i]._resTime = 0;
					}
				}
				_nowTime = _customerData[nextC]._arriveTime;
				availW = _get_avail_window();
			}

			_customerData[nextC]._served = true;
			if (_maxServeTime != 0 && _maxServeTime < _customerData[nextC]._needTime)
			{
				_windowData[availW]._resTime = _maxServeTime;
			}
			else
				_windowData[availW]._resTime = _customerData[nextC]._needTime;

			_customerData[nextC]._serveTime = _nowTime;
			++_windowData[availW]._servedNum;
			_windowData[availW]._servedSeq.push_back(nextC);
		}
	}

	_show_results();
	
	return;
}

void Stimulation::print_info() const
{
	printf("---------------------------------------------------------------\n");
	printf("The bank opens at %s, and closes at %s.\n", seconds_to_time(_openTime).c_str(),
		seconds_to_time(_closeTime).c_str());
	if (_maxServeTime != 0)
	{
		printf("Each window can only serve for each customer for %d minutes at most.\n", _maxServeTime / 60);
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
