#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
#include <cmath>
#include <thread>
using namespace boost::asio;
int main()
{
	io_service ioservice;
	steady_timer timer1{ioservice, std::chrono::seconds{3}};
	timer1.async_wait([](const boost::system::error_code &ec)
					 {std::cerr<<"3 sec\n";});
	steady_timer timer2{ioservice, std::chrono::seconds{3}};
	timer2.async_wait([](const boost::system::error_code &ec)
					 {std::cerr << "3 sec\n";});

	std::thread thread1{[&ioservice](){ioservice.run();}};
	std::thread thread2{[&ioservice](){ioservice.run();}};

	std::cout << "async_wait returned" << std::endl;
	double x=0.1;
	for(int i=0;i<100000;++i)
	{
		for(int j=0;j<400;++j)
		{
			x = sin(sin(sin(x+0.2)))*cos(cos(cos(x)));
			x = sin(sin(sin(x+0.2)))*cos(cos(cos(x)));
		}
	}
	std::cout << "x = " << x << std::endl;
	//ioservice.run();
	std::cout << "main finished" << std::endl;
	thread1.join();
	thread2.join();
}
//why ioservice.run()
//why async_wait cannot pring anything when for loop is running.
