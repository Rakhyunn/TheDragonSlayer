#pragma once
#include <sqlite3.h>
#include <string>
#include <mutex>

using namespace std;

class Database {
public:
	Database();
	~Database();

	bool RegisterUser(const string& userID, const string& Password, const string& Nickname);
	bool LoginUser(const string& userID, const string& Password);
	bool CheckID(const string& userID);
	bool CheckNickname(const string& Nickname);

private:
	sqlite3* db;
	mutex db_mutex;
};

