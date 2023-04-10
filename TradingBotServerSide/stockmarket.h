#pragma once

#ifndef STOCKMARKET_H
#define STOCKMARKET_H

#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <Windows.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <stdio.h>
#include <tchar.h>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <iostream>
#include <cpr/cpr.h>
#include <Lmcons.h>

#include "stock.h"
#include "dailydata.h"
#include "mysqlconnector.h"

class stockmarket {

	
public:
	std::vector <stock> allStocks;
	std::string basePath;
	std::string logPath;
	std::string symbolPath;
	std::string filestreamFile;
	std::string infoFile;


	tm startingTime = getCurrentTMtime();
	tm last15minAgg = getCurrentTMtime();
	tm last30minAgg = getCurrentTMtime();
	tm last1hourAgg = getCurrentTMtime();
	tm last2hourAgg = getCurrentTMtime();
	tm last4hourAgg = getCurrentTMtime();

	bool firstInit = false;
	bool marketOpen = checkMarketStatus();

public:
	stockmarket();
	
	void main();

	// stockmarketstream
	
	void initialize();
	void stream();
	void writeInfoFile();
	void initializeStocks(); 
	bool initializeDirectories(); 
	void initializeStockData(); 
	void init15minAgg(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void init30minAgg(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void init1hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void init2hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void init4hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void initRealTime(boost::filesystem::directory_entry symbolData, stock* workingStock);
	void clearFilestream(bool showMessage);
	void clearStockData();
	uint64_t timeSinceEpochMillisec();
	tm getCurrentTMtime();
	std::string getTimeStamp(tm& time);
	void appendStockData(tm& time);
	void appendAggregates(tm& time); 
	void append15minAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);
	void append30minAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);
	void append1hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);
	void append2hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);
	void append4hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);
	bool checkMarketStatus();
	void appendDailyData(tm& checkTime);
	std::string getCurrentDayString();
	bool getSimpleMovingAverage(int interval, int ticks, int startingPos, stock& stock);
	bool checkStreamTime();
	bool checkAggregateTime(int interval, tm& currentTime);
	int calcTimeCircle(int lastTime, int currTime);
	int getCurrentIntTime(tm currentTime);
	void appendSMAtoFile(double sma, std::string filepath);
	void appendSMAtoStreamFile(std::string streampath, int type, double value);
	void pushToMySQL(stock& stock, aggregate& aggregate, int type, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row);

	// indicators
	double movingAverage(stock& stock, int interval, int amount);
	void addAllMovingAverages();

	// algorithms
	void swingtrade(stock& stock);


};






#endif // !STOCKMARKET_H
