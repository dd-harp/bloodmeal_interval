#include <vector>

std::tuple<std::vector<int>,std::vector<int>,std::vector<double>> humans_at_site_orig(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when
    );

std::tuple<std::vector<int>,std::vector<int>,std::vector<double>>
humans_at_site(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& sites,
    const std::map<int,boost::icl::interval_map<double,int>>& human
    );
