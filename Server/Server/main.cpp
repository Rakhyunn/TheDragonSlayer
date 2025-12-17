#include "Server.h"
#include <iostream>

using namespace std;

int main() {
	try{
		boost::asio::io_context io_context;
		Server server(io_context, 6000);

		cout << "Server Waiting Response...\n";
		io_context.run();
	}
	catch (exception& e) {
		cerr << "Server Error: " << e.what() << "\n";
	}
	return 0;
}
