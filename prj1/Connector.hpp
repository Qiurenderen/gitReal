/*
 * Connector.hpp
 *
 *  Created on: Oct 27, 2017
 *      Author: botao
 */

#ifndef CONNECTOR_HPP_
#define CONNECTOR_HPP_
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <boost/thread/thread.hpp>
#include "RWHandler.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

class Connector
{
public:

    Connector(io_service& ios, const string& strIP, short port):
			 m_ios(ios),
			 m_socket(ios),
			 m_serverAddr(tcp::endpoint(address::from_string(strIP), port)),
			 m_isConnected(false),
			 m_chkThread(nullptr)
    {
        CreateEventHandler(ios);
    }

    ~Connector(){ }

    bool Start()
    {
        m_eventHandler->GetSocket().async_connect(m_serverAddr, [this](const boost::system::error_code& error)
        {
            if (error)
            {
                HandleConnectError(error);
                return;
            }
            cout << "connect ok" << endl;
            m_isConnected = true;
            m_eventHandler->HandleRead(); //连接成功后发起一个异步读的操作
        });

        boost::this_thread::sleep(boost::posix_time::seconds(1));
        return m_isConnected;
    }

    bool IsConnected() const
    {
        return m_isConnected;
    }

    void Send(char* data, int len)
    {
        if (!m_isConnected)
            return;

        m_eventHandler->HandleWrite(data, len);
    }

    /*
    void AsyncSend(char* data, int len)
    {
        if (!m_isConnected)
            return;

        m_eventHandler->HandleAsyncWrite(data, len);
    }
     */

private:
    void CreateEventHandler(io_service& ios)
    {
        m_eventHandler = std::make_shared<RWHandler>(ios);
        m_eventHandler->SetCallBackError([this](int connid){HandleRWError(connid); });
    }

    void CheckConnect()
    {
        if (m_chkThread != nullptr)
            return;

        m_chkThread = std::make_shared<std::thread>([this]
        {
            while (true)
            {
                if (!IsConnected())
                    Start();

                boost::this_thread::sleep(boost::posix_time::seconds(1));
            }
        });
    }

    void HandleConnectError(const boost::system::error_code& error)
    {
        m_isConnected = false;
        cout << error.message() << endl;
        m_eventHandler->CloseSocket();
        CheckConnect();
    }

    void HandleRWError(int connid)
    {
        m_isConnected = false;
        CheckConnect();
    }

private:
    io_service& m_ios;
    tcp::socket m_socket;
    tcp::endpoint m_serverAddr; //服务器地址

    std::shared_ptr<RWHandler> m_eventHandler;
    bool m_isConnected;
    std::shared_ptr<std::thread> m_chkThread; //专门检测重连的线程
};

#endif /* CONNECTOR_HPP_ */
