
#pragma once
#pragma warning( push )
#pragma warning( disable : 4101)

#ifndef AGGREGATE_H
#define AGGREGATE_H

#include <iostream>

struct aggregate {

    double high = 0;
    double low = 0;
    double open = 0;
    double close = 0;

    static inline aggregate getAggregateStruct(std::string& dataLine);
};

aggregate aggregate::getAggregateStruct(std::string& dataLine) {

    aggregate data;

    std::string value = "";
    int valueType = 0;
    for (char& character : dataLine) {
        if (character == ',') {
            switch (valueType) {

            case 0:
            {
                data.open = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 1:
            {
                data.close = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 2:
            {
                data.high = std::stod(value);
                value = "";
                valueType++;
            } break;

            case 3:
            {
                data.low = std::stod(value);
                value = "";
                return data;
                valueType++;

            } break;

            default:
            {
                std::cout << "[ERROR] aggregate bruh";
            } break;

            }

        }
        else {
            value += character;
        }
    }

    return data;
}




#endif // !AGGREGATES_H
