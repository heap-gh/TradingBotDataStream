
#pragma warning( push )
#pragma warning( disable : 4244)

#include "stockmarket.h"


std::mutex lock;

stockmarket::stockmarket() {

    main();

}



void stockmarket::main() {

    //optional
    //clearStockData();
    
    initialize();
    stream();
    
}



std::string stockmarket::getTimeStamp(tm& time) {

    std::string timestamp = "[";    

    if (time.tm_hour < 10) {
        timestamp += "0" + std::to_string(time.tm_hour) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_hour) + ":";
    }

    if (time.tm_min < 10) {
        timestamp += "0" + std::to_string(time.tm_min) + ":";
    }
    else {
        timestamp += std::to_string(time.tm_min) + ":";
    }

    if (time.tm_sec < 10) {
        timestamp += "0" + std::to_string(time.tm_sec);
    }
    else {
        timestamp += std::to_string(time.tm_sec);
    }

    timestamp += "]";

    return timestamp;

}



void stockmarket::stream() {

    tm time = getCurrentTMtime();

    std::cout << getTimeStamp(time) + "[STREAM] Starting \n";

    if (this->marketOpen == false) {
        std::cout << getTimeStamp(time) + "[STREAM] Market Closed. Waiting ... \n";
    }

    while (true) {

        if (checkStreamTime()) {

            time = getCurrentTMtime();
            appendStockData(time); // 20sec

        }

        Sleep(50);

    }

}



void stockmarket::initialize() {

    
    char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    GetUserNameA(acUserName, &nUserName);

    this->basePath = std::string("C:/Users/") + std::string(acUserName) + std::string("/Desktop/TradingBot/");
    this->logPath = std::string("C:/Users/") + std::string(acUserName) + std::string("/Desktop/TradingBot/logFiles/");
    this->symbolPath = std::string("C:/Users/") + std::string(acUserName) + std::string("/Desktop/TradingBot/symbols/");
    this->filestreamFile = std::string("C:/Users/") + std::string(acUserName) + std::string("/Desktop/TradingBot/filestream.txt");
    this->infoFile = std::string("C:/Users/") + std::string(acUserName) + std::string("/Desktop/TradingBot/info.txt");


    tm start = getCurrentTMtime();

    clearFilestream(true);
    std::cout << getTimeStamp(start) + "[INIT] Starting \n";
 
    initializeDirectories();
    writeInfoFile();
    initializeStocks();
    initializeStockData();

    tm ending = getCurrentTMtime();
    std::cout << getTimeStamp(ending) + "[INIT] Done\n";
}



void stockmarket::writeInfoFile() {

    std::ofstream infoFile(this->infoFile);

    if (infoFile.is_open()) {
        
        infoFile << "THE DATA IN SYMBOL DIRECTORY: " << this->symbolPath << "(TICKER)\n"
            << "(TICKER)1hour = 1 hour aggreagtes\n"
            << "(TICKER)2hour = 2 hour aggregates\n"
            << "(TICKER)4hour = 4 hour aggregates\n"
            << "(TICKER)15m = 15 minute aggregates\n"
            << "(TICKER)30m = 30 minute aggregates\n"
            << "(TICKER)DAILY = Daily aggregates\n"
            << "(TICKER)RTDATA = 1 minute aggregates\n"
            << "(TICKER)stream = File for TICKER algorithm to read the data\n\n\n\n"
            << "__AGGREGATE STRUCT__ (15min, 30min, 1hour, 2hour, 4hour)\n"
            << "OPEN,CLOSE,HIGH,LOW\n\n\n\n"
            << "IMPORTANT: PROGRAM WILL ONLY STREAM UNTIL 22:00 WHILE DATA IS APPENDED UNTIL 22:15\n"
            << "IMPORTANT: TODAYS AGGREGATE WILL BE APPENDEN ON 22:30:30. TRY NOT TO START PROGRAM AFTER THIS TIME\n\n\n\n";

        infoFile.close();
    }


}



bool stockmarket::checkStreamTime() {

    tm time = getCurrentTMtime();
    int timer = getCurrentIntTime(time);

    appendDailyData(time);

    if (time.tm_hour == 15 && time.tm_min == 44 && time.tm_sec == 45) {
        clearFilestream(true);
        Sleep(1000);
    }

    if (time.tm_hour == 15 && time.tm_min == 35 && time.tm_sec == 30) {
        this->marketOpen = checkMarketStatus();
        Sleep(2000);
    } 

    if (((timer > 1544) && (timer < 2216)) && this->marketOpen) {

        if (this->firstInit == false) { 

            std::cout << getTimeStamp(time) << "[FIRST INIT]\n";
            this->startingTime = time;
            this->last15minAgg = time;
            this->last30minAgg = time;
            this->last1hourAgg = time;
            this->last2hourAgg = time;
            this->last4hourAgg = time;
            
            this->firstInit = true;
        }
        return true;

    }

    return false;

}


