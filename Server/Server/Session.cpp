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
					return;
				}
				string command = tokens[0];
				if (command == "REGISTER") {
					if (tokens.size() < 3) {
						cout << "Can't Solve Command\n";
						WriteMessage("Can't Solve Command");
						return;
					}
					string userID = tokens[1], password = tokens[2];
					if (db_.RegisterUser(userID, password)) {
						if (db_.CreateCharacter(userID, userID)) {
							cout << "Register & Create Character Success: " << userID << "\n";
							WriteMessage("Register Success");
						}
						else {
							cout << "Register Success But Character Create Failed\n";
							WriteMessage("Register Partial Success");
						}
					}
					else {
						cout << "Register Failed\n";
						WriteMessage("Register Failed");
					}
				}
				else if (command == "LOGIN") {
					string userID = tokens[1], password = tokens[2];
					if (server_.IsUserLoggedIn(userID)) {
						cout << "Login Failed: Already Logged In\n";
						WriteMessage("Login Failed: Already Logged In");
					}
					else if (db_.LoginUser(userID, password)) {
						server_.AddLoggedInUser(userID);
						LoggedInUserID_ = userID;
						CurrentCharacter_ = userID;	// 임시로 닉네임의 ID로 설정
						cout << "Login Success: " << CurrentCharacter_ << "\n";
						WriteMessage("Login Success " + userID);
					}
					else {
						cout << "Login Failed\n";
						WriteMessage("Login Failed");
					}
				}
				else if (command == "CHECKID") {
					string userID = tokens[1];
					if (db_.CheckID(userID)) {
						cout << "ID Use Impossible\n";
						WriteMessage("ID Use Impossible");
					}
					else {
						cout << "ID Use Possible\n";
						WriteMessage("ID Use Possible");
					}
				}
				else if (command == "CREATECHARACTER") {
					string userID = tokens[1], nickname = tokens[2];
					if (db_.CheckNickname(nickname)) {
						WriteMessage("Create Failed: Nickname can't Use");
					}
					else if (db_.CreateCharacter(userID, nickname)) {
						cout << "Character Create Success\n";
						WriteMessage("Character Create Success");
					}
					else {
						cout << "Nickname Use Possible\n";
						WriteMessage("Nickname Use Possible");
					}
				}
				else if (command == "CHECKNICK") {
					string nickname = tokens[1];
					if (db_.CheckNickname(nickname)) {
						cout << "Nickname Use Impossible\n";
						WriteMessage("Nickname Use Impossible");
					}
					else {
						cout << "Nickname Use Possible\n";
						WriteMessage("Nickname Use Possible");
					}
				}
				else if (command == "SAVEUSER") {
					if (tokens.size() >= 9) {
						string id = LoggedInUserID_;
						string nickname = CurrentCharacter_;
						int level = stoi(tokens[3]);
						int exp = stoi(tokens[4]);
						int gold = stoi(tokens[5]);
						string map = tokens[6];
						float x = stof(tokens[7]);
						float y = stof(tokens[8]);
						float z = stof(tokens[9]);
						string inventory = "";
						for (size_t i = 10; i < tokens.size(); i++) {
							if (i > 10) inventory += " ";
							inventory += tokens[i];
						}
						if (db_.SaveUserData(id, nickname, level, exp, gold, map, x, y, z, inventory)) {
							WriteMessage("Save Success");
							cout << "Saved User: " << id << "\n";
						}
						else {
							WriteMessage("Save Failed");
						}
					}
				}
				else if (command == "LOADUSER") {
					if (tokens.size() >= 3) {
						string id = tokens[1];
						string nickname = tokens[2];
						string data = db_.LoadUserData(id, nickname);
						if (!data.empty()) {
							WriteMessage("LoadData " + id + " " + data);
							cout << "Loaded User: " << id << "\n";
						}
						else {
							WriteMessage("Load Fail");
						}
					}
				}
				else {
					cout << "Invalid Command\n";
					WriteMessage("Invalid Command");
				}
				ReadMessage();
			}
			else {
				if (!LoggedInUserID_.empty()) {
					server_.RemoveLoggedInUser(LoggedInUserID_);
					cout << "User " << LoggedInUserID_ << " Logged Out\n";
				}
				cout << "Client Disconnected: " << ec.message() << "\n";
			}
		});
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
