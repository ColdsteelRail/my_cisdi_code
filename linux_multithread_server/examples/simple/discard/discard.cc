#include "examples/simple/discard/discard.h"

#include "muduo/base/Logging.h"

DiscardServer::DiscardServer(muduo::net::EventLoop* loop,
                      const muduo::net::InntAddress& listenAddr)
    : server_(loop, listenAddr, "DiscardServer")
{
    server.setConnectionCallback(
        std::bind(&DiscardServer::onConnection, this, _1)
    );

    server.setConnectionCallback(
        std::bind(&DiscardServer::onMessage, this, _1, _2, _3)
    );
}

void DiscardServer::start()
{
  server_.start();
}

void DiscardServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    LOG_INFO << "DiscardServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void DiscardServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
                muduo::net::Buffer* buf,
                muduo::Timestamp time)
{
    string msg(buf->retriveAllAsString());
    LOG_INFO << conn->name() " discards " << msg.size()
             << " bytes recieved at " << time.toString();
}