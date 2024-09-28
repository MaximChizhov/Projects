#include <gtest.h>

#include "structure.h"

using namespace std; 

// TableInfo Constructor
TEST(TableInfo, default_constructor_test) 
{
    TableInfo tableInfo;

    ASSERT_TRUE(tableInfo.isFree);
    EXPECT_EQ(tableInfo.clientName, "");
}
//CompareEventTime
TEST(CompareEventTime, different_hours_test_true) 
{
    tm eventTime = {};
    eventTime.tm_hour = 9;
    eventTime.tm_min = 48;
    tm startTime = {};
    startTime.tm_hour = 8;
    startTime.tm_min = 32;

    bool result = compareEventTime(eventTime, startTime);

    ASSERT_TRUE(result); 
}

TEST(CompareEventTime, different_hours_test_false)
{
    tm eventTime = {};
    eventTime.tm_hour = 7;
    eventTime.tm_min = 48;
    tm startTime = {};
    startTime.tm_hour = 8;
    startTime.tm_min = 32;

    bool result = compareEventTime(eventTime, startTime);

    ASSERT_FALSE(result);
}

TEST(CompareEventTime, same_hours_test_true)
{
    tm eventTime = {};
    eventTime.tm_hour = 9;
    eventTime.tm_min = 48;
    tm startTime = {};
    startTime.tm_hour = 9;
    startTime.tm_min = 32;

    bool result = compareEventTime(eventTime, startTime);

    ASSERT_TRUE(result);
}

TEST(CompareEventTime, same_hours_test_false)
{
    tm eventTime = {};
    eventTime.tm_hour = 8;
    eventTime.tm_min = 30;
    tm startTime = {};
    startTime.tm_hour = 8;
    startTime.tm_min = 35;

    bool result = compareEventTime(eventTime, startTime);

    ASSERT_FALSE(result);
}
//calculateRevenueForTable
TEST(calculateRevenueForTable, exactly_eight_hours)
{
    tm time = {};
    time.tm_hour = 8;
    time.tm_min = 0;

    int price = 15;
    int total = calculateRevenueForTable(time, price);
    EXPECT_EQ(total, 15 * 8);
}

TEST(calculateRevenueForTable, exactly_eight_hours_and_one_minute)
{
    tm time = {};
    time.tm_hour = 8;
    time.tm_min = 1;

    int price = 15;
    int total = calculateRevenueForTable(time, price);
    EXPECT_EQ(total, 15 * 9);
}

TEST(calculateRevenueForTable, exactly_eight_hours_and_forty_five_minutes)
{
    tm time = {};
    time.tm_hour = 8;
    time.tm_min = 45;

    int price = 15;
    int total = calculateRevenueForTable(time, price);
    EXPECT_EQ(total, 15 * 9);
}
//getSecondsFromMidnight
TEST(getSecondsFromMidnight, get_seconds_from_ten_minutes)
{
    tm time = {};
    time.tm_hour = 0;
    time.tm_min = 10;

    int seconds = getSecondsFromMidnight(time);
    EXPECT_EQ(seconds, 60*10);
}

TEST(getSecondsFromMidnight, get_seconds_from_ten_hours)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 0;

    int seconds = getSecondsFromMidnight(time);
    EXPECT_EQ(seconds, 60*60*10);
}

TEST(getSecondsFromMidnight, get_seconds_from_eight_hours_and_forty_two_minutes)
{
    tm time = {};
    time.tm_hour = 8;
    time.tm_min = 42;

    int seconds = getSecondsFromMidnight(time);
    EXPECT_EQ(seconds, 8*60*60+42*60);
}
//FindTableByClient
TEST(FindTableByClient, find_client_true)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    map<int, TableInfo> table;
    table[1].isFree = 0;
    table[1].clientName = "John";
    table[1].startTime = time;

    table[2].isFree = 0;
    table[2].clientName = "Kate";
    table[2].startTime = time;

    string name1 = "John";
    string name2 = "Kate";

    EXPECT_EQ(FindTableByClient(name1, table), 1);
    EXPECT_EQ(FindTableByClient(name2, table), 2);
}

