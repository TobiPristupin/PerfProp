This is the coding style that I try to adhere for this project. Hopefully knowing this will help explain why some 
design decisions were taken, and help makes reading code easier.


* Always prefer modern C++ over C style coding.
* Use smart pointers to indicate ownership. Raw pointers are strictly for non-owning pointers. There is an exception 
  here when dealing with C APIs that do not have smart pointers. In that case, clearly document why a smart pointer 
  wasn't used and who owns it.
* Prefer exceptions over C style error codes. Exceptions can be made when writing tightly coupled high performance 
  code where it is known that the error code will be checked close by and not propagated across many functions.
* Use [[nodiscard]] only when ignoring the result of a function will cause incorrect behavior. Just because a 
  function is pure does not mean that we need to mark it [[nodiscard]]. Compilers and IDEs are good enough at 
  warning for unused variables. Examples where [[nodiscard]] can be useful is when a function returns a pointer 
  that must be managed by the caller. However, see smart pointers bullet on that. 