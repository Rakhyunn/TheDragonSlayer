#include "Database.h"
#include <iostream>
#include <sstream>

Database::Database() {
	//SQLite3
	if (sqlite3_open("server.db", &db) != SQLITE_OK) {
		cerr << "DB Open Failed\n";
		return;
	}
	else {
		cout << "DB Open Success\n";
	}
	//Users
	const char* createAccTBL = "CREATE TABLE IF NOT EXISTS Accounts"
								"(ID TEXT PRIMARY KEY, "
								"Password TEXT NOT NULL);";
	//Characters
	const char* createCharTBL = "CREATE TABLE IF NOT EXISTS Characters ("
								"Nickname TEXT PRIMARY KEY, "
								"OwnerID TEXT NOT NULL, "
								"Class TEXT DEFAULT 'Warrior', "
								"Level INT DEFAULT 1, "
								"Exp FLOAT DEFAULT 0, "
								"Gold INT DEFAULT 0, "
								"MapName TEXT DEFAULT '', "
								"PosX FLOAT DEFAULT 0.0, "
								"PosY FLOAT DEFAULT 0.0, "
								"PosZ FLOAT DEFAULT 0.0, "
								"Inventory TEXT DEFAULT '', "
								"FOREIGN KEY(OwnerID) REFERENCES Accounts(ID));";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, createAccTBL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
	}
	else {
		cout << "Accounts Table Create Success\n";
	}
	if (sqlite3_exec(db, createCharTBL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
	}
	else {
		cout << "Characters Table Create Success\n";
	}
}

Database::~Database() {
	sqlite3_close(db);
	cout << "DB Closed\n";
}

bool Database::RegisterUser(const string& userID, const string& Password) {
	lock_guard <recursive_mutex> lock(db_mutex);
	string sqlAcc = "INSERT INTO Accounts (ID, Password) VALUES "
					"('" + userID + "', '" + Password + "');";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sqlAcc.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}
	cout << "Register Success\n";
	return true;
}

bool Database::LoginUser(const string& userID, const string& Password) {
	lock_guard <recursive_mutex> lock(db_mutex);
	string sql = "SELECT * FROM Accounts WHERE ID = '" + userID + "' AND Password = '" + Password + "';";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool IsSuccess = sqlite3_step(stmt) == SQLITE_ROW;		
	sqlite3_finalize(stmt);
	return IsSuccess;
}

bool Database::CheckID(const string& userID) {
	lock_guard <recursive_mutex> lock(db_mutex);
	string sql = "SELECT ID FROM Accounts WHERE ID = '" + userID + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool isExists = sqlite3_step(stmt) == SQLITE_ROW;		
	sqlite3_finalize(stmt);
	return isExists;
}

bool Database::CreateCharacter(const string& userID, const string& nickname, const string& charClass)
{
	lock_guard <recursive_mutex> lock(db_mutex);
	if (CheckNickname(nickname)) return false;
	if (GetCharacterCount(userID) >= 3) return false;
	string sqlChar = "INSERT INTO Characters (Nickname, OwnerID, Class) VALUES ('" + nickname + "', '" + userID + "', '" + charClass + "');";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sqlChar.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}
	cout << "Create Character Success\n";
	return true;
}

bool Database::CheckNickname(const string& Nickname) {
	lock_guard <recursive_mutex> lock(db_mutex);
	string sql = "SELECT Nickname FROM Characters WHERE Nickname = '" + Nickname + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool isExists = sqlite3_step(stmt) == SQLITE_ROW;
	sqlite3_finalize(stmt);
	return isExists;
}

int Database::GetCharacterCount(const string& userID) {
	lock_guard<recursive_mutex> lock(db_mutex);

	string sql = "SELECT COUNT(*) FROM Characters WHERE OwnerID = '" + userID + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	int count = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW)
		count = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	return count;
}

vector<CharacterInfo> Database::GetCharacterList(const string& userID) {
	lock_guard<recursive_mutex> lock(db_mutex);
	vector<CharacterInfo> list;
	string sql = "SELECT Nickname, Level, Class FROM Characters WHERE OwnerID = '" + userID + "'";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return list;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		CharacterInfo info;
		const unsigned char* nick = sqlite3_column_text(stmt, 0);
		const unsigned char* cls = sqlite3_column_text(stmt, 2);

		info.nickname = nick ? string((char*)nick)  : "";
		info.level = sqlite3_column_int(stmt, 1);
		info.charClass = cls ? string((char*)cls) : "Warrior";
		list.push_back(info);
	}
	sqlite3_finalize(stmt);
	return list;
}

bool Database::SaveUserData(const string& id, const string& nickname, int level, float exp, int gold, const string& mapName, float x, float y, float z, const string& inventory) {
	lock_guard<recursive_mutex> lock(db_mutex);
	string sql = "UPDATE Characters SET "
		"Level = " + to_string(level) + ", "
		"Exp = " + to_string(exp) + ", "
		"Gold = " + to_string(gold) + ", "
		"MapName = '" + mapName + "', "
		"PosX = " + to_string(x) + ", "
		"PosY = " + to_string(y) + ", "
		"PosZ = " + to_string(z) + ", "
		"Inventory = '" + inventory + "' "
		"WHERE OwnerID = '" + id + "' AND Nickname = '" + nickname + "';";
	char* errMsg = nullptr;
	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
		cerr << "Save Error: " << errMsg << "\n";
		sqlite3_free(errMsg);
		return false;
	}
	return true;
}

string Database::LoadUserData(const string& id, const string& nickname)
{
	lock_guard<recursive_mutex> lock(db_mutex);
	string sql = "SELECT Level, Exp, Gold, MapName, PosX, PosY, PosZ, Inventory, Class FROM Characters WHERE OwnerID = '" + id + "' AND Nickname = '" + nickname + "';";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return string();
	string result = "";
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		// 공백으로 구분하여 하나의 긴 문자열로 만듦
		int level = sqlite3_column_int(stmt, 0);
		double exp = sqlite3_column_double(stmt, 1);
		int gold = sqlite3_column_int(stmt, 2);
		const unsigned char* map = sqlite3_column_text(stmt, 3);
		double x = sqlite3_column_double(stmt, 4);
		double y = sqlite3_column_double(stmt, 5);
		double z = sqlite3_column_double(stmt, 6);
		const unsigned char* inv = sqlite3_column_text(stmt, 7);
		const unsigned char* cls = sqlite3_column_text(stmt, 8);
		string mapName = map ? string((char*)map) : "";
		string inventory = inv ? string((char*)inv) : "[]";
		string charClass = cls ? string((char*)cls) : "Warrior";
		if (mapName.empty()) mapName = "NEW";
		ostringstream oss;
		oss << level << " "
			<< exp << " "
			<< gold << " "
			<< mapName << " "
			<< x << " "
			<< y << " "
			<< z << " "
			<< charClass << " "
			<< inventory;
		result = oss.str();
	}
	sqlite3_finalize(stmt);
	return result;
}
