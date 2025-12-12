#pragma once
#include <boost/asio.hpp>
#include "Database.h"
#include <set>
#include <mutex>

using boost::asio::ip::tcp;
class Session;

class Server
{
public:
	Server(boost::asio::io_context& io_context, int port);
	bool IsUserLoggedIn(const string& userID);
	void AddLoggedInUser(const string& userID);
	void RemoveLoggedInUser(const string& userID);

private:
	void StartAccept();
	void HandleAccept(shared_ptr<Session> session, const boost::system::error_code& error);

	tcp::acceptor acceptor_;
	Database db_;
	set<string> LoggedInUsers_;
	mutex LoggedInMutex_;
};

