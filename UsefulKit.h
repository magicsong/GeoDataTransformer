#pragma once
#include "stdafx.h"
class UsefulKit
{
public:
	UsefulKit();
	~UsefulKit();
	static string FindFileName(string path, bool hasExtention);
	template <typename T> void PrintArray(T* arrayToShow, int count, char sp=',')
	{
		for (int i = 0; i < count; i++)
		{
			cout.setf(ios::dec | ios::fixed);
			cout << arrayToShow[i] << sp;
		}
		cout << endl;
	}
};

