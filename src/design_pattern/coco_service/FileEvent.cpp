#include "FileEvent.hh"

FileEvent::FileEvent(int fd, short events, short revents)
    : _fileDescriptor({fd, events, revents})
{
}

FileEvent::FileEvent(const pollfd& fileDescriptor)
    : _fileDescriptor(fileDescriptor)
{
}

FileDescriptor FileEvent::getFd() const
{
    return _fileDescriptor.fd;
}

PollFd FileEvent::getPollFd() const
{
    return _fileDescriptor;
}

bool FileEvent::operator==(const FileEvent& other) const
{
    return _fileDescriptor.fd == other._fileDescriptor.fd;
}

bool FileEvent::operator<(const FileEvent& other) const
{
    return _fileDescriptor.fd < other._fileDescriptor.fd;
}
