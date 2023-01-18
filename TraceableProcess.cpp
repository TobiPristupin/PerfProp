#include "TraceableProcess.h"

pid_t TraceableProcess::create(const std::vector<std::string> &programToTrace){
    /*
    * Set up a pipe so that the parent can send data to the child. The parent will notify the child that its
    * setup is done, so the child can begin executing.
    */
    auto [childReadFd, parentWriteFd_] = initPipe();
    this->parentWriteFd = parentWriteFd_;

    pid_t pid = fork();
    if (pid < 0){
        Logger::error("fork(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to create child process");
    }

    if (pid == 0){ //child. This code segment will never return
        close(parentWriteFd_);
        waitUntilReadyToExecute(childReadFd);

        Logger::debug("Child received execution signal. Calling exec");
        char** execArgs = stringVectorToExecArgs(programToTrace);
        if (execvp(execArgs[0], execArgs) < 0){ //exevcp or exit will never return
            reportErrorAndTerminateChild("execvp()");
        }

        //NOTE: No need to delete execArgs, because child will either exit or call execvp (which reclaims memory)
        return 0; //unreachable, here just to make compiler happy
    }

    //parent
    childPid = pid;
    if (close(childReadFd) < 0){
        Logger::error("close(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to create child process");
    }
    return pid;
}

void TraceableProcess::beginExecution(){
    if (!childPid.has_value()){
        throw std::runtime_error("Must call create() before calling begin execution");
    }
    assert(parentWriteFd.has_value()); //parentWriteFd should have value if childPid has value

    if (write(parentWriteFd.value(), &childStartExecutionCode, sizeof(childStartExecutionCode)) < 0){
        Logger::error("write(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to start child execution");
    }

    if (close(parentWriteFd.value()) < 0){
        Logger::error("close(): " + std::string(strerror(errno)));
        throw std::runtime_error("Failed to start child execution");
    }
}

void TraceableProcess::waitUntilReadyToExecute(int childReadFd) const {
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
std::pair<int, int> TraceableProcess::initPipe() {
    int fds[2];
    pipe(fds);
    return std::make_pair(fds[0], fds[1]);
}

void TraceableProcess::reportErrorAndTerminateChild(const std::string &message) {
    int error = errno;
    Logger::error(message + ": " + std::string(strerror(errno)));
    Logger::error("Terminating child");
    exit(error);
}

char** TraceableProcess::stringVectorToExecArgs(const std::vector<std::string> &vector) {
    char** arr = new char*[vector.size() + 1]; //+1 in size
    for (int i = 0; i < vector.size(); i++){
        arr[i] = new char[vector[i].size() + 1];
        std::strcpy(arr[i], vector[i].c_str());
    }
    arr[vector.size()] = nullptr; //required by exec;
    return arr;
}

TraceableProcess::~TraceableProcess() {
    //if beginExecution() is never called, close the fd here.
    if (parentWriteFd.has_value()){
        close(parentWriteFd.value());
    }

    if (childPid.has_value()){
        kill(childPid.value(), SIGTERM);
    }
}
