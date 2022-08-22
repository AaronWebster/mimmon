#ifndef COM_WEBSTER_MIMMON_LOGGING_H_
#define COM_WEBSTER_MIMMON_LOGGING_H_

#include <cstdlib>
#include <iostream>
#include <ostream>

#define CHECK(condition)                                          \
  do {                                                            \
    if (!(condition)) {                                           \
      std::cerr << __builtin_FILE() << ":" << __builtin_LINE()    \
                << "] Check failed: " << #condition << std::endl; \
      std::abort();                                               \
    }                                                             \
  } while (0)

#define PCHECK(condition)                                                    \
  do {                                                                       \
    if (!(condition)) {                                                      \
      const auto saved_errno = errno;                                        \
      std::cerr << __builtin_FILE() << ":" << __builtin_LINE()               \
                << "] Check failed: " << #condition << " ("                  \
                << std::strerror(saved_errno) << " [" << saved_errno << "])" \
                << std::endl;                                                \
      std::abort();                                                          \
    }                                                                        \
  } while (0)

#endif  // COM_WEBSTER_MIMMON_LOGGING_H_
