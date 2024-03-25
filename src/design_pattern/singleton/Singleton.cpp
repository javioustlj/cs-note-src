// Copyright (c) 2024 Javioustlj. All rights reserved.
#include <thread>
#include <iostream>
#include "Singleton.hpp"

Singleton* Singleton::instance{nullptr};
std::mutex Singleton::_mutex;

Singleton& Singleton::getInstance(int value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!instance)
    {
        instance = new Singleton(value);
    }
    return *instance;
}

int Singleton::getValue() const
{
    return _value;
}

Singleton::Singleton(int value)
    : _value (value)
{
}

Singleton::~Singleton()
{
}
