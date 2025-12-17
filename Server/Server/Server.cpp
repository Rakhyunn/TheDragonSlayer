#include "Server.h"
#include <iostream>
#include <string>
#include <boost/asio/ts/io_context.hpp>
#include "Session.h"

using namespace std;

Server::Server(boost::asio::io_context& io_context, int port) 
	: acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), db_() {
	StartAccept();
}

bool Server::IsUserLoggedIn(const string& userID)
{
	lock_guard<mutex> lock(LoggedInMutex_);
	return LoggedInUsers_.count(userID) > 0;
}

void Server::AddLoggedInUser(const string& userID)
{
	lock_guard<mutex> lock(LoggedInMutex_);
	LoggedInUsers_.insert(userID);
}

void Server::RemoveLoggedInUser(const string& userID)
{
	lock_guard<mutex> lock(LoggedInMutex_);
	LoggedInUsers_.erase(userID);
}

void Server::StartAccept() {
	auto session = make_shared<Session>(tcp::socket(acceptor_.get_executor()), db_, *this);
	acceptor_.async_accept(session->GetSocket(), [this, session](const boost::system::error_code& error) {
		HandleAccept(session, error);
	});
}

void Server::HandleAccept(shared_ptr<Session> session, const boost::system::error_code& error) {
	if (!error) {
		cout << "Client connected!" << "\n";
		session->Start();
	}
	StartAccept();
}