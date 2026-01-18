#pragma once

#include <DaemonApplication.h>

#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>

namespace Threader::Threads::QtEventLoop
{

template<class ThreadMainType>
class DaemonApplicationQt : public DaemonApplication<ThreadMainType>
{
public:
    explicit DaemonApplicationQt(int argc,
                                 char *argv[],
                                 const std::string &description = std::string(),
                                 const std::string &serviceName = std::string());

    virtual ~DaemonApplicationQt();

protected:
    virtual void runMainLoop();
    virtual void exitMainLoop();

private:
    QCoreApplication *_application;
    ThreadMainType *_thread;
};

template<class ThreadMainType>
DaemonApplicationQt<ThreadMainType>::DaemonApplicationQt(int argc,
                                                         char *argv[],
                                                         const std::string &description,
                                                         const std::string &serviceName)
    : DaemonApplication<ThreadMainType>(argc, argv, description, serviceName)
    , _application(new QCoreApplication(argc, argv))
{
}

template<class ThreadMainType>
DaemonApplicationQt<ThreadMainType>::~DaemonApplicationQt()
{
    if (_application)
    {
        delete _application;
        _application = nullptr;
    }
}

template<class ThreadMainType>
void DaemonApplicationQt<ThreadMainType>::runMainLoop()
{
    if (!_application)
        return;

    if (this->console())
    {
        auto waitEnter = [&]()
        {
            std::string line;
            std::getline(std::cin, line);

            auto thread = this->threadMain();
            thread->postTerminateEvent();
            while (!thread->isThreadFinished())
            {
                DateTimeUtils::sleepMilliseconds(100);
            }

            QCoreApplication::exit(0);
        };

        QtConcurrent::run(waitEnter);
    }

    _application->exec();
}

template<class ThreadMainType>
void DaemonApplicationQt<ThreadMainType>::exitMainLoop()
{
    QCoreApplication::exit(0);
}

}
