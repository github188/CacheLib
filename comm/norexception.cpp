#include "norexception.h"

NorException::NorException(const std::string& strWhat) throw():
    _what(strWhat)
{
}
