#include "Max.h"
#include <string>

template<typename T>
T const& Max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

// 函数模板的实例化(Function Template Instantiation)
template int const& Max(const int& t1, const int& t2);
template double const& Max(const double& t1, const double& t2);
template std::string const& Max(const std::string& t1, const std::string& t2);
