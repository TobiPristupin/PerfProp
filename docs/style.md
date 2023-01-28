# Style Guide

This is the coding style that I try to adhere for this project. Hopefully knowing this will help explain why some design decisions were taken, and help makes reading code easier.

* **C:** Avoid C style coding when possible.
* **Pointers:** Use smart pointers to indicate ownership. Raw pointers are strictly for non-owning pointers. There is an exception here when dealing with C APIs that do not have smart pointers. In that case, clearly document why a smart pointer wasn't used and who owns it.
* **Exceptions:** Prefer exceptions over C style error codes. Error codes can be used when writing tightly coupled high performance code where it is known that the error code will be checked close by and not propagated across many functions.
* **nodiscard:** Use [[nodiscard]] only when ignoring the result of a function will cause incorrect behavior. Just because a function is pure does not mean that we need to mark it [[nodiscard]]. Compilers and IDEs are good enough at warning for unused variables. Examples where [[nodiscard]] can be useful is when a function returns a pointer that must be managed by the caller. However, see smart pointers bullet on that. 
* **Const Correctness:** Mark member functions const (if they are const). Also mark function parameters. If a function parameter is a non-const reference, you can assume that the function's purpose is to modify that object.