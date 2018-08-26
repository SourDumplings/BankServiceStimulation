#include "stdafx.h"
#include "Customer.h"
#include <stdexcept>

int time_to_seconds(const string &timeS)
{
	if (timeS.length() < 8)
	{
		printf("Error: time format is wrong!\n");
		throw std::exception();
	}

	string tempTimeS(timeS.substr(0, 8));
	int h = (tempTimeS[0] - '0') * 10 + tempTimeS[1] - '0';
	int m = (tempTimeS[3] - '0') * 10 + tempTimeS[4] - '0';
	int s = (tempTimeS[6] - '0') * 10 + tempTimeS[7] - '0';
	return h * 3600 + m * 60 + s;
}

string seconds_to_time(int seconds)
{
	int h = seconds / 3600;
	int s = seconds % 60;
	int m = (seconds - h * 3600 - s) / 60;
	char tempS[11];
	sprintf_s(tempS, "%02d:%02d:%02d", h, m, s);
	return string(tempS);
}

Customer::Customer() {}

Customer::Customer(const string &name_, const string &arriveTimeS_, int needTime_, bool isVIP_)
{
	_name = name_;
	_arriveTime = time_to_seconds(arriveTimeS_);
	_needTime = needTime_;
	_isVIP = isVIP_;
}


Customer::~Customer()
{
}

inline void Customer::add_friends(const string &friendName)
{
	_friends.insert(friendName);
	return;
}

void Customer::print_info() const
{
	printf("Customer %s%s: \n", _name.c_str(), _isVIP ? "(VIP)" : "");
	printf("He arrived at %s, %s.\n", seconds_to_time(_arriveTime).c_str(),
		_served ? seconds_to_time(_serveTime).c_str() : "not be served");
	if (_served)
	{
		printf("He waited for %d minutes.\n", (_serveTime - _arriveTime + 30) / 60);
	}

	if (_friends.empty())
	{
		printf("He doesn't have friends.\n");
	}
	else
	{
		printf("He has friends:");
		for (auto &f : _friends)
		{
			printf(" %s", f.c_str());
		}
	}
	printf("\n\n");
	return;
}

void Customer::get_serve(const string &serveTimeS_)
{
	_served = true;
	_serveTime = time_to_seconds(serveTimeS_);
	return;
}

bool operator<(const Customer &c1, const Customer &c2)
{
	return c1._arriveTime < c2._arriveTime;
}
