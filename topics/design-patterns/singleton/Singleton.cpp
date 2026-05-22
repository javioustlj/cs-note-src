// Copyright (c) 2024 Javioustlj. All rights reserved.
#include <thread>
#include <iostream>
#include "Singleton.hpp"

std::shared_ptr<Singleton> Singleton::_instance;
std::mutex Singleton::_mutex;

std::shared_ptr<Singleton> Singleton::getInstance(int value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_instance) {
        _instance = std::shared_ptr<Singleton>(new Singleton(value));
    }
    return _instance->shared_from_this();
}

int Singleton::getValue() const
{
    return _value;
}

Singleton::Singleton(int value)
    : _value (value)
{
    std::cout << "Singleton::Singleton()" << std::endl;
}

Singleton::~Singleton()
{
}