// clears the filestream file
void stockmarket::clearFilestream(bool showMessage) {

    std::ofstream ofs;
    ofs.open(this->filestreamFile, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
    
    if (showMessage) {
        tm start = getCurrentTMtime();
        std::cout << getTimeStamp(start) + "[CLEAR] Filestream cleared\n";
    }
}


// clears all stock data files
void stockmarket::clearStockData() {

    tm start = getCurrentTMtime();
    std::cout << getTimeStamp(start) + "[CLEAR] Clearing Stock Data --> ";

    using namespace boost::filesystem;

    for (directory_entry& symbolDir : directory_iterator(this->symbolPath)) {
        for (directory_entry& symbolData : directory_iterator(symbolDir.path())) {
            remove(symbolData.path());
        }
    }

    tm ending = getCurrentTMtime();
    std::cout << getTimeStamp(ending) + "[+] Clearing Stock Data finished\n";

}


// initializes all stocks
void stockmarket::initializeStocks() {

    using namespace boost::filesystem;

    tm start = getCurrentTMtime();
    std::cout << "    " + getTimeStamp(start) + "[+] Initializing Stocks --> ";


    int x = 0;

    for (directory_entry& symbolDir : directory_iterator(this->symbolPath)) {
        
        stock newStock;

        std::wstring ws(symbolDir.path().filename().c_str());
        std::string symbol(ws.begin(), ws.end());
        
        newStock.symbol = symbol;
        newStock.index = x;

        this->allStocks.push_back(newStock);

        x++;
    }
    
    tm ending = getCurrentTMtime();
    std::cout << getTimeStamp(ending) + "[+] Finished\n";

}


// initializes all directories
bool stockmarket::initializeDirectories() {


    struct stat info;

    std::vector <std::string> directorys = { this->basePath, this->logPath, this->symbolPath};

    // Check the directory tree
    for (std::string directory : directorys) {

        if (stat(directory.c_str(), &info) != 0) {
            std::cout << "    [DIR] " << directory << " not found! Creating it ... \n";
            bool couldCreate = _mkdir(directory.c_str());
            if (!couldCreate) {
                std::cout << "    [DIR] Successfully created " << directory << "\n";
                
            }
            else {
                std::cout << "    [DIR] Could not create " << directory << "\n";
                return false;
            }
        }
        else if (info.st_mode & S_IFDIR) {
            std::cout << "    [DIR] " << directory << " is present!\n";
        }
        else {
            std::cout << "    [DIR] Could not create directory! (" << directory << ")\n";
            return false;
        }

    }

    return true;
}



std::string stockmarket::getCurrentDayString() {

    tm time = getCurrentTMtime();

    std::string yearSSTR = std::to_string(time.tm_year + 1900);
    std::string monthSTR = "";
    if (time.tm_mon < 10) {
        monthSTR += "0" + std::to_string(time.tm_mon + 1);
    }
    else {
        monthSTR = std::to_string(time.tm_mon + 1);
    }

    std::string daySTR = "";
    if (time.tm_mday < 10) {
        daySTR += "0" + std::to_string(time.tm_mday);
    }
    else {
        daySTR = std::to_string(time.tm_mday);
    }

    return yearSSTR + "-" + monthSTR + "-" + daySTR;

}



void stockmarket::appendDailyData(tm& checkTime) {

    if (checkTime.tm_hour == 23 && checkTime.tm_min == 30 && checkTime.tm_sec == 30) {

        std::cout << getTimeStamp(checkTime) << "[DAILY] Appending Daily Aggregate\n";

        for (stock& stock : this->allStocks) {
                        
            std::ofstream dailyDataFile("C:/Users/kiyan/Desktop/TradingBot/symbols/" + stock.symbol + "/" + stock.symbol + "DAILY.txt", std::ios::app);

            if (dailyDataFile.is_open()) {

                //daily data only save in file
                std::string date = getCurrentDayString();
                std::string url = "https://api.polygon.io/v1/open-close/" + stock.symbol + "/" + date + "?adjusted=true&apiKey=yTkl2Z4b3aDRRtFYhnJvzCkYtwwgcgIS";
                std::string request = cpr::Get(cpr::Url{ url }).text;

                std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
                if (stockstreamFile.is_open()) {
                    stockstreamFile << request + "\n";
                    stockstreamFile.close();
                }
                else {
                    std::cout << "\n    [-] Could not open stock stream file for " << stock.symbol;
                }
                dailyDataFile << request << "\n";

                dailyDataFile.close();
            }
        }

        tm ending = getCurrentTMtime();
        std::cout << getTimeStamp(ending) << "[DAILY] Done\n";
    
    }

}



void stockmarket::init15minAgg(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream min15File(symbolData.path().c_str());
    if (min15File.is_open()) {

        aggregate min15Aggregate;
        std::string data = "";

        while (getline(min15File, data)) {

            workingStock->min15.push_back(min15Aggregate.getAggregateStruct(data));
        }
    
    }
    else {
        std::cerr << "    [-] Could not open 15min aggregate File for " << symbolData.path().c_str() << "\n";

    }

}



void stockmarket::init30minAgg(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream hour1File(symbolData.path().c_str());
    if (hour1File.is_open()) {

        aggregate min30Aggregate;
        std::string data = "";

        while (getline(hour1File, data)) {

            workingStock->min30.push_back(min30Aggregate.getAggregateStruct(data));
        }

    }
    else {
        std::cerr << "    [-] Could not open 30min aggregate File for " << symbolData.path().c_str() << "\n";

    }

}



void stockmarket::init1hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream hour1File(symbolData.path().c_str());
    if (hour1File.is_open()) {

        aggregate hour1Aggregate;
        std::string data = "";

        while (getline(hour1File, data)) {
            
            workingStock->hour1.push_back(hour1Aggregate.getAggregateStruct(data));
        }

    }
    else {
        std::cerr << "    [-] Could not open 1hour aggregate File for " << symbolData.path().c_str() << "\n";

    }

}



