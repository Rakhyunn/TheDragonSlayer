#include "Database.h"
#include <iostream>

Database::Database() {
	//SQLite3
	if (sqlite3_open("server.db", &db) != SQLITE_OK) {
		cerr << "DB Open Failed\n";
	}
	else {
		cout << "DB Open Success\n";
	}
	//Users
	const char* createTBLSql = "CREATE TABLE IF NOT EXISTS Users"
								"(ID TEXT PRIMARY KEY, "
								"Password TEXT NOT NULL, "
								"Nickname TEXT UNIQUE NOT NULL);";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, createTBLSql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
	}
	else {
		cout << "Table Create Success\n";
	}
}

Database::~Database() {
	sqlite3_close(db);
}

bool Database::RegisterUser(const string& userID, const string& Password, const string& Nickname) {
	lock_guard <mutex> lock(db_mutex);
	string sql = "INSERT INTO Users (ID, Password, Nickname) VALUES "
				"('" + userID + "', '" + Password + "', '" + Nickname + "');";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}
	cout << "Table Create Success\n";
	return true;
}

bool Database::LoginUser(const string& userID, const string& Password) {
	lock_guard <mutex> lock(db_mutex);
	string sql = "SELECT * FROM Users WHERE ID = '" + userID + "' AND Password = '" + Password + "';";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool IsSuccess = sqlite3_step(stmt) == SQLITE_ROW;		
	sqlite3_finalize(stmt);
	return IsSuccess;
}

bool Database::CheckID(const string& userID) {
	lock_guard <mutex> lock(db_mutex);
	string sql = "SELECT ID FROM USERS WHERE ID = '" + userID + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool isExists = sqlite3_step(stmt) == SQLITE_ROW;		
	sqlite3_finalize(stmt);
	return isExists;
}

bool Database::CheckNickname(const string& Nickname) {
	lock_guard <mutex> lock(db_mutex);
	string sql = "SELECT Nickname FROM USERS WHERE Nickname = '" + Nickname + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool isExists = sqlite3_step(stmt) == SQLITE_ROW;		
	sqlite3_finalize(stmt);
	return isExists;
}
