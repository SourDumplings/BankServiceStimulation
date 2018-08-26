#pragma once

#include <set>
#include <string>

using namespace std;

class Stimulation;
class Window;

class Customer
{
	friend class Stimulation;
	friend class Window;
	friend bool operator<(const Customer &c1, const Customer &c2);
public:
	Customer();
	Customer(const string &name_, const string &arriveTimeS_, int needTime_, bool isVIP_ = false);
	~Customer();

	void print_info() const;
private:
	string _name;
	int _arriveTime, _serveTime;
	int _needTime;
	bool _isVIP, _served;
	set<string> _friends;

	void add_friends(const string &friendName);
	void get_serve(const string &serveTimeS_);

};

int time_to_seconds(const string &timeS);
string seconds_to_time(int seconds);

bool operator<(const Customer &c1, const Customer &c2);

