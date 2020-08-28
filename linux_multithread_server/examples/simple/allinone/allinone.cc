#include "examples/simple/chargen/chargen.h"
#include "examples/simple/daytime/daytime.h"
#include "examples/simple/discard/discard.h"
#include "examples/simple/echo/echo.h"
#include "examples/simple/time/time.h"

#include "muduo/base/Logging.h"
#include "muduo/net/EventLoop.h"

#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int main()
{
  LOG_INFO << "pid = " << getpid();
  EventLoop loop;
  
  ChargenServer chargenserver(&loop, InetAddress(2019));
  chargenserver.start();

  DaytimeServer daytimeServer(&loop, InetAddress(2013));
  daytimeServer.start();

  DiscardServer DiscardServer(&loop, InetAddress(2009));
  discardServer.start();

  EchoServer echoServer(&loop, InetAddress(2007));
  echoServer.start();

  TimeServer timeServer(&loop, InetAddress(2037));
  timeServer.start();

  loop.loop();
}