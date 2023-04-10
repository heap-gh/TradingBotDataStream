#pragma once

#ifndef STOCK_H
#define STOCK_H

#include <vector>

#include "realtimedata.h"
#include "aggregate.h"


class stock {


public:
	
	std::string symbol;
	std::string fullName;
	std::vector <realtimedata> stockData;
	
	// aggregates
	std::vector <aggregate> min15;
	std::vector <aggregate> min30;
	std::vector <aggregate> hour1;
	std::vector <aggregate> hour2;
	std::vector <aggregate> hour4;
	
	// streamdata
	std::vector <realtimedata> last15minData;
	std::vector <realtimedata> last30minData;
	std::vector <realtimedata> last1hourdata;
	std::vector <realtimedata> last2hourdata;
	std::vector <realtimedata> last4hourdata;
	size_t index;


public:

	stock();

};


#endif // !STOCK_H