TEST(FindTableByClient, find_client_false)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    map<int, TableInfo> table;
    table[1].isFree = 0;
    table[1].clientName = "John";
    table[1].startTime = time;

    table[2].isFree = 0;
    table[2].clientName = "Kate";
    table[2].startTime = time;

    string name1 = "Anna";

    EXPECT_EQ(FindTableByClient(name1, table), -1);
}
//DiffTime
TEST(DiffTime, time_difference_test_one)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    int tab = 1;
    map<int, TableInfo> table;
    table[tab].isFree = 0;
    table[tab].clientName = "John";
    table[tab].startTime = time;

    tm time2 = {};
    time2.tm_hour = 15;
    time2.tm_min = 43;


    DiffTime(time2, time, tab, table);
    tm diff = {};
    diff.tm_hour = 5;
    diff.tm_min = 16;

    EXPECT_EQ(table[tab].totalTime.tm_hour, diff.tm_hour);
    EXPECT_EQ(table[tab].totalTime.tm_min, diff.tm_min);
}

TEST(DiffTime, time_difference_test_two)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    int tab = 1;
    map<int, TableInfo> table;
    table[tab].isFree = 0;
    table[tab].clientName = "John";
    table[tab].startTime = time;

    tm time2 = {};
    time2.tm_hour = 14;
    time2.tm_min = 13;


    DiffTime(time2, time, tab, table);
    tm diff = {};
    diff.tm_hour = 3;
    diff.tm_min = 46;

    EXPECT_EQ(table[tab].totalTime.tm_hour, diff.tm_hour);
    EXPECT_EQ(table[tab].totalTime.tm_min, diff.tm_min);
}
//ClearTable
TEST(ClearTable, clear_table_test)
{
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    int tab = 1;
    string name = "John";
    map<int, TableInfo> table;
    table[tab].isFree = 0;
    table[tab].clientName = name;
    table[tab].startTime = time;
    int occupied = 1;
    tm time2 = {};
    time2.tm_hour = 14;
    time2.tm_min = 13;
 

    ClearTable(name, time2, occupied, table, tab);
    tm diff = {};
    diff.tm_hour = 3;
    diff.tm_min = 46;

    ASSERT_TRUE(table[tab].isFree);
    EXPECT_EQ(table[tab].clientName, "");
    EXPECT_EQ(table[tab].totalTime.tm_hour, diff.tm_hour);
    EXPECT_EQ(table[tab].totalTime.tm_min, diff.tm_min);
    EXPECT_EQ(occupied, 0);
}
//ClearQueue
TEST(ClearQueue, clear_queue_test)
{
    list<string> queue;
    list<string> guests;
    string name = "kate";
    queue.push_back(name);
    guests.push_back(name);
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    int tab = 1;
    string name1 = "John";
    map<int, TableInfo> table;
    table[tab].isFree = 0;
    table[tab].clientName = name;
    table[tab].startTime = time;
    int occupied = 1;
    tm time2 = {};
    time2.tm_hour = 14;
    time2.tm_min = 13;

    int tab2 = 2;

    ClearQueue(queue, guests, time2, table, occupied, tab2);

    ASSERT_FALSE(table[tab2].isFree);
    EXPECT_EQ(table[tab2].clientName, name);
    EXPECT_EQ(table[tab2].startTime.tm_hour, time2.tm_hour);
    EXPECT_EQ(table[tab2].startTime.tm_min, time2.tm_min);
    EXPECT_EQ(occupied, 2);
    EXPECT_EQ(queue.size(), 0);
}

TEST(ClearQueue, clear_queue_test_unknown_client)
{
    list<string> queue;
    list<string> guests;
    string name = "kate";
    queue.push_back(name);
    tm time = {};
    time.tm_hour = 10;
    time.tm_min = 27;
    int tab = 1;
    string name1 = "John";
    map<int, TableInfo> table;
    table[tab].isFree = 0;
    table[tab].clientName = name;
    table[tab].startTime = time;
    int occupied = 1;
    tm time2 = {};
    time2.tm_hour = 14;
    time2.tm_min = 13;

    int tab2 = 2;

    stringstream capturedOutput;

    // Перенаправляем stdout на захватывающий поток
    streambuf* originalOutput = cout.rdbuf();
    cout.rdbuf(capturedOutput.rdbuf());

    ClearQueue(queue, guests, time2, table, occupied, tab2);

    cout.rdbuf(originalOutput);

    // Проверяем вывод на консоль
    string output = capturedOutput.str();
    EXPECT_EQ(output, "14:13 13 ClientUnknown!\n");
}