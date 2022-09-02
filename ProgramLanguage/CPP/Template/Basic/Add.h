#ifndef __ADD_H__
#define DECLARE_IMPL_SEPARATION
#ifdef DECLARE_IMPL_SEPARATION
template<typename T>
T Add(const T& t1, const T& t2);

template int Add(const int& t1, const int& t2);
#else
template<typename T>
T Add(const T& t1, const T& t2) {
    return t1 + t2;
}
#endif
#endif