#include "codec.h"

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
            messageCallback_(conn, message, receiveTime);
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

    MutexLockGuard lock(mutex_);
    if (!connections_.unique())
    {
        connections_.reset(new ConnectionList(*connections_));
    }
    assert(connections_.unique());

    if (conn->connected())
    {
        connections_->insert(conn);
    }
    else
    {
        connections_->erase(conn);
    }
}


  void onStringMessage(const TcpConnectionPtr&,
                       const string& message,
                       Timestamp)
  {
    ConnectionListPtr connections = getConnectionList();;
    for (ConnectionList::iterator it = connections->begin();
        it != connections->end();
        ++it)
    {
      codec_.send(get_pointer(*it), message);
    }
  }

  ConnectionListPtr getConnectionList()
  {
    MutexLockGuard lock(mutex_);
    return connections_;
  }

  TcpServer server_;
  LengthHeaderCodec codec_;
  MutexLock mutex_;
  ConnectionListPtr connections_;
};

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

