/*
 * Server.hpp
 *
 *  Created on: Oct 26, 2017
 *      Author: botao
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <list>
#include <unordered_map>
//#include "Message.hpp"
#include "RWHandler.hpp"

using namespace boost::asio::ip;
using namespace std;

class Server
{
public:
	enum: int {MaxConnectionNum = 65536};
	enum: int {MaxRecvSize = 65536};
	Server(io_service& ios, short port) : m_ios(ios), m_acceptor(ios, tcp::endpoint(tcp::v4(), port)), m_cnnIdPool(MaxConnectionNum)
    {
        int current = 0;
        std::generate_n(m_cnnIdPool.begin(), MaxConnectionNum, [&current]{return ++current; });
    }

    ~Server(){}

    void Accept()
    {
        cout << "Start Listening " << endl;
        std::shared_ptr<RWHandler> handler = CreateHandler();

        m_acceptor.async_accept(handler->GetSocket(), [this, handler](const boost::system::error_code& error)
        {
            if (error)
            {
                cout << "Accept error: " << error.value() << " " << error.message() << endl;
                HandleAcpError(handler, error);
            }

            m_handlers.insert(std::make_pair(handler->GetConnId(), handler));
            cout << "current connect count: " << m_handlers.size() << endl;

            handler->HandleRead();
            Accept();
        });
    }

private:
    void HandleAcpError(std::shared_ptr <RWHandler> eventHanlder, const boost::system::error_code& error)
    {
        cout << "Error，error reason：" << error.value() << error.message() << endl;
        //关闭socket，移除读事件处理器
        eventHanlder->CloseSocket();
        StopAccept();
    }

    void StopAccept()
    {
        boost::system::error_code ec;
        m_acceptor.cancel(ec);
        m_acceptor.close(ec);
        m_ios.stop();
    }

    std::shared_ptr<RWHandler> CreateHandler()
    {
        int connId = m_cnnIdPool.front();
        m_cnnIdPool.pop_front();
        std::shared_ptr<RWHandler> handler = std::make_shared<RWHandler>(m_ios);

        handler->SetConnId(connId);

        handler->SetCallBackError([this](int connId)
        {
            RecyclConnid(connId);
        });

        return handler;
    }

    void RecyclConnid(int connId)
    {
        auto it = m_handlers.find(connId);
        if (it != m_handlers.end())
        {
            m_handlers.erase(it);
            cout << "removed connection Id " << connId << endl;
        }
        cout << "current connect count: " << m_handlers.size() << endl;
        m_cnnIdPool.push_back(connId);
        cout << "connection Id is recycled." << endl;
    }

private:
    io_service& m_ios;
    tcp::acceptor m_acceptor;
    unordered_map<int, std::shared_ptr<RWHandler>> m_handlers;
    list<int> m_cnnIdPool;
};

#endif /* SERVER_HPP_ */
