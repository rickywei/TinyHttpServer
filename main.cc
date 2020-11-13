#include <unistd.h>

#include "HttpServer.h"
#include "Logger.h"

using namespace tiny;

int main() {
  Logger::Init();

  HttpServer server;
  server.Start();

  sleep(5);

  return 0;
}