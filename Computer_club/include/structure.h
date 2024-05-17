#ifndef _TABLE_
#define _TABLE_
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <list>

using namespace std;


struct TableInfo
{
	bool isFree;
	string clientName;
	tm startTime;
	tm totalTime;

	TableInfo()
	{
		isFree = true;
		clientName = "";
		startTime = {};
		totalTime = {};
	}
};

bool compareEventTime(const tm& eventTime, const tm& startTime);
bool isValidFile(const string& filename);
int FindTableByClient(string& clientName, map<int, TableInfo> tableInfo);
int getSecondsFromMidnight(const tm& time);
int calculateRevenueForTable(tm time, int hourPrice);
void DiffTime(const tm& eventTime, const tm& time, int& table, map<int, TableInfo>& tableInfo);
void ClearQueue(list<string>& guestsQueue, list<string>& guestsList, const tm& eventTime,
	map<int, TableInfo>& tableInfo, int& occupiedTable, int& table);
void ClearTable(const string& clientName, const tm& eventTime, int& occupiedTable, map< int, TableInfo>& tableInfo, int& table);

#endif