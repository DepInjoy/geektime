#ifndef __MAX_INC_H__
#define __MAX_INC_H__

template<typename T>
T const& Max(const T& t1, const T& t2) {
    return (t1 < t2) ? t2 : t1;
}
#endif