// Copyright (c) 2024 Javioustlj. All rights reserved.
#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <mutex>

class Singleton
{
public:
    Singleton(Singleton &other) = delete;
    void operator=(const Singleton &) = delete;
    static Singleton& getInstance(int);
    int getValue() const;

private:
    Singleton(int value);
    ~Singleton();
    int _value;
    static Singleton* instance;
    static std::mutex _mutex;
};

#endif
