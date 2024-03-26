#pragma once

#include <poll.h>

using FileDescriptor = int;
using PollFd         = pollfd;

class FileEvent
{
public:
    FileEvent(int fd, short events, short revents = 0);
    explicit FileEvent(const pollfd& fileDescriptor);

    FileDescriptor getFd() const;
    PollFd         getPollFd() const;

    bool operator==(const FileEvent&) const;
    bool operator!=(const FileEvent&) const;
    bool operator<(const FileEvent&) const;
    bool operator>(const FileEvent&) const;

private:
    PollFd _fileDescriptor;
};
