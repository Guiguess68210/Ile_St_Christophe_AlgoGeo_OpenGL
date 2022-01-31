#pragma once
#ifndef ENVELOPPE_H
#define ENVELOPPE_H

#include "point.h"
#include <vector>

using namespace std;

void sort(vector<Point>& arr, int sz)
{
	for (int i = 1; i < sz; i++)
	{
		for (int j = 0; j < sz - i; j++)
		{
			if (arr[j].x() == arr[j + 1].x() && arr[j].y() == arr[j + 1].y())
			{
				arr.erase(arr.begin() + j);
			}
			if (arr[j].x() > arr[j + 1].x())
			{
				Point temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
			else if (arr[j].x() == arr[j + 1].x())
			{
				if (arr[j].y() < arr[j + 1].y())
				{
					Point temp = arr[j];
					arr[j] = arr[j + 1];
					arr[j + 1] = temp;
				}
			}
		}
	}
}
#endif