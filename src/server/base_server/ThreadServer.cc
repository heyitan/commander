
#include <muduo/base/Logging.h>
#include <muduo/base/Mutex.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <set>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

void ThreadServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime);
{
    while (buf->readableBytes() >= kHeaderLen) // kHeaderLen == 4
    {
        // FIXME: use Buffer::peekInt32()
        const void* data = buf->peek();
        int32_t be32 = *static_cast<const int32_t*>(data); // SIGBUS
        const int32_t len = sockets::networkToHost32(be32);
        if (len > 65536 || len < 0)
        {
            LOG_ERROR << "Invalid length " << len;
            conn->shutdown();  // FIXME: disable reading
            break;
        }
        else if (buf->readableBytes() >= len + kHeaderLen)
        {
            buf->retrieve(kHeaderLen);
            string message(buf->peek(), len);
            if (messageCallback_ != NULL)
            {
                messageCallback_(conn, buf, receiveTime);
            }
            buf->retrieve(len);
        }
        else
        {
            break;
        }
    }
}

void ThreadServer::onConnection(const TcpConnectionPtr& conn)
{
    LOG_INFO << conn->localAddress().toIpPort() << " -> "
        << conn->peerAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");

    if( connectionCallback_ != NULL)
    {
        connectionCallback_(conn);
    }
}

void ThreadServer::start() ->
{
    server_.start();
}

void ThreadServer::send(TcpConnection *conn, const StringPiece& message)
{
    Buffer buf;
    buf.append(message.data(), message.size());
    int32_t len = static_cast<int32_t>(message.size());
    int32_t be32 = sockets::hostToNetwork32(len);
    buf.prepend(&be32, sizeof be32);
    conn->send(&buf);
}

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid();
  if (argc > 1)
  {
    EventLoop loop;
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    InetAddress serverAddr(port);
    ThreadServer server(&loop, serverAddr);
    if (argc > 2)
    {
      server.setThreadNum(atoi(argv[2]));
    }
    server.start();
    loop.loop();
  }
  else
  {
    printf("Usage: %s port [thread_num]\n", argv[0]);
  }
}

