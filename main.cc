#include <stdio.h>

#include "HttpServer.h"
#include "Logger.h"

using namespace hs;
using namespace log;

int main() {
  Logger::Init();

  HttpServer server;
  server.Start();

  return 0;
}