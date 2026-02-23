#include "Session.h"
#include <iostream>
#include <sstream>
#include "Server.h"

Session::Session(tcp::socket socket, Database& db, Server& server)
	: socket_(move(socket)), db_(db), server_(server) {
	fill(data_.begin(), data_.end(), 0);
}

void Session::Start() {
	ReadMessage();
}

void Session::ReadMessage() {
	fill(data_.begin(), data_.end(), 0);
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_), 
		[this, self](boost::system::error_code ec, size_t length) {
			if (!ec) {
				string message(data_.data(), length);
				cout << "Client Request: " << message << "\n";
				if (message.empty()) {
					cerr << "Empty Message!\n";
					WriteMessage("Empty Request");
					ReadMessage();
					return;
				}
				istringstream iss(message);
				vector<string> tokens;
				string token;
				while (iss >> token) {
					tokens.push_back(token);
				}
				if (tokens.size() < 1) {
					WriteMessage("Too Short Request");
					ReadMessage();
					return;
				}
				HandleCommand(tokens);
				ReadMessage();
			}
			else {
				if (!LoggedInUserID_.empty()) {
					server_.RemoveLoggedInUser(LoggedInUserID_);
					cout << "User " << LoggedInUserID_ << " Logged Out (Disconnected)\n";
				}
				cout << "Client Disconnected: " << ec.message() << "\n";
			}
		});
}

void Session::HandleCommand(const vector<string>& tokens) {
	const string& cmd = tokens[0];

	// 회원가입
	if (cmd == "REGISTER") {
		if (tokens.size() < 3) { WriteMessage("Too Short Message"); return; }
		const string& userID = tokens[1];
		const string& password = tokens[2];
		if (db_.CheckID(userID)) {
			WriteMessage("Register Failed: ID Already Exists");
			return;
		}
		// 추후 캐릭터 선택 구현 시 삭제
		if (db_.RegisterUser(userID, password)) {
			WriteMessage("Register Success");
		}
		else {
			WriteMessage("Register Failed");
		}
	}

	// 로그인
	else if (cmd == "LOGIN") {
		if (tokens.size() < 3) { WriteMessage("Too Short Message"); return; }
		const string& userID = tokens[1];
		const string& password = tokens[2];
		if (server_.IsUserLoggedIn(userID)) {
			WriteMessage("Login Failed: Already Logged In");
			return;
		}
		if (db_.LoginUser(userID, password)) {
			server_.AddLoggedInUser(userID);
			LoggedInUserID_ = userID;
			cout << "Login: " << userID << "\n";
			WriteMessage("Login Success " + userID);
		}
		else {
			WriteMessage("Login Failed");
		}
	}

	// ID 중복확인
	else if (cmd == "CHECKID") {
		if (tokens.size() < 2) { WriteMessage("Too Short Message"); return; }
		WriteMessage(db_.CheckID(tokens[1]) ? "ID Use Impossible" : "ID Use Possible");
	}

	// 닉네임 중복확인
	else if (cmd == "CHECKNICK") {
		if (tokens.size() < 2) { WriteMessage("Too Short Message"); return; }
		WriteMessage(db_.CheckNickname(tokens[1]) ? "Nickname Use Impossible" : "Nickname Use Possible");
	}

	// 보유 캐릭터 리스트
	else if (cmd == "GETCHARACTERLIST") {
		if (LoggedInUserID_.empty()) {
			WriteMessage("ID Not Exists in Server");
			return;
		}
		auto list = db_.GetCharacterList(LoggedInUserID_);
		ostringstream oss;
		oss << "CharacterList " << list.size();
		for (const auto& c : list)
			oss << " " << c.nickname << " " << c.level << " " << c.charClass;
		WriteMessage(oss.str());
		cout << "CharacterList sent to " << LoggedInUserID_ << "\n";
	}

	// 캐릭터 생성
	else if (cmd == "CREATECHARACTER") {
		if (LoggedInUserID_.empty()) {
			WriteMessage("ID Not Exists in Server");
			return;
		}
		if (tokens.size() < 3) {
			WriteMessage("Too Short Message");
			return;
		}
		const string& nickname = tokens[1];
		const string& charClass = tokens[2];
		if (db_.CheckNickname(nickname)) {
			WriteMessage("Create Failed: Nickname Use Impossible");
			return;
		}
		if (db_.CreateCharacter(LoggedInUserID_, nickname, charClass)) {
			WriteMessage("Character Create Success " + nickname);
		}
		else {
			WriteMessage("Create Failed");
		}
	}

	// 데이터 저장
	else if (cmd == "SAVEUSER") {
		if (LoggedInUserID_.empty() || CurrentCharacter_.empty()) {
			WriteMessage("ID or Character Not Exists in Server"); return;
		}
		if (tokens.size() < 9) {
			WriteMessage("Too Short Message"); return;
		}
		try {
			int   level = stoi(tokens[1]);
			float exp = stof(tokens[2]);
			int   gold = stoi(tokens[3]);
			const string& mapName = tokens[4];
			float x = stof(tokens[5]);
			float y = stof(tokens[6]);
			float z = stof(tokens[7]);
			// 인벤토리는 나머지 토큰을 공백으로 합침 (JSON 배열 등)
			string inventory;
			for (size_t i = 8; i < tokens.size(); i++) {
				if (i > 8) inventory += " ";
				inventory += tokens[i];
			}
			if (db_.SaveUserData(LoggedInUserID_, CurrentCharacter_, level, exp, gold, mapName, x, y, z, inventory)) {
				WriteMessage("Save Success");
			}
			else {
				WriteMessage("Save Failed");
			}
		}
		catch (const exception& e) {
			cerr << "SAVEUSER parse error: " << e.what() << "\n";
			WriteMessage("Save parse Failed");
		}
	}

	// 캐릭터 선택
	else if (cmd == "SELECTCHARACTER") {
		if (LoggedInUserID_.empty()) {
			WriteMessage("ID Not Exsits in Server"); return;
		}
		if (tokens.size() < 2) {
			WriteMessage("Too Short Message"); return;
		}
		const string& nickname = tokens[1];
		string data = db_.LoadUserData(LoggedInUserID_, nickname);
		if (!data.empty()) {
			CurrentCharacter_ = nickname;
			cout << "SelectCharacter: " << LoggedInUserID_ << " -> " << nickname << "\n";
			WriteMessage("LoadData " + nickname + " " + data);
		}
		else {
			WriteMessage("Load Fail: Character Not Found");
		}
	}

	// 데이터 로드
	else if (cmd == "LOADUSER") {
		if (tokens.size() < 3) { 
			WriteMessage("Too Short Message"); return;
		}
		const string& id = tokens[1];
		const string& nickname = tokens[2];
		string data = db_.LoadUserData(id, nickname);
		if (!data.empty())
			WriteMessage("LoadData " + nickname + " " + data);
		else
			WriteMessage("Load Fail");
	}

	else {
		cout << "Unknown Command: " << tokens[0] << "\n";
		WriteMessage("Invalid Command");
	}
}

void Session::WriteMessage(string message) {
	auto self(shared_from_this());
	auto sendBuffer = make_shared<string>(message);
	boost::asio::async_write(socket_, boost::asio::buffer(/*message*/*sendBuffer),
		[this, self, sendBuffer](boost::system::error_code ec, size_t) {
			if (ec) {
				cerr << "Error: " << ec.message() << "\n";
			}
	});
}
