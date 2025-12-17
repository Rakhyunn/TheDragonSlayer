#pragma once
#include <boost/asio.hpp>
#include <memory>
#include "Database.h"

using boost::asio::ip::tcp;
using namespace std;
class Server;

class Session : public enable_shared_from_this<Session>
{
public:
	explicit Session(tcp::socket socket, Database& db, Server& server);
	void Start();
	tcp::socket& GetSocket() { return socket_; }

private:
	void ReadMessage();
	void WriteMessage(string message);

	tcp::socket socket_;
	Database& db_;
	//char data_[1024];
	array<char, 1024> data_;
	Server& server_;
	string LoggedInUserID_;
};

