#ifndef BAYESPERF_CPP_TRACEABLEPROCESS_H
#define BAYESPERF_CPP_TRACEABLEPROCESS_H

#include <csignal>
#include <cassert>
#include <string>
#include <optional>
#include <cstring>
#include "expected.h"
#include "Logger.h"


/*
 * This class creates a child process to be traced. create() will take in the shell command to run and will
 * fork the child process and return the pid. The child process will not begin executing until beginExecution()
 * is called. This allows the user of this class to perform some initial setup using the traced process pid
 * before the traced process begins executing.
 *
 * This class follows RAII. The destructor for this class will send SIGTERM to the traced process.
 */
class TraceableProcess {
public:

    /*
     * Creates child process using fork and returns its pid, or an error code. Takes in the shell command that the
     * child process will execute. For example, if you want to execute "ls -lah", programToTrace would
     * contain {"ls", "-lah"}.
     *
     * Note that the child process will not begin execution until beginExecution() is called.
     */
    tl::expected<pid_t, int> create(const std::vector<std::string> &programToTrace);

    /*
     * Signals to the traceable process that it can begin execution. create() needs to be called before
     * calling this method.
     */
    std::optional<int> beginExecution();

    /*
     * Close file descriptors and send SIGTERM to child
     */
    ~TraceableProcess();

private:

    /*
     * Called by the child to wait until the parent signals that it can start executing.
     * A child can start executing when it receives the childStartExecution code on its pipe.
     */
    void waitUntilReadyToExecute(int childReadFd) const;

    static std::pair<int, int> initPipe();

    static void reportErrorAndTerminateChild(const std::string& message);

    /**
     * Given a vector of program commands to be fed into exec, this function will convert it into a an array
     * of C-strings. It will also append a NULL element to the end of the array, which is required by exec.
     *
     * NOTE: We cannot use smart pointers here, as we need to pass in the raw array to exec,
     * and unpacking nested smart pointers would be a lot of work.
     */
    static char** stringVectorToExecArgs(const std::vector<std::string>& vector);

    std::optional<pid_t> childPid{std::nullopt};
    const int childStartExecutionCode = 91218; //arbitrary number
    std::optional<int> parentWriteFd{std::nullopt};
};


#endif
