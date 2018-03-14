/*
 * RWHandler.hpp
 *
 *  Created on: Oct 26, 2017
 *      Author: botao
 */

#ifndef RWHANDLER_HPP_
#define RWHANDLER_HPP_

#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

class RWHandler
{
public:
	enum : int {MAX_IP_PACK_SIZE = 65536};
	enum : int {HEAD_LEN = 4};

    RWHandler(io_service& ios) : m_sock(ios),m_connId(0){}
    ~RWHandler(){}

    void HandleRead()
    {
        //三种情况下会返回：1.缓冲区满；2.transfer_at_least为真(收到特定数量字节即返回)；3.有错误发生
        boost::asio::async_read(m_sock, buffer(m_buff), transfer_at_least(HEAD_LEN),
        		   [this](const boost::system::error_code& ec, size_t size)
					{
						if (ec != nullptr)
						{
							HandleError(ec);
							return;
						}

						//cout << m_buff.data() + HEAD_LEN << endl;
						cout << "handler Id: " << m_connId << " handling read" << endl;
						cout << m_buff.data() << endl;

						HandleRead();
					});
    }

    void HandleWrite(char* data, int len)
    {
        boost::system::error_code ec;
        write(m_sock, buffer(data, len), ec);
        if (ec != nullptr)
            HandleError(ec);
    }

    tcp::socket& GetSocket(){ return m_sock; }

    void CloseSocket()
    {
        boost::system::error_code ec;
        m_sock.shutdown(tcp::socket::shutdown_send, ec);
        m_sock.close(ec);
    }

    void SetConnId(int connId){ m_connId = connId; }
    int GetConnId() const { return m_connId; }

    template<typename F>
    void SetCallBackError(F f)
    {
        m_callbackError = f;
    }

private:
    void HandleError(const boost::system::error_code& ec)
    {
        CloseSocket();
        cout << "Handle Error: " << ec.message() << endl;
        if (m_callbackError)
            m_callbackError(m_connId);
    }

private:
    tcp::socket m_sock;
    std::array<char, MAX_IP_PACK_SIZE> m_buff;
    int m_connId;
    std::function<void(int)> m_callbackError;
};

#endif /* RWHANDLER_HPP_ */
