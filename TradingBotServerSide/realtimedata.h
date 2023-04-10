#pragma once

#ifndef REALTIMEDATA_H
#define REALTIMEDATA_H

#include <string>

struct realtimedata {


	long long int volume = 0;
	long long int accumulated_volume = 0;
	double vwap = 0;
	double open = 0;
	double close = 0;
	double high = 0;
	double low = 0;
	double aggregate_vwap = 0;
	double average_size = 0;
	long long int start_timestamp = 0;
	long long int end_timestamp = 0;

	static inline realtimedata getRealTimeDataStruct(std::string& data);
    static inline std::string getSymbol(std::string& data);
};



std::string realtimedata::getSymbol(std::string& data) {

    std::string symbol = "";
    size_t index = 0;
    index = data.find("symbol=");

    if (index != std::string::npos) {

        int y = 0;
        while (data[index + y] != '=') {
            y++;
        }
        y += 2;
        while (isalpha(data[index + y])) {
            symbol += data[index + y];
            y++;
        }
    }

    return symbol;
}



realtimedata realtimedata::getRealTimeDataStruct(std::string& dataLine) {

    std::vector <std::string> values = { "volume=", "accumulated_volume=", "vwap=", "open=", "close=", "high=", "low=", "aggregate_vwap=", "average_size=", "start_timestamp=", "end_timestamp=" };

    int count = 0;

    realtimedata dataStruct;
    for (std::string value : values) {
        size_t x = dataLine.find(value);
        std::string unit = "";
        while (dataLine[x] != '=') {
            x++;
        }
        x++;
        while (dataLine[x] != ',') {
            unit += dataLine[x];
            x++;
        }

        switch (count) {

        case 0:
            dataStruct.volume = std::stoll(unit);
            count++;
            break;

        case 1:
            dataStruct.accumulated_volume = std::stoll(unit);
            count++;
            break;

        case 2:
            dataStruct.vwap = std::stod(unit);
            count++;
            break;

        case 3:
            dataStruct.open = std::stod(unit);
            count++;
            break;

        case 4:
            dataStruct.close = std::stod(unit);
            count++;
            break;

        case 5:
            dataStruct.high = std::stod(unit);
            count++;
            break;

        case 6:
            dataStruct.low = std::stod(unit);
            count++;
            break;

        case 7:
            dataStruct.aggregate_vwap = std::stod(unit);
            count++;
            break;

        case 8:
            dataStruct.average_size = std::stod(unit);
            count++;
            break;

        case 9:
            dataStruct.start_timestamp = std::stoll(unit);
            count++;
            break;

        case 10:
            dataStruct.end_timestamp = std::stoll(unit);
            break;
        }
    }

    return dataStruct;
}

#endif // !REALTIMEDATA_H
