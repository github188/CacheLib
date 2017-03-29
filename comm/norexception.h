#ifndef NOREXCEPTION_H
#define NOREXCEPTION_H

#include <string>
#include <exception>

class NorException : public std::exception
{
public:
    NorException(const std::string& strWhat ) throw();
    virtual ~NorException() throw()
    {
    }

    const char* what() const throw()
    {
        return _what.c_str();
    }
private:
    std::string  _what;
};

#endif // CACHEEXCEPTION_H
