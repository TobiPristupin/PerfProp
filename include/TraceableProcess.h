#ifndef BAYESPERF_CPP_TRACEABLEPROCESS_H
#define BAYESPERF_CPP_TRACEABLEPROCESS_H

#include <csignal>
#include <cassert>
#include <string>
#include <optional>
#include <cstring>
#include <memory>
#include <vector>
#include "Logger.h"

/*
 * Construct by using TraceableProcessFactory::create(). A traceable process does not begin execution
 * (i.e. call execvp) until beginExecution is called. This allows the user of this class to perform some initial setup
 * using the traced process pid before the traced process begins execution.
 *
 * This class follows RAII. The destructor for this class will send SIGTERM to the traced process.
 *
 * We use a factory pattern because a lot may fail in the construction of the process, and we want to handle
 * resource cleanup nicely (i.e. no throwing exceptions from constructors). In this way, the factory either
 * creates a valid traceable process, or fails to create it and does its own cleanup. If you have a TraceableProcess
 * instance, you can be ensured that it is valid and that it will clean up after itself.
 */
class TraceableProcess {
public:
    /*
     * Signals to the traceable process that it can begin execution. create() needs to be called before
     * calling this method.
     *
     * May throw std::runtime_exception
     */
    void beginExecution() const;

    [[nodiscard]] pid_t getPid() const;

    [[nodiscard]] bool hasTerminated() const;

    /*
     * Sends SIGTERM to child
     */
    ~TraceableProcess();

    friend class TraceableProcessFactory;

private:
    TraceableProcess(pid_t childPid, int parentWriteFd, int childStartExecutionCode);

    const int childStartExecutionCode;
    pid_t childPid;
    int parentWriteFd;
};

class TraceableProcessFactory {
public:
    /*
     * Creates a traceable process. Takes in the shell command that the child process will execute. For example, if
     * you want to execute "ls -lah", programToTrace should contain {"ls", "-lah"}.
     *
     * May throw std::runtime_exception.
     *
     * Note that the returned process will not begin execution until beginExecution() is called on it.
     */
    static std::unique_ptr<TraceableProcess> create(const std::vector<std::string> &programToTrace);

private:

    /*
     * Called by the child to wait until the parent signals that it can start executing.
     * A child can start executing when it receives the childStartExecution code on its pipe.
     */
    static void waitUntilReadyToExecute(int childReadFd);

    static std::pair<int, int> initPipe();

    static void reportErrorAndTerminateChild(const std::string& message);

    /**
     * Given a vector of program commands to be fed into exec, this function will convert it into a an array
     * of C-strings. It will also append a NULL element to the end of the array, which is required by exec.
     *
     * NOTE: We cannot use smart pointers here, as we need to pass in the raw array to exec,
     * and unpacking nested smart pointers would be a lot of work.
     */
    [[nodiscard]] static char** stringVectorToExecArgs(const std::vector<std::string>& vector);


    static const int childStartExecutionCode = 91218; //arbitrary number
};

#endif
