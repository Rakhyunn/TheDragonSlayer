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
				/*cout << "Client Request (Hex): ";
				for (char c : message) {
					cout << std::hex << (int)(unsigned char)c << " ";
				}
				cout << endl;*/
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
				string userID, password, nickname;
				if (tokens.size() == 2) {
					if (command == "CHECKID") {
						userID = tokens[1];
						password = "";
						nickname = "";
					}
					else if (command == "CHECKNICK") {
						nickname = tokens[1];
						userID = "";
						password = "";
					}
				}
				else if (tokens.size() >= 3) {
					userID = tokens[1];
					password = tokens[2];
					nickname = (tokens.size() > 3) ? tokens[3] : "";
				}
				cout << command << "/" << userID << "/" << password << "/" << nickname << "/\n";
				if (command == "REGISTER") {
					if (tokens.size() < 4) {
						cout << "Can't Solve Command\n";
						WriteMessage("Can't Solve Command");
						return;
					}
					if (db_.RegisterUser(userID, password, nickname)) {
						cout << "Register Success\n";
						WriteMessage("Register Success");
					}
					else {
						cout << "Register Failed\n";
						WriteMessage("Register Failed");
					}
				}
				else if (command == "LOGIN") {
					if (server_.IsUserLoggedIn(userID)) {
						cout << "Login Failed: Already Logged In\n";
						WriteMessage("Login Failed: Already Logged In");
					}
					else if (db_.LoginUser(userID, password)) {
						server_.AddLoggedInUser(userID);
						LoggedInUserID_ = userID;
						cout << "Login Success\n";
						WriteMessage("Login Success");
					}
					else {
						cout << "Login Failed\n";
						WriteMessage("Login Failed");
					}
				}
				else if (command == "CHECKID") {
					if (db_.CheckID(userID)) {
						cout << "ID Use Impossible\n";
						WriteMessage("ID Use Impossible");
					}
					else {
						cout << "ID Use Possible\n";
						WriteMessage("ID Use Possible");
					}
				}
				else if (command == "CHECKNICK") {
					if (db_.CheckNickname(nickname)) {
						cout << "Nickname Use Impossible\n";
						WriteMessage("Nickname Use Impossible");
					}
					else {
						cout << "Nickname Use Possible\n";
						WriteMessage("Nickname Use Possible");
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
