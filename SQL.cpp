#include "SQL.h"
#include <iostream>

SQL::SQL(
	std::string _driverName, 
	std::string _server, 
	std::string _port,
	std::string _database, 
	std::string _id, 
	std::string _pwd) : 
	driverName(_driverName), 
	server(_server), 
	port(_port), 
	database(_database),
	id(_id), pwd(_pwd) {
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->env);
	SQLSetEnvAttr(this->env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
	SQLAllocHandle(SQL_HANDLE_DBC, this->env, &this->con);
	std::string s("DRIVER={" + this->driverName + "};SERVER=" + this->server + ";PORT_NO=" + this->port + "; DATABASE=" + this->database + "; UID=" + this->id + "; PWD=" + this->pwd + ";");
	SQLDriverConnect(this->con, NULL, (SQLCHAR*)s.c_str(), SQL_NTS, NULL, 1024, NULL, SQL_DRIVER_NOPROMPT);
	SQLSetConnectAttr(this->con, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_IS_UINTEGER);
	SQLAllocStmt(this->con, &this->command);

	SQLSetStmtAttr(this->command, SQL_ATTR_PARAM_BIND_TYPE, SQL_PARAM_BIND_BY_COLUMN, NULL);
	SQLSetStmtAttr(this->command, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)VARCHAR_SIZE, NULL);
	SQLSetStmtAttr(this->command, SQL_ATTR_PARAM_STATUS_PTR, this->ParamStatusArray, NULL);
	SQLSetStmtAttr(this->command, SQL_ATTR_PARAMS_PROCESSED_PTR, this->ParamsProcessed, SQL_NTS);
}

SQL::~SQL() {
	SQLFreeHandle(SQL_HANDLE_STMT, this->command);
	SQLDisconnect(this->con);
	SQLFreeHandle(SQL_HANDLE_DBC, this->con);
	SQLFreeHandle(SQL_HANDLE_ENV, this->env);
}

void SQL::executeUpdate(std::vector<std::string> queryStrings) {
	for (std::string _query : queryStrings) {
		SQLCHAR* query((SQLCHAR*)_query.c_str());
		SQLRETURN ret(SQLExecDirect(this->command, query, SQL_NTS));
	}
	SQLCloseCursor(this->command);
}

std::vector<std::vector<std::string>> SQL::executeQuery(std::string _query, int column) {
	std::vector<std::vector<std::string>> data;
	SQLCHAR* query((SQLCHAR*)_query.c_str());
	SQLExecDirect(this->command, query, SQL_NTS);
	int step(1);
	while (true) {
		std::vector<std::string> _data;
		SQLCHAR** item(new SQLCHAR*[column]);
		SQLLEN* Citem(new SQLLEN[column]);
		for (int i(0); i < column; i++) {
			item[i] = new SQLCHAR[VARCHAR_SIZE];
			SQLBindCol(this->command, i + 1, SQL_C_CHAR, item[i], VARCHAR_SIZE, &Citem[i]);
		}
		SQLRETURN ret(SQLFetch(this->command));
		if (ret == SQL_NO_DATA || ret == SQL_INVALID_HANDLE) break;
		else {
			for (int i(0); i < column; i++) {
				_data.push_back((const char*)item[i]);
				delete[] item[i];
				item[i] = nullptr;
			}
			delete[] item, Citem;
			item = nullptr;
			Citem = nullptr;
			data.push_back(_data);
		}
	}
	SQLCloseCursor(this->command);
	return data;
}