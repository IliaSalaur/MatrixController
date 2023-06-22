#ifndef IFORMAT_HPP
#define IFORMAT_HPP

#include <string>

class IFormat
{
protected:

public:
    IFormat() = default;
    virtual const std::string format(const std::string& args) = 0;
    virtual ~IFormat() = default;
};

#endif