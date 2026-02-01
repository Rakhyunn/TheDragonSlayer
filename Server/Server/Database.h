#pragma once
#include <sqlite3.h>
#include <string>
#include <mutex>

using namespace std;

class Database {
public:
	Database();
	~Database();
	// 로그인 및 회원가입 함수
	bool RegisterUser(const string& userID, const string& Password);
	bool LoginUser(const string& userID, const string& Password);
	bool CheckID(const string& userID);
	// 캐릭터 생성 함수
	bool CreateCharacter(const string& userID, const string& nickname);
	bool CheckNickname(const string& Nickname);
	// 저장 및 로드 함수
	bool SaveUserData(const string& id, const string& nickname, int level, int exp, int gold, const string& mapName, float x, float y, float z, const string& inventory);
	string LoadUserData(const string& id, const string& nickname);

private:
	sqlite3* db;
	recursive_mutex db_mutex;
};

