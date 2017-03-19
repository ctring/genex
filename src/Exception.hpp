#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

namespace genex {

class GenexException : public std::runtime_error
{
public:
  GenexException(const char* msg) : std::runtime_error(msg) {}
};

} // namespace genex

#endif // EXCEPTION_H
