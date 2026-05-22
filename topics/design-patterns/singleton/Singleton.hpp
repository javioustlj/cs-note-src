// Copyright (c) 2024 Javioustlj. All rights reserved.

#pragma once

#include <mutex>
#include <memory>

class Singleton : public std::enable_shared_from_this<Singleton>
{
public:
    Singleton(Singleton &other) = delete;
    void operator=(const Singleton &) = delete;
    ~Singleton();
    static std::shared_ptr<Singleton> getInstance(int);
    int getValue() const;

private:
    Singleton(int value);
    int _value;
    static std::shared_ptr<Singleton> _instance;
    static std::mutex _mutex;
};
