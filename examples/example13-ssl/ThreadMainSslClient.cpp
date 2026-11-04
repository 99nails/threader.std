#include "ThreadMainSslClient.h"

#include "ThreadSslClientConnection.h"

ThreadMainSslClient::ThreadMainSslClient(const std::string &applicationBinaryName,
                                         const std::string &applicationFriendlyName)
    : ThreadMainWithoutSettings(nullptr,
                               applicationBinaryName,
                               applicationFriendlyName)
{
}

void ThreadMainSslClient::startChildThreads()
{
    _threadClient = registerAndStartChildThread(new ThreadSslClientConnection(this,
                                                                              "localhost",
                                                                              45796,
                                                                              false));

    _threadClientSsl = registerAndStartChildThread(new ThreadSslClientConnection(this,
                                                                                 "localhost",
                                                                                 45797,
                                                                                 true));
}
