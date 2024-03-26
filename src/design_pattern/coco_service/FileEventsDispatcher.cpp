#include <algorithm>
#include <iterator>
#include "FileEventsDispatcher.hpp"

FileEventsDispatcher::FileEventsDispatcher()
{
}

FileEventsDispatcher::~FileEventsDispatcher()
{
}

std::vector<PollFd> FileEventsDispatcher::getPollFds()
{
    auto                keys = getKeys();
    std::vector<PollFd> fds;
    fds.reserve(keys.size());
    std::transform(keys.begin(), keys.end(), std::back_inserter(fds), [&](const auto& key) { return key.getPollFd(); });

    return fds;
}
