#pragma once

#ifndef DAILYDATA_H
#define DAILYDATA_H

#include <iostream>


struct dailydata {

	double afterHours = -1;
	double close = -1;
	std::string from = "EMPTY";
	double high = -1;
	double low = -1;
	double open = -1;
	double preMarket = -1;
	long long int volume = -1;


	static inline dailydata getDailyDataStruct(std::string& data);

};


dailydata dailydata::getDailyDataStruct(std::string& data) {

	dailydata ddata;
	std::string value = "";

	// AFTERHOURS
	size_t index = data.find("afterHours");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.afterHours = std::stod(value);
		value = "";
	}


	// CLOSE
	index = data.find("close");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.close = std::stod(value);
		value = "";
	}


	// FROM
	index = data.find("from");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index))) {
				value += data.at(index);
			}
			index++;
		}
		ddata.from = value;
		value = "";
	}


	// HIGH
	index = data.find("high");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.high = std::stod(value);
		value = "";
	}


	// LOW
	index = data.find("low");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.low = std::stod(value);
		value = "";
	}


	// OPEN
	index = data.find("open");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.open = std::stod(value);
		value = "";
	}


	// PREMARKET
	index = data.find("preMarket");
	if (index != std::string::npos) {
		while (data.at(index) != ',' && data.at(index) != '}') {
			if (std::isdigit(data.at(index)) || data.at(index) == '.') {
				value += data.at(index);
			}
			index++;
		}
		ddata.preMarket = std::stod(value);
		value = "";
	}


	// VOLUME
	index = data.find("volume");
	if (index != std::string::npos) {
		index += 7;
		while (data.at(index) != ',' && data.at(index) != '}') {

			if (std::isdigit(data.at(index)) || data.at(index) == '+' || data.at(index) == 'e' || data.at(index) == '.') {
				value += data.at(index);
			}

			index++;
		}

		std::string actualValue = "";
		int iter = 0;
		try {
			while ((value.at(iter) != 'e' && value.at(iter) != '}' && value.at(iter) != ',') && (std::isdigit(value.at(iter)) || value.at(iter) == '.')) {
				actualValue += value.at(iter);
				iter++;
			}
		}
		catch (std::exception) {

		}

		size_t scientificIndex = value.find('+');
		if (scientificIndex != std::string::npos) {

			scientificIndex += 1;

			std::string scientificValue = "";
			try {
				while (std::isdigit(value.at(scientificIndex))) {
					scientificValue += value.at(scientificIndex);
					scientificIndex++;
				}
			}
			catch (std::exception) {

			}

			int scientificValueINT = std::stoi(scientificValue) + 1;
			std::string volumeValue = "";

			for (int x = 0; x < scientificValueINT; x++) {

				try {
					if (std::isdigit(actualValue.at(x))) {
						volumeValue += actualValue.at(x);
					}
					else {
						scientificValueINT += 1;
					}
				}
				catch (std::out_of_range) {
					volumeValue += '0';
				}
			}
			ddata.volume = std::stoll(volumeValue);

		}
		else {
			ddata.volume = std::stoll(value);
		}

	}

	return ddata;
}


#endif // !DAILYDATA_H

