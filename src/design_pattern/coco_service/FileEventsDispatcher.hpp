#pragma once

#include <memory>
#include <vector>
#include "FileEvent.hh"
#include "SignalToOne.hh"

class FileEventsDispatcher : public SignalToOne<void(FileEvent)>
{
public:
    FileEventsDispatcher();
    ~FileEventsDispatcher();
    std::vector<PollFd> getPollFds();
};
