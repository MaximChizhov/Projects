#include "structure.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <list>

using namespace std;

bool compareEventTime(const tm& eventTime, const tm& startTime)
{
    if (eventTime.tm_hour < startTime.tm_hour)
        return false;
    else if (eventTime.tm_hour > startTime.tm_hour)
        return true;
    else
    {
        if (eventTime.tm_min < startTime.tm_min)
            return false;
        else if (eventTime.tm_min > startTime.tm_min)
            return true;
        else
            return true;
    }
}

bool isValidFile(const string& filename)
{
    ifstream file(filename);

    int lineCount = 0;
    // Проверка, удалось ли открыть файл
    if (!file.is_open())
    {
        cout << "File open error" << endl;
        return 1;
    }
    int num;
    string numStr;
    string hoursStr;
    string priceStr;

    getline(file, numStr);
    lineCount++;
    if (numStr.empty())
    {
        cout << "Empty line found in the file at line " << lineCount << endl;
        return false;
    }
    istringstream(numStr) >> num;
    for (char c : numStr) {
        if (!isdigit(c)) {
            cout << "Invalid character in number of tables in line " << lineCount << endl;
            return false;
        }
        if (numStr[0] == '0') {
            cout << "Number of tables should not start with 0 in line " << lineCount << endl;
            return false;
        }
    }

    tm stime = {};
    tm etime = {};
    getline(file, hoursStr);
    lineCount++;
    if (hoursStr.empty())
    {
        cout << "Empty line found in the file at line " << lineCount << endl;
        return false;
    }
    stringstream ss(hoursStr);
    if (!(ss >> get_time(&stime, "%H:%M")) || !(ss >> get_time(&etime, "%H:%M")))
    {
        cout << "Error reading times in line " << lineCount << endl;
        return false;
    }
    if (!compareEventTime(etime, stime))
    {
        cout << "Error reading times in line " << lineCount << endl;
        return false;
    }

    getline(file, priceStr);
    lineCount++;
    if (priceStr.empty())
    {
        cout << "Empty line found in the file at line " << lineCount << endl;
        return false;
    }
    for (char c : priceStr) {
        if (!isdigit(c)) {
            cout << "Invalid character in number of tables in line " << lineCount << endl;
            return false;
        }
        if (priceStr[0] == '0') {
            cout << "Number of tables should not start with 0 in line " << lineCount << endl;
            return false;
        }
    }
    tm prevTime = {};
    string line;
    while (getline(file, line))
    {
        lineCount++;
        tm eventTime = {};
        int tableNumber = 0;
        int eventNumber = 0;
        string clientName;
        stringstream ss(line);
        if (line.empty())
        {
            cout << "Empty line found in the file at line " << lineCount << endl;
            return false;
        }
        if (!(ss >> get_time(&eventTime, "%H:%M")))
        {
            cout << "Error reading event time in line " << lineCount << endl;
            return false;
        }
        if (compareEventTime(eventTime, prevTime))
            prevTime = eventTime;
        else
        {
            int prevLine = lineCount - 1;
            cout << "temporary events are not ordered. Error in lines: " << lineCount << " " << prevLine << endl;
            return false;
        }
        if (!(ss >> eventNumber))
        {
            cout << "Error reading event number in line " << lineCount << endl;
            return false;
        }
        if (eventNumber > 4 || eventNumber < 0)
        {
            cout << "Error reading event number in line " << lineCount << endl;
            return false;
        }
        if (!(ss >> clientName))
        {
            cout << "Error reading client name in line" << lineCount << endl;
            return false;
        }
        string lastWord;
        if (ss >> lastWord)
        {
            if (eventNumber != 2)
            {
                // Если номер события не равен 2, нет необходимости обрабатывать дополнительные цифры после имени клиента
                if (!lastWord.empty()) {
                    cout << "Unexpected digit after client name in line " << lineCount << endl;
                    return false;
                }
            }
            else
            {
                // Если номер события равен 2, нужно проверить наличие и корректность цифры (номера стола)
                if (lastWord.empty())
                {
                    cout << "Table number missing in line " << lineCount << endl;
                    return false;
                }
                else {
                    stringstream tableNumberStream(lastWord);
                    {
                        if (!(tableNumberStream >> tableNumber) || tableNumber > num || tableNumber <= 0)
                        {
                            cout << "Invalid table number in line " << lineCount << endl;
                            return false;
                        }
                    }
                }
            }
        }
        else
        {
            if (eventNumber == 2)
            {
                cout << "Table number missing in line " << lineCount << endl;
                return false;
            }
        }
    }
    file.close();
    return true;
}

int FindTableByClient(string& clientName, map<int, TableInfo> tableInfo)
{
    for (const auto& table : tableInfo)
    {
        if (table.second.clientName == clientName)
            return table.first; // Возвращаем номер стола, если клиент найден

    }
    return -1; // Возвращаем -1, если клиент не найден
}

int getSecondsFromMidnight(const tm& time) { return time.tm_hour * 3600 + time.tm_min * 60; }

int calculateRevenueForTable(tm time, int hourPrice)
{
    if (time.tm_min != 0)
        time.tm_hour += 1;
    int totalHours = time.tm_hour;
    int revenue = totalHours * hourPrice; // Общая выручка с одного стола
    return revenue;
}

void DiffTime(const tm& eventTime, const tm& time, int& table, map<int, TableInfo>& tableInfo)
{
    int startSeconds = getSecondsFromMidnight(time);
    int endSeconds = getSecondsFromMidnight(eventTime);
    tm currTime = tableInfo[table].totalTime;
    int currSeconds = getSecondsFromMidnight(currTime);
    int total = endSeconds - startSeconds + currSeconds;
    int totalHours = total / 3600;
    int totalMinutes = (total % 3600) / 60;

    tableInfo[table].totalTime.tm_hour = totalHours;
    tableInfo[table].totalTime.tm_min = totalMinutes;
}

void ClearQueue(list<string>& guestsQueue, list<string>& guestsList, const tm& eventTime,
    map<int, TableInfo>& tableInfo, int& occupiedTable, int& table)
{
    string client = guestsQueue.front();
    guestsQueue.pop_front();
    auto it = find(guestsList.begin(), guestsList.end(), client);
    if (it == guestsList.end())
    {
        cout << put_time(&eventTime, "%H:%M") << " " << 13 << " ClientUnknown!" << endl;
        return;
    }
    tableInfo[table].clientName = client;
    tableInfo[table].isFree = 0;
    tableInfo[table].startTime = eventTime;
    occupiedTable++;
    cout << put_time(&eventTime, "%H:%M") << " " << 12 << " " << client << " " << table << endl;
}

void ClearTable(const string& clientName, const tm& eventTime, int& occupiedTable, map< int, TableInfo>& tableInfo, int& table)
{
    tm time = tableInfo[table].startTime;
    tableInfo[table].isFree = 1;
    tableInfo[table].clientName = "";

    DiffTime(eventTime, time, table, tableInfo);

    occupiedTable--;
}