void stockmarket::init2hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream hour2File(symbolData.path().c_str());
    if (hour2File.is_open()) {

        aggregate hour2Aggregate;
        std::string data = "";

        while (getline(hour2File, data)) {

            workingStock->hour2.push_back(hour2Aggregate.getAggregateStruct(data));
        }

    }
    else {
        std::cerr << "    [-] Could not open 2hour aggregate File for " << symbolData.path().c_str() << "\n";

    }

}



void stockmarket::init4hourAgg(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream hour4File(symbolData.path().c_str());
    if (hour4File.is_open()) {

        aggregate hour4Aggregate;
        std::string data = "";

        while (getline(hour4File, data)) {

            workingStock->hour4.push_back(hour4Aggregate.getAggregateStruct(data));
        }

    }
    else {
        std::cerr << "    [INIT] Could not open 4hour aggregate File for " << symbolData.path().c_str() << "\n";

    }

}



void stockmarket::initRealTime(boost::filesystem::directory_entry symbolData, stock* workingStock) {

    std::ifstream dataFile(symbolData.path().c_str());
    std::string line;
    std::vector <std::string> allLines;
    
    if (dataFile.is_open()){
        while (getline(dataFile, line)) {
            allLines.push_back(line);
        }
        dataFile.close();

        bool firstIteration = true;
        std::string symbol = "";
        size_t stockIndex = 0;

        for (std::string dataLine : allLines) {

            if (firstIteration) {
                firstIteration = false;
                symbol = realtimedata::getSymbol(dataLine);
                bool stockPresent = false;

                for (stock& stock : this->allStocks) {
                    if (stock.symbol == symbol) {
                        stockIndex = stock.index;
                        stockPresent = true;
                        break;
                    }
                }

                if (!stockPresent) {
                    stock newStock;
                    newStock.symbol = symbol;
                    newStock.index = this->allStocks.size();
                    this->allStocks.push_back(newStock);
                    stockIndex = newStock.index;
                }
                
            }

            this->allStocks.at(stockIndex).stockData.push_back(realtimedata::getRealTimeDataStruct(dataLine));

        }

    }
    else {
        std::cerr << "    [INIT] Could not open RTDATA-File File for " << symbolData.path().c_str() << "\n";
    }
    

    

}


// initializes stock dataand pushes all aggregates into memory
void stockmarket::initializeStockData() {

    using namespace boost::filesystem;

    tm start = getCurrentTMtime();
    std::cout << "    " + getTimeStamp(start) + "[+] Initializing Stock Data--> ";

    int progress = 0;

    for (directory_entry& symbolDir : directory_iterator(this->symbolPath)) {

        stock* workingStock = nullptr;
        
        // convert the symbol
        std::string symbolPath = symbolDir.path().string();
        std::string symbol = "";
        
        size_t index = symbolPath.find("symbols") + 7;
        for (index; index < symbolPath.length(); index++) {
            if (std::isalpha(symbolPath[index])) {
                symbol += symbolPath[index];
            }
        }
        
        for (stock& stock : this->allStocks) {
            if (stock.symbol == symbol) {
                workingStock = &stock;
            }
        }

        std::vector <std::thread> initThreads;

        for (directory_entry& symbolData : directory_iterator(symbolDir.path())) {


            auto name = symbolData.path().c_str();
            std::wstring string(name);

            if (string.find(L"15m") != std::string::npos) {

                std::thread init15Thread(&stockmarket::init15minAgg,this ,symbolData, workingStock);
                initThreads.push_back(std::move(init15Thread));

            }
            else if (string.find(L"30m") != std::string::npos) {
                
                std::thread init30Thread(&stockmarket::init30minAgg, this, symbolData, workingStock);
                initThreads.push_back(std::move(init30Thread));

            }
            else if (string.find(L"1hour") != std::string::npos) {

                std::thread init1hourThread(&stockmarket::init1hourAgg, this, symbolData, workingStock);
                initThreads.push_back(std::move(init1hourThread));

            }
            else if (string.find(L"2hour") != std::string::npos) {

                std::thread init2hourThread(&stockmarket::init2hourAgg, this, symbolData, workingStock);
                initThreads.push_back(std::move(init2hourThread));

            }
            else if (string.find(L"4hour") != std::string::npos) {

                std::thread init4hourThread(&stockmarket::init4hourAgg, this, symbolData, workingStock);
                initThreads.push_back(std::move(init4hourThread));

            }
            else if (string.find(L"RTDATA") != std::string::npos) {
                    
                std::thread initRealTimeThread(&stockmarket::initRealTime, this, symbolData, workingStock);
                initThreads.push_back(std::move(initRealTimeThread));
                
            }

        }
        
        for (std::thread& workerThread : initThreads) {
            if (workerThread.joinable())
                workerThread.join();
        }

        initThreads.clear();
        
        progress++;

    }

    tm ending = getCurrentTMtime();
    std::cout << getTimeStamp(ending) + "[+] Finished\n";

}


