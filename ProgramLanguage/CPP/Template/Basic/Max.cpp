
template<typename T>
T const& Max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}

template int const& Max(const int& t1, const int& t2);
template double const& Max(const int& t1, const int& t2);
template std::string const& Max(const int& t1, const int& t2);