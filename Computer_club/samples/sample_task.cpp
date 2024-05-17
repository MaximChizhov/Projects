#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include "structure.h"

using namespace std;

int main(int argc, char* argv[]) //int argc, char* argv[]
{
    if (argc < 2) 
        cout << "No file at arguments." << endl;

    string fileName = argv[1];
    //string fileName;
    //cin >> fileName;
    if (!isValidFile(fileName))
       return 1;

    // Открытие файла
    ifstream file(fileName);

    // Проверка, удалось ли открыть файл
    if (!file.is_open())
    {
        cout << "File open error" << endl;
        return 1;
    }

    // Считывание и вывод строк из файла
    string numTablesStr;
    string workHoursStr;
    string hourPriceStr;

    // Считываем первые три строки
    getline(file, numTablesStr);
    getline(file, workHoursStr);
    getline(file, hourPriceStr);

    int numTables = stoi(numTablesStr);
    tm startTime, endTime;
    stringstream ss(workHoursStr);

    ss >> get_time(&startTime, "%H:%M") >> get_time(&endTime, "%H:%M");

    int hourPrice = stoi(hourPriceStr);
    cout << put_time(&startTime, "%H:%M") << endl;

    map<int, TableInfo> tableInfo;

    int occupiedTable = 0;
    list<string> guestsList;
    list<string> guestsQueue;
    string line;
    
    while (getline(file, line))
    {
        tm eventTime = {};
        int tableNumber = 0;
        int eventNumber = 0;
        string clientName;
        stringstream ss(line);
        ss >> get_time(&eventTime, "%H:%M"); // Считываем время события
        ss >> eventNumber; // Считываем номер события
        ss >> clientName; // Считываем имя клиента

        // Проверяем, есть ли число (номер стола) в конце строки
        string lastWord;
        if (ss >> lastWord)
            stringstream(lastWord) >> tableNumber;

        // Вывод полученных данных
        cout << put_time(&eventTime, "%H:%M") << " " << eventNumber << " " << clientName;
        if (tableNumber != 0)
            cout << " " << tableNumber;
        cout << endl;
        if (eventNumber == 1)
        {
            if (compareEventTime(eventTime, startTime))
            {
                auto it = find(guestsList.begin(), guestsList.end(), clientName);
                if (it == guestsList.end())
                    guestsList.push_back(clientName);
                else
                    cout << put_time(&eventTime, "%H:%M") << " " << 13 << " YouShallNotPass" << endl;
            }
            else 
                cout << put_time(&eventTime, "%H:%M") << " " << 13 << " NotOpenYet" << endl;
        }
        else if (eventNumber == 2)
        {
            auto it = find(guestsList.begin(), guestsList.end(), clientName);
            if (it != guestsList.end())
            {
                if (tableInfo[tableNumber].isFree)
                {
                    auto it2 = FindTableByClient(clientName, tableInfo); // обрабатываем случай пересадки
                    if (it2 != -1)
                    {
                        ClearTable(clientName, eventTime, occupiedTable, tableInfo, it2);
                        if (!guestsQueue.empty())
                            ClearQueue(guestsQueue, guestsList, eventTime, tableInfo, occupiedTable, it2);
                    }
                    tableInfo[tableNumber].isFree = 0;
                    tableInfo[tableNumber].clientName = clientName;
                    tableInfo[tableNumber].startTime = eventTime;
                    auto it3 = find(guestsQueue.begin(), guestsQueue.end(), clientName);
                    if (it3 != guestsQueue.end())
                        guestsQueue.remove(clientName);
;
                        
                    occupiedTable++;
                }
                else
                   cout << put_time(&eventTime, "%H:%M") << " " << 13 << " PlaceIsBusy" << endl;
            }
            else
                cout << put_time(&eventTime, "%H:%M") << " " << 13 << " ClientUnknown" << endl;
            
        }
        else if (eventNumber == 3)
        {
            if (occupiedTable != numTables)
                cout << put_time(&eventTime, "%H:%M") << " " << 13 << " ICanWaitNoLonger" << endl;
            guestsQueue.push_back(clientName);
            if (guestsQueue.size() > numTables)
            {
                string guest = guestsQueue.back();
                guestsQueue.pop_back();
                guestsList.remove(guest);
                cout << put_time(&endTime, "%H:%M") << " 11 " << guest << endl;
            }
        }
        else if (eventNumber == 4)
        {
            auto it = find(guestsList.begin(), guestsList.end(), clientName);
            if (it != guestsList.end())
            {
                int table = FindTableByClient(clientName, tableInfo);
                ClearTable(clientName, eventTime, occupiedTable, tableInfo,table);
                guestsList.remove(clientName);
                if (!guestsQueue.empty())
                    ClearQueue(guestsQueue, guestsList, eventTime, tableInfo, occupiedTable, table);
            }
            else
                cout << put_time(&eventTime, "%H:%M") << " " << 13 << " ClientUnknown!" << endl;
        }

    }
    while (!guestsList.empty())
    {
        string guest = guestsList.front();
        int table = FindTableByClient(guest, tableInfo);
        cout << put_time(&endTime, "%H:%M") << " 11 " << guest << endl;
        ClearTable(guest, endTime, occupiedTable, tableInfo, table);
        guestsList.remove(guest);
    }
    cout << put_time(&endTime, "%H:%M") << endl;
    for (const auto& table : tableInfo)
        cout << table.first << " " << calculateRevenueForTable(table.second.totalTime, hourPrice) << " " << put_time(&table.second.totalTime, "%H:%M") << endl;

    // Закрытие файла
    file.close();

    return 0;
}