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

class ThreadServer : boost::noncopyable
{
public:
    typedef boost::function<void (const TcpConnectionPtr&, const string& message, Timestamp)> StringMessageCallback;
    typedef std::set<TcpConnectionPtr> ConnectionList;
    typedef boost::shared_ptr<ConnectionList> ConnectionListPtr;

    ThreadServer(EventLoop* loop,
                 const InetAddress& listenAddr): server_(loop, listenAddr, "ThreadServer"), connections_(new ConnectionList)
    {
        server_.setConnectionCallback(
        boost::bind(&ThreadServer::onConnection, this, _1));
        server_.setMessageCallback(
        boost::bind(&ThreadServer::onMessage, this, _1, _2, _3));
    }

    void setThreadNum(int numThreads)
    {server_.setThreadNum(numThreads);}

    void start()
    {server_.start();}

private:
  void onConnection(const TcpConnectionPtr& conn);


  void onStringMessage(const TcpConnectionPtr&, const string& message, Timestamp);
  void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);

  // FIXME: TcpConnectionPtr
  void send(TcpConnection* conn,
            const StringPiece& message)
  {
    Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof be32);
    conn->send(&buf);
  }

 private:
  StringMessageCallback messageCallback_;
  const static size_t kHeaderLen = sizeof(int32_t);
};

#endif  // MUDUO_EXAMPLES_ASIO_CHAT_CODEC_H