// appends stock data during runtime
void stockmarket::appendStockData(tm& currTime) {

    if (currTime.tm_sec == 20) {
       
        std::ifstream filestream(this->filestreamFile);
       
        if (filestream.is_open()) {

            std::cout << getTimeStamp(currTime) + "[STREAM] Appending Stockdata --> ";
            
            std::vector <std::string> allLines;
            std::string line;

            while (getline(filestream, line)) {
                allLines.push_back(line);
            }

            bool oneDoesNotExist = false;

            for (std::string dataLine : allLines) {
                    
                std::string symbol = realtimedata::getSymbol(dataLine);

                // push realtimedata in memory
                bool stockExists = false;
                for (stock& stock : this->allStocks) {
                    if (stock.symbol == symbol) {
                        stockExists = true;

                        // Add stockdata to stock
                        realtimedata data = realtimedata::getRealTimeDataStruct(dataLine);
                        stock.stockData.push_back(data);
                        stock.last15minData.push_back(data);
                        stock.last30minData.push_back(data);
                        stock.last1hourdata.push_back(data);
                        stock.last2hourdata.push_back(data);
                        stock.last4hourdata.push_back(data);
                        break;
                    }
                }

                if (!stockExists) {
                    stock newStock;
                    newStock.symbol = symbol;
                    newStock.index = this->allStocks.size();
                    oneDoesNotExist = true;

                    // Add stockdata to stock
                    realtimedata data = realtimedata::getRealTimeDataStruct(dataLine);
                    newStock.stockData.push_back(data);
                    newStock.last15minData.push_back(data);
                    newStock.last30minData.push_back(data);
                    newStock.last1hourdata.push_back(data);
                    newStock.last2hourdata.push_back(data);
                    newStock.last4hourdata.push_back(data);

                    this->allStocks.push_back(newStock);
                }

                // push only realtimedata in file
                std::ofstream dataFile(this->symbolPath + symbol + "/" + symbol + "RTDATA.txt", std::ios::app);
                if (dataFile.is_open()) {
                    dataFile << dataLine + "\n";
                    dataFile.close();
                }
                else {
                        
                    std::string symbolPath = this->symbolPath;
                    std::string wholePath = symbolPath + symbol + "/";
                        
                    if (!_mkdir(wholePath.c_str())) {
                            
                        std::cout << "\n    [+] Created path for " << symbol;
                        std::ofstream dataFile(this->symbolPath + symbol + "/" + symbol + "RTDATA.txt", std::ios::app);
                        if (dataFile.is_open()) {
                            dataFile << dataLine + "\n";
                            dataFile.close();
                        }
                        else {
                            std::cerr << "\n    [-] Still could not create path for " << symbol;
                        }

                    }
                    else {
                        std::cerr << "\n    [-] Could not create path for " << symbol;
                    }

                }
                // push realtimedata in stockstreamfile
                std::ofstream stockstreamFile(this->symbolPath + symbol + "/" + symbol + "stream.txt", std::ios::app);
                if (stockstreamFile.is_open()) {
                    stockstreamFile << dataLine + "\n";
                    stockstreamFile.close();
                }
                else {
                    std::cout << "\n    [-] Could not open stock stream file for " << symbol;
                }
            }

            if (oneDoesNotExist == true) {
                std::cout << "\n";
            }

            appendAggregates(currTime);

            tm ending = getCurrentTMtime();
            std::cout << getTimeStamp(ending) + "[STREAM] Finished\n";
            clearFilestream(false);
            Sleep(2000);

        } else {

            std::cout << getTimeStamp(currTime) + "[ERROR] Could not open filestream\n";
        }
    }
}



