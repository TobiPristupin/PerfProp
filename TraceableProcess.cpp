#include "TraceableProcess.h"

TraceableProcess::TraceableProcess(pid_t childPid, int parentWriteFd, int childStartExecutionCode)
: childPid(childPid), parentWriteFd(parentWriteFd), childStartExecutionCode(childStartExecutionCode) {}

void TraceableProcess::beginExecution() const {
    if (write(parentWriteFd, &childStartExecutionCode, sizeof(childStartExecutionCode)) < 0){
        Logger::error("write(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to start child execution");
    }
}

pid_t TraceableProcess::getPid() const {
    return childPid;
}

TraceableProcess::~TraceableProcess() {
    close(parentWriteFd);
    kill(childPid, SIGTERM);
}


std::unique_ptr<TraceableProcess> TraceableProcessFactory::create(const std::vector<std::string> &programToTrace) {
    /*
    * Set up a pipe so that the parent can send data to the child. The parent will notify the child that its
    * setup is done, so the child can begin executing.
    */
    auto [childReadFd, parentWriteFd] = initPipe();

    pid_t pid = fork();
    if (pid < 0){
        close(childReadFd);
        close(parentWriteFd);
        Logger::error("fork(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to create child process");
    }

    if (pid == 0){ //child. This code segment will never return
        close(parentWriteFd);
        waitUntilReadyToExecute(childReadFd);

        Logger::debug("Child received execution signal. Calling exec");
        char** execArgs = stringVectorToExecArgs(programToTrace);
        if (execvp(execArgs[0], execArgs) < 0){ //exevcp or exit will never return
            reportErrorAndTerminateChild("execvp()");
        }

        //NOTE: No need to delete execArgs, because child will either exit or call execvp (which reclaims memory)

        //unreachable, here just to make compiler happy
        return std::unique_ptr<TraceableProcess>(new TraceableProcess(0,0,0));
    }

    //parent
    close(childReadFd);
    return std::unique_ptr<TraceableProcess>(new TraceableProcess(pid, parentWriteFd, childStartExecutionCode));
}

void TraceableProcessFactory::waitUntilReadyToExecute(int childReadFd) {
    int buf = 0;
    //block until something is read
    ssize_t bytesRead = read(childReadFd, &buf, sizeof(buf));
    if (bytesRead < 0){
        reportErrorAndTerminateChild("Child read()");
    }

    if (buf != childStartExecutionCode){
        Logger::error("Child received invalid pipe message. Exiting");
        exit(EINVAL);
    }

    close(childReadFd);
}

std::pair<int, int> TraceableProcessFactory::initPipe() {
    int fds[2];
    if (pipe(fds) < 0){
        Logger::error("pipe(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to create child process");
    }
    return std::make_pair(fds[0], fds[1]);
}

void TraceableProcessFactory::reportErrorAndTerminateChild(const std::string &message) {
    int error = errno;
    Logger::error(message + ": " + std::string(strerror(errno)));
    Logger::error("Terminating child");
    exit(error);
}


char **TraceableProcessFactory::stringVectorToExecArgs(const std::vector<std::string> &vector) {
    char** arr = new char*[vector.size() + 1]; //+1 in size
    for (int i = 0; i < vector.size(); i++){
        arr[i] = new char[vector[i].size() + 1];
        std::strcpy(arr[i], vector[i].c_str());
    }
    arr[vector.size()] = nullptr; //required by exec;
    return arr;
}
