#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

namespace genex {

class GenexException : public std::runtime_error
{
public:
  GenexException(const char* msg) : std::runtime_error(msg) {}
  GenexException(std::string msg) : std::runtime_error(msg.c_str()) {}
};

} // namespace genex

#endif // EXCEPTION_H