void stockmarket::pushToMySQL(stock& stock, aggregate& aggregate, int type, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row) {

    using namespace ::databaseconnection;

    std::string tableType = "";

    switch (type) {

    case 0:
        tableType = "15min";
        break;

    case 1:
        tableType = "30min";
        break;
    
    case 2:
        tableType = "1hour";
        break;

    case 3:
        tableType = "2hour";
        break;

    case 4:
        tableType = "4hour";
        break;
    
    }

    std::string symbol = stock.symbol;
    std::transform(symbol.begin(), symbol.end(), symbol.begin(), ::tolower);
    std::string table = symbol + "_" + tableType;

    
    tm time = getCurrentTMtime();
    std::string timestamp = std::to_string(time.tm_year + 1900) + "." + std::to_string(time.tm_mon + 1) + "." + std::to_string(time.tm_mday) + "-" + getTimeStamp(time);
    bool fail = false;
    mysql_perform_query(con, "INSERT INTO " + table + " (open, close, high, low, timestamp) VALUES(" + '\'' + std::to_string(aggregate.open) + "\', " + '\'' + std::to_string(aggregate.close) + "\', " + '\'' + std::to_string(aggregate.high) + "\', " + '\'' + std::to_string(aggregate.low) + "\', '" + timestamp + '\'' + "); ", fail);
    

    if (fail) {
        mysql_perform_query(con, "CREATE TABLE " + table + " (`open` VARCHAR(255),`close` VARCHAR(255),`high` VARCHAR(255),`low` VARCHAR(255),`timestamp` VARCHAR(255)); ", fail);
        mysql_perform_query(con, "INSERT INTO " + table + " (open, close, high, low, timestamp) VALUES(" + '\'' + std::to_string(aggregate.open) + "\', " + '\'' + std::to_string(aggregate.close) + "\', " + '\'' + std::to_string(aggregate.high) + "\', " + '\'' + std::to_string(aggregate.low) + "\', '" + timestamp + '\'' + "); ", fail);
    }


}



void stockmarket::append15minAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row){
    
    aggregate newData = {};

    // push in memory
    if (!stock.last15minData.empty()) {
        
        newData.high = stock.last15minData.at(0).high;
        newData.low = stock.last15minData.at(0).low;
        newData.open = stock.last15minData.at(0).open;
        newData.close = stock.last15minData.at(stock.last15minData.size() - 1).close;

        for (realtimedata& data : stock.last15minData) {

            if (data.high > newData.high) {
                newData.high = data.high;
            }
            
            if (data.low < newData.low) {
                newData.low = data.low;
            }

        }

        // push to mysql
        pushToMySQL(stock, newData, 0, con, res, row);

        stock.min15.push_back(newData);

        // push in file
        std::ofstream dataFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "15m.txt", std::ios::app);
        if (dataFile.is_open()) {

            dataFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",\n";
            dataFile.close();
        }
        else {
            std::cerr << "    [-] Could not open 15min Agg Data File for " << stock.symbol << "\n";
        }
        
        lock.lock();
        // push in stockstream file
        std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
        if (stockstreamFile.is_open()) {
            stockstreamFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",(15min)\n";
            stockstreamFile.close();
        }
        else {
            std::cout << "    [-] Could not open stock stream file for " << stock.symbol << "\n";
        }
        lock.unlock();

    }

}



void stockmarket::append30minAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row) {

    aggregate newData = {};

    // push in memory
    if (!stock.last30minData.empty()) {

        newData.high = stock.last30minData.at(0).high;
        newData.low = stock.last30minData.at(0).low;
        newData.open = stock.last30minData.at(0).open;
        newData.close = stock.last30minData.at(stock.last30minData.size() - 1).close;

        for (realtimedata& data : stock.last30minData) {

            if (data.high > newData.high) {
                newData.high = data.high;
            }

            if (data.low < newData.low) {
                newData.low = data.low;
            }

        }

        // push to mysql
        pushToMySQL(stock, newData, 1, con, res, row);

        stock.min30.push_back(newData);

        // push in file
        std::ofstream dataFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "30m.txt", std::ios::app);
        if (dataFile.is_open()) {

            dataFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",\n";
            dataFile.close();
        }
        else {
            std::cerr << "    [-] Could not open 30min Agg Data File for " << stock.symbol << "\n";
        }

        lock.lock();
        // push in stockstream file
        std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
        if (stockstreamFile.is_open()) {
            stockstreamFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",(30min)\n";
            stockstreamFile.close();
        }
        else {
            std::cout << "    [-] Could not open stock stream file for " << stock.symbol << "\n";
        }
        lock.unlock();

    }

}



