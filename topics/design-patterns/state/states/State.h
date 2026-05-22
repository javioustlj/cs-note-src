#pragma once

class State
{
public:
    virtual ~State() = default;

    virtual void entry() = 0;
    virtual void exit()  = 0;
    virtual void handleInterrupt() = 0;
};

