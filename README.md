# TinyHttpServer

A C++ Http Server

Features

- Use Reactor mode
  - Main Reactor listen for the new connection
  - SubReactor process the concrete IO
    - Use epoll lt mode
- ThreadPool
- priority_queue is used for timer manager
- Use mmap to accelerate file IO
- logger is used from my other repo [https://github.com/RickyWei/TinyLogger]