void stockmarket::append1hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row) {

    aggregate newData = {};

    // push in memory
    if (!stock.last1hourdata.empty()) {

        newData.high = stock.last1hourdata.at(0).high;
        newData.low = stock.last1hourdata.at(0).low;
        newData.open = stock.last1hourdata.at(0).open;
        newData.close = stock.last1hourdata.at(stock.last1hourdata.size() - 1).close;

        for (realtimedata& data : stock.last1hourdata) {

            if (data.high > newData.high) {
                newData.high = data.high;
            }

            if (data.low < newData.low) {
                newData.low = data.low;
            }
        }

        // push to mysql
        pushToMySQL(stock, newData, 2, con, res, row);

        stock.hour1.push_back(newData);

        // push in file
        std::ofstream dataFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "1hour.txt", std::ios::app);
        if (dataFile.is_open()) {

            dataFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",\n";
            dataFile.close();
        }
        else {
            std::cerr << "    [-] Could not open 1hour Agg Data File for " << stock.symbol << "\n";
        }

        lock.lock();
        // push in stockstream file
        std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
        if (stockstreamFile.is_open()) {
            stockstreamFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",(1hour)\n";
            stockstreamFile.close();
        }
        else {
            std::cout << "    [-] Could not open stock stream file for " << stock.symbol << "\n";
        }
        lock.unlock();

    }

}



void stockmarket::append2hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row) {

    aggregate newData = {};

    // push in memory
    if (!stock.last2hourdata.empty()) {

        newData.high = stock.last2hourdata.at(0).high;
        newData.low = stock.last2hourdata.at(0).low;
        newData.open = stock.last2hourdata.at(0).open;
        newData.close = stock.last2hourdata.at(stock.last2hourdata.size() - 1).close;

        for (realtimedata& data : stock.last2hourdata) {

            if (data.high > newData.high) {
                newData.high = data.high;
            }

            if (data.low < newData.low) {
                newData.low = data.low;
            }
        }

        // push to mysql
        pushToMySQL(stock, newData, 3, con, res, row);

        stock.hour2.push_back(newData);

        // push in file
        std::ofstream dataFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "2hour.txt", std::ios::app);
        if (dataFile.is_open()) {

            dataFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",\n";
            dataFile.close();
        }
        else {
            std::cerr << "    [-] Could not open 2hour Agg Data File for " << stock.symbol << "\n";
        }

        lock.lock();
        // push in stockstream file
        std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
        if (stockstreamFile.is_open()) {
            stockstreamFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",(2hour)\n";
            stockstreamFile.close();
        }
        else {
            std::cout << "    [-] Could not open stock stream file for " << stock.symbol << "\n";
        }
        lock.unlock();

    }

}



void stockmarket::append4hourAgg(stock& stock, MYSQL* con, MYSQL_RES* res, MYSQL_ROW row) {

    aggregate newData = {};

    // push in memory
    if (!stock.last4hourdata.empty()) {

        newData.high = stock.last4hourdata.at(0).high;
        newData.low = stock.last4hourdata.at(0).low;
        newData.open = stock.last4hourdata.at(0).open;
        newData.close = stock.last4hourdata.at(stock.last4hourdata.size() - 1).close;

        for (realtimedata& data : stock.last4hourdata) {

            if (data.high > newData.high) {
                newData.high = data.high;
            }

            if (data.low < newData.low) {
                newData.low = data.low;
            }
        }

        // push to mysql
        pushToMySQL(stock, newData, 4, con, res, row);

        stock.hour4.push_back(newData);

        // push in file
        std::ofstream dataFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "4hour.txt", std::ios::app);
        if (dataFile.is_open()) {

            dataFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",\n";
            dataFile.close();
        }
        else {
            std::cerr << "    [-] Could not open 4hour Agg Data File for " << stock.symbol << "\n";
        }

        lock.lock();
        // push in stockstream file
        std::ofstream stockstreamFile(this->symbolPath + stock.symbol + "/" + stock.symbol + "stream.txt", std::ios::app);
        if (stockstreamFile.is_open()) {
            stockstreamFile << newData.open << "," << newData.close << "," << newData.high << "," << newData.low << ",(4hour)\n";
            stockstreamFile.close();
        }
        else {
            std::cout << "    [-] Could not open stock stream file for " << stock.symbol << "\n";
        }
        lock.unlock();

    }

}


// checks if its time to make aggregates
bool stockmarket::checkAggregateTime(int interval, tm& currentTime) {

    switch (interval) {

    case 0: // 15min
    {
        int currTime = getCurrentIntTime(currentTime);
        int lastTime = getCurrentIntTime(this->last15minAgg);

        if (calcTimeCircle(lastTime, currTime) == 1) {
            this->last15minAgg = currentTime;
           
            return true;
        }
        return false;

    }break;


    case 1: // 30 min
    {
        int currTime = getCurrentIntTime(currentTime);
        int lastTime = getCurrentIntTime(this->last30minAgg);

        if (calcTimeCircle(lastTime, currTime) == 2) {
            this->last30minAgg = currentTime;
            return true;
        }
        return false;

    }break;


    case 2: // 1 hour
    {
        int currTime = getCurrentIntTime(currentTime);
        int lastTime = getCurrentIntTime(this->last1hourAgg);

        if (calcTimeCircle(lastTime, currTime) == 4) {
            this->last1hourAgg = currentTime;
            return true;
        }
        return false;

    }break;


    case 3: // 2 hour
    {
        int currTime = getCurrentIntTime(currentTime);
        int lastTime = getCurrentIntTime(this->last2hourAgg);

        if (calcTimeCircle(lastTime, currTime) == 8) {
            this->last2hourAgg = currentTime;
            return true;
        }
        return false;

    }break;


    case 4: // 4 hour
    {
        int currTime = getCurrentIntTime(currentTime);
        int lastTime = getCurrentIntTime(this->last4hourAgg);

        if (calcTimeCircle(lastTime, currTime) == 16) {
            this->last4hourAgg = currentTime;
            return true;
        }
        return false;

    }break;

    default:
    {
        std::cout << "[ERROR] bruh\n";
        return false;

    }break;

    }

}


// return time in integer format (1545 e.g.)
int stockmarket::getCurrentIntTime(tm currentTime) {
    
    if (currentTime.tm_min < 10) {
        return std::stoi(std::to_string(currentTime.tm_hour) + "0" + std::to_string(currentTime.tm_min));
    }
    else {
        return std::stoi(std::to_string(currentTime.tm_hour) + std::to_string(currentTime.tm_min));
    }
}


// calculates time for 15:30 to 22:00 in a circle
int stockmarket::calcTimeCircle(int lastTime, int currTime) {
    
    int lastTwo = 0;
    int firstTwo = 0;
    int rotations = 1;
    for (rotations; rotations < 17; rotations++) {
        lastTime += 15;

        lastTwo = lastTime % 100;
        firstTwo = lastTime / 100;
        if (lastTwo >= 60) {
            firstTwo++;
            lastTwo = lastTwo - 60;
            lastTime = firstTwo * 100 + lastTwo;
        }

        if (lastTime > 2215) {
            lastTime = 1545 + lastTime - 2215;
        }

        lastTwo = lastTime % 100;
        firstTwo = lastTime / 100;
        if (lastTwo >= 60) {
            firstTwo++;
            lastTwo = lastTwo - 60;
            lastTime = firstTwo * 100 + lastTwo;
        }

        if (lastTime == currTime)
            break;
    }
    return rotations;
}


// appends aggregates 
void stockmarket::appendAggregates(tm& currentTime) {
 
    using namespace ::databaseconnection;

    MYSQL* con = nullptr;
    MYSQL_RES* res = nullptr;
    MYSQL_ROW row = NULL;

    con = mysql_connection_setup();
    tm time = getCurrentTMtime();

    // 15 min
    if (checkAggregateTime(0, currentTime)) {

        std::cout << "\n    [+] Appending aggregates (15min) --> ";
        for (stock& stock : this->allStocks) {
            append15minAgg(stock, con, res, row);
            stock.last15minData.clear();
        }
        tm currentTime2 = getCurrentTMtime();
        std::cout << "[+] Finished  " << getTimeStamp(currentTime2) << "\n";
    }

    // 30 min
    if (checkAggregateTime(1, currentTime)) {

        std::cout << "    [+] Appending aggregates (30min) --> ";
        for (stock& stock : this->allStocks) {
            append30minAgg(stock, con, res, row);
            stock.last30minData.clear();
        }
        tm currentTime2 = getCurrentTMtime();
        std::cout << "[+] Finished  " << getTimeStamp(currentTime2) << "\n";
    }

    // 1 hour
    if (checkAggregateTime(2, currentTime)) {

        std::cout << "    [+] Appending aggregates (1hour) --> ";
        for (stock& stock : this->allStocks) {
            append1hourAgg(stock, con, res, row);
            stock.last1hourdata.clear();
        }
        tm currentTime2 = getCurrentTMtime();
        std::cout << "[+] Finished  " << getTimeStamp(currentTime2) << "\n";
    }

    // 2 hour
    if (checkAggregateTime(3, currentTime)) {

        std::cout << "    [+] Appending aggregates (2hour) --> ";
        for (stock& stock : this->allStocks) {
            append2hourAgg(stock, con, res, row);
            stock.last2hourdata.clear();
        }
        tm currentTime2 = getCurrentTMtime();
        std::cout << "[+] Finished  " << getTimeStamp(currentTime2) << "\n";
    }

    // 4 hour
    if (checkAggregateTime(4, currentTime)) {

        std::cout << "    [+] Appending aggregates (4hour) --> ";
        for (stock& stock : this->allStocks) {
            append4hourAgg(stock, con, res, row);
            stock.last4hourdata.clear();
        }
        tm currentTime2 = getCurrentTMtime();
        std::cout << "[+] Finished  " << getTimeStamp(currentTime2) << "\n";
    }  

    mysql_free_result(res);
    mysql_close(con);

}

#pragma warning( push )
#pragma warning( disable : 4996)

// gets the current local time in tm struct
tm stockmarket::getCurrentTMtime() {

    time_t rawTime;
    struct tm* timeinfo;
    time(&rawTime);
    timeinfo = localtime(&rawTime);

    return *timeinfo;
}

