// BankServiceQueuingStimulation.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int main()
{
	int stimulationNum;
	printf("Welcome to Bank Service Queuing Stimulation!\n");
	printf("-------------版权所有：酸饺子-------------\n");

	printf("How many stimulations you want to run?: ");
	read_integer(stimulationNum, 0);

	while (stimulationNum--)
	{
		Stimulation S;
		S.exec();
	}
    return 0;
}

