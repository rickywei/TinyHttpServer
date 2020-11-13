# TinyHttpServer

A C++ Http Server

![GitHub](https://img.shields.io/github/license/RickyWei/TinyHttpServer)
![GitHub last commit](https://img.shields.io/github/last-commit/RickyWei/TinyHttpServer)
![Travis (.com)](https://img.shields.io/travis/com/RickyWei/TinyHttpServer)

## Features

- Use Reactor mode
  - Main Reactor listen for the new connection
  - SubReactor process the concrete IO
    - Use epoll lt mode
- ThreadPool
- priority_queue is used for timer manager
- Use mmap to accelerate file IO
- logger is used from my other repo <https://github.com/RickyWei/TinyLogger>

## Performance

- Environment
  1. Debin10 on virtualbox with settings: 4 CPU core and 4GB memory
  2. host CPU: Intel(R) Core(TM) i5-1035G4 CPU @ 1.10GHz

```bash
webbench -t 10 -c 1000 http://127.0.0.1:8080/
Webbench - Simple Web Benchmark 1.5
Copyright (c) Radim Kolar 1997-2004, GPL Open Source Software.

Benchmarking: GET http://127.0.0.1:8080/
1000 clients, running 10 sec.

Speed=624318 pages/min, 5983047 bytes/sec.
Requests: 104053 susceed, 0 failed.

QPS is about 10405
```

## Code summary

```bash
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             11            237             58           1729
C                                4            140            117            910
C/C++ Header                     8             87              0            323
CMake                            9             65             26            323
make                             3            146             78            298
Markdown                         1             20              0             54
HTML                             5             16              0             53
Bourne Shell                     2              8              0             21
YAML                             1              3              6              5
JSON                             1              0              0              3
-------------------------------------------------------------------------------
SUM:                            45            722            285           3719
-------------------------------------------------------------------------------
```

## Reference

> <https://github.com/chenshuo/muduo>

> <https://github.com/qinguoyi/TinyWebServer>

> <https://github.com/linyacool/WebServer>