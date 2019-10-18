#pragma once
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <string>
#include <vector>
#define MAX_VARCHAR_SIZE 4096
#define VARCHAR_SIZE 50

class SQL {
	std::string driverName, server, port, database, id, pwd;
protected:
	SQLHANDLE env, con, command;
	SQLINTEGER ParamsProcessed[VARCHAR_SIZE];
	SQLUSMALLINT i, ParamStatusArray[VARCHAR_SIZE];
public:
	SQL(std::string _driverName, std::string _server, std::string _port, std::string _database, std::string _id, std::string _pwd);
	~SQL();
	void executeUpdate(std::vector<std::string> queryStrings);
	std::vector<std::vector<std::string>> executeQuery(std::string _query, int column);
};