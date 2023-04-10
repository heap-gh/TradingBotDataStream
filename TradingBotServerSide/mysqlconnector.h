#pragma once
#pragma once

#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#pragma warning( push )
#pragma warning( disable : 4996)

#include <mysql/mysql.h>
#include <iostream>

namespace databaseconnection {

    inline std::string getTimeStamp();
    inline tm getCurrentTMtime();


    inline MYSQL* mysql_connection_setup() {
        MYSQL* connection = mysql_init(NULL); // mysql instance

        //connect database
        if (!mysql_real_connect(connection, "141.98.26.4", "lotusalg_tradingdata", "ioksejhf343", "lotusalg_stream_data", 0, NULL, 0)) {
            std::cout << getTimeStamp() << "[MYSQL] Connection Error: " << mysql_error(connection) << std::endl;
        }

        return connection;
    }

    // mysql_res = mysql result
    inline MYSQL_RES* mysql_perform_query(MYSQL* connection, std::string sql_query, bool& fail) {
        //send query to db
        if (mysql_query(connection, sql_query.c_str())) {
            std::cout << getTimeStamp() << "[MYSQL] MySQL Query Error: " << mysql_error(connection) << std::endl;
            
            if (std::string(mysql_error(connection)).find("doesn't exist") != std::string::npos)
                fail = true;
        }

        return mysql_use_result(connection);
    }


    std::string getTimeStamp() {

        tm time = databaseconnection::getCurrentTMtime();

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

    tm getCurrentTMtime() {

        time_t rawTime;
        struct tm* timeinfo;
        time(&rawTime);
        timeinfo = localtime(&rawTime);

        return *timeinfo;
    }

}

#pragma warning( pop )

#endif // !DATABASECONNECTION_H