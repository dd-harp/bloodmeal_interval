#include "random.hpp"

boost::random::lagged_fibonacci_01_engine<double, 48, 607u, 273u>
make_rng(size_t seed) {
    return(boost::random::lagged_fibonacci607(seed));
}
