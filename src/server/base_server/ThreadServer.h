#ifndef __THREADED_H__
#define __THREADED_H__

#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>
#include <muduo/net/Endian.h>
#include <muduo/net/TcpConnection.h>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

using namespace muduo;
using namespace muduo::net;

#define THREAD_NUM 10;

class ThreadServer : boost::noncopyable
{
public:
    typedef boost::function<void (const TcpConnectionPtr&, const string& message, Timestamp)> StringMessageCallback;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;

    ThreadServer(EventLoop* loop,
                 const InetAddress& listenAddr): server_(loop, listenAddr, "ThreadServer")
    {
        server_.setConnectionCallback(
            boost::bind(&ThreadServer::onConnection, this, _1));
        server_.setMessageCallback(
            boost::bind(&ThreadServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(THREAD_NUM);
    }


  // FIXME: TcpConnectionPtr
    void send(TcpConnection* conn, const StringPiece& message);

    void start();
    void setConnectionCallback(ConnectionCallback& cb) {connectionCallback_ = cb};
    void setMessageCallback(StringMessageCallback& cb) {messageCallback_ = cb};

private:
    void onConnection(const TcpConnectionPtr& conn);

    TcpServer server_;
    const static size_t kHeaderLen = sizeof(int32_t);

    void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);


    StringMessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
};

#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
