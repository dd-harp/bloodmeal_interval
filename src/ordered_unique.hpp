#ifndef _ORDERED_UNIQUE_H_
#define _ORDERED_UNIQUE_H_ 1
#include <vector>
#include <set>

/*! Given a vector of elements, return the unique elements, in lexical order.
 *
 */
template<typename T>
std::vector<T> ordered_unique(const std::vector<T>& arr) {
    std::set<T> unique_set(arr.begin(), arr.end());
    std::vector<T> ordunique(unique_set.begin(), unique_set.end());
    std::sort(ordunique.begin(), ordunique.end());
    return(ordunique);
}

#endif  // _ORDERED_UNIQUE_H_
