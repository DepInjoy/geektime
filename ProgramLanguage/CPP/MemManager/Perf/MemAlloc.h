#include "../../Perf/TimeCost.h"
#include <string>

#define TRY_ROUNDS              5000
#define ONE_ROUND_TRY_TIMES     1000

template<class T>
void oneRoundCost(std::string uniqueFlag) {
    T* array[ONE_ROUND_TRY_TIMES];
    TimeCost cost(uniqueFlag);
    for (int i = 0; i < TRY_ROUNDS; ++i) {
        for (int j = 0; j < ONE_ROUND_TRY_TIMES; ++j) {
            array[j] = new T(i);
        }
        for (int j = 0; j < ONE_ROUND_TRY_TIMES; ++j) {
            delete array[j];
        }
    }
}