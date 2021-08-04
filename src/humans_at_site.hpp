#include <vector>

void humans_at_site(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when,
    std::vector<int>& location,
    std::vector<int>& period,
    std::vector<double>& integrated
    );