#pragma warning( pop )

// gets current local time since epoch in milliseconds
uint64_t stockmarket::timeSinceEpochMillisec() {
    using namespace std::chrono;
    auto s = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    auto newString = std::to_string(s);

    for (int x = 0; x < 4; x++) {
        newString[newString.length() - 1 - x] = 0;
    }

    return std::stoll(newString);
}



bool stockmarket::checkMarketStatus() {

    std::string url = "https://api.polygon.io/v1/marketstatus/now?apiKey=yTkl2Z4b3aDRRtFYhnJvzCkYtwwgcgIS";

    std::string response = cpr::Get(cpr::Url{ url }).text;

    size_t index = response.find("\"market\"");

    if (index != std::string::npos) {
        index += 10;
        std::string answer = "";

        while (response.at(index) != ',') {
            if (std::isalpha(response.at(index))) {
                answer += response.at(index);
            }
            index++;            
        }

        if (answer == "open") {
            return true;
        }
        else {
            return false;
        }
    }

    return false;
}



// WTF
void stockmarket::appendSMAtoFile(double sma, std::string filepath) {

    using namespace boost::filesystem;

    if (!exists(filepath)) {
        std::ofstream s(filepath);
    }

    std::ofstream smaDataFile(filepath, std::ios::app);

    if (smaDataFile.is_open()) {
        
        smaDataFile << sma << "\n";
        smaDataFile.close();
    }
    else {
        tm time = getCurrentTMtime();
        std::cout << getTimeStamp(time) << "[SMA FAILED] " << filepath << "\n";
    }

}



void stockmarket::appendSMAtoStreamFile(std::string streampath, int type, double value) {

    std::ofstream appendStream(streampath, std::ios::app);

    if (appendStream.is_open()) {

        switch (type) {

        case 1:
            appendStream << "SimpleMovingAvgRT(" + std::to_string(value) + ")\n";
            break;

        case 2:
            appendStream << "SimpleMovingAvg15min(" + std::to_string(value) + ")\n";
            break;

        case 3:
            appendStream << "SimpleMovingAvg30min(" + std::to_string(value) + ")\n";
            break;

        case 4:
            appendStream << "SimpleMovingAvg1hour(" + std::to_string(value) + ")\n";
            break;

        case 5:
            appendStream << "SimpleMovingAvg2hour(" + std::to_string(value) + ")\n";
            break;

        case 6:
            appendStream << "SimpleMovingAvg4hour(" + std::to_string(value) + ")\n";
            break;


        }
    }
    else {

        tm time = getCurrentTMtime();
        std::cout << getTimeStamp(time) << "[SMA FAILED STREAM] " << streampath << "\n";

    }

   

}



bool stockmarket::getSimpleMovingAverage(int interval, int ticks, int startingPos, stock& stock) {

    int index = 0;
    int count = 0;
    double mavg = 0;

    switch (interval) {
    
    case 1:
    {
        if (stock.stockData.size() - startingPos >= ticks) {
            for (index = stock.stockData.size() - 1 - startingPos, count = 0; count < ticks; index--, count++) {
                mavg += stock.stockData.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "smaRT.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 1, mavg);

            return true;
        }
        return false;
    } break;


    case 2:
    {
        if (stock.min15.size() - startingPos >= ticks) {
            for (index = stock.min15.size() - 1, count = 0; count < ticks; index--, count++) {
                mavg += stock.min15.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "sma15min.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 2, mavg);

            return true;
        }
        return false;
    } break;


    case 3:
    {
        if (stock.min30.size() - startingPos >= ticks) {
            for (index = stock.min30.size() - 1, count = 0; count < ticks; index--, count++) {
                mavg += stock.min30.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "sma30min.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 3, mavg);

            return true;
        }
        return false;
    } break;


    case 4:
    {
        if (stock.hour1.size() - startingPos >= ticks) {
            for (int index = stock.hour1.size() - 1, count = 0; count < ticks; index--, count++) {
                mavg += stock.hour1.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "smahour1.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 4, mavg);

            return true;
        }
        return false;
    } break;


    case 5:
    {
        if (stock.hour2.size() - startingPos >= ticks) {
            for (int index = stock.hour2.size() - 1, count = 0; count < ticks; index--, count++) {
                mavg += stock.hour2.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "smahour2.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 5, mavg);

            return true;
        }
        return false;
    } break;


    case 6:
    {
        if (stock.hour4.size() - startingPos >= ticks) {
            for (int index = stock.hour4.size() - 1, count = 0; count < ticks; index--, count++) {
                mavg += stock.hour4.at(index).close;
            }
            mavg /= ticks;

            appendSMAtoFile(mavg, std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "smahour4.txt");
            appendSMAtoStreamFile(std::string(this->symbolPath) + stock.symbol + "/" + stock.symbol + "stream.txt", 6, mavg);

            return true;
        }
        return false;
    } break;


    // no dailydata


    }

}


#pragma warning( pop )