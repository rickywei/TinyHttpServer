# TinyHttpServer

A C++ Http Server

![GitHub](https://img.shields.io/github/license/RickyWei/TinyHttpServer)
![GitHub last commit](https://img.shields.io/github/last-commit/RickyWei/TinyHttpServer)
![Travis (.com)](https://img.shields.io/travis/com/RickyWei/TinyHttpServer)

---

Features

- Use Reactor mode
  - Main Reactor listen for the new connection
  - SubReactor process the concrete IO
    - Use epoll lt mode
- ThreadPool
- priority_queue is used for timer manager
- Use mmap to accelerate file IO
- logger is used from my other repo [https://github.com/RickyWei/TinyLogger]

---

Code summary

```bash
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                             11            237             56           1731
C                                2            104             51            502
C/C++ Header                     8             87              0            323
CMake                            9             64             26            321
make                             1            125             67            226
HTML                             5             16              0             53
Bourne Shell                     1              5              0             12
Markdown                         1              3              0             11
JSON                             1              0              0              3
-------------------------------------------------------------------------------
SUM:                            39            641            200           3182
-------------------------------------------------------------------------------
```

---

Reference

> [https://github.com/chenshuo/muduo]
> [https://github.com/qinguoyi/TinyWebServer]
> [https://github.com/linyacool/WebServer]