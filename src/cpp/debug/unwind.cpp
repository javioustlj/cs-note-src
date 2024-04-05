// Copyright (c) 2024 Javioustlj. All rights reserved.

#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <cxxabi.h>

void printSigInfo([[maybe_unused]] int signo, siginfo_t* info, [[maybe_unused]] void* context)
{
    std::cout << "Signal Info:" << std::endl;
    std::cout << "  si_signo: " << info->si_signo << " - Signal number" << std::endl;
    std::cout << "  si_code: " << info->si_code << " - Signal code" << std::endl;
    std::cout << "  si_errno: " << info->si_errno << " - Errno value associated with signal" << std::endl;
    if (info->si_code > 0 && info->si_code < NSIG) { // 可能需要针对具体情况进行检查
        std::cout << "  si_pid: " << info->si_pid << " - Sending process ID" << std::endl;
        std::cout << "  si_uid: " << info->si_uid << " - Real user ID of sending process" << std::endl;
        // 如果信号与特定地址相关联（如段错误等），打印该地址
        if (info->si_addr) {
            std::cout << "  si_addr: " << static_cast<void*>(info->si_addr) << " - Faulting address" << std::endl;
        }

    }
}

void LinuxSigHandler(int signo, siginfo_t* info, void* context)
{
    switch (signo) {
        case SIGABRT:
            std::cout << "Caught signal SIGABRT (" << signo << ")." << std::endl;
            printSigInfo (signo, info, context);
            break;
        case SIGTERM:
            std::cout << "Caught signal SIGTERM (" << signo << ")." << std::endl;
            printSigInfo (signo, info, context);
            pthread_exit(0);
            break;
        default:
            std::cout << "Caught signal " << signo << std::endl;
            break;
    }

    // 重新引发信号
    raise(signo);
}

void InitSignalHandlers(void)
{
    /* Observed signals. */
    struct sigaction action;
    action.sa_handler = NULL;
    action.sa_sigaction = LinuxSigHandler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = (typeof(action.sa_flags))(SA_RESTART | SA_SIGINFO | SA_RESETHAND);
    sigaction (SIGSEGV, &action, NULL);
    sigaction (SIGILL, &action, NULL);
    sigaction (SIGFPE, &action, NULL);
    sigaction (SIGBUS, &action, NULL);
    sigaction (SIGABRT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    /* Ignored signals. */
    struct sigaction action_ignore;
    action_ignore.sa_handler = SIG_IGN;
    action_ignore.sa_flags = 0;
    sigemptyset (&action_ignore.sa_mask);
    sigaction (SIGPIPE, &action_ignore, NULL);
}

void main_thread()
{
    for (;;)
    {
        std::cout << "main_thread is running:" << std::endl;
        sleep(10);
    }
}

int main()
{
    InitSignalHandlers();

    try {
        main_thread();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
	catch (abi::__forced_unwind&) {
		std::cerr << "Caught forced_unwid error and retrow it" << std::endl;
		throw;
	}
    catch (...) {
        std::cerr << "Caught unknown exception" << std::endl;
    }

    std::cerr << "process close" << std::endl;

    return 0;
}
