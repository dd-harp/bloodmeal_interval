#include <iostream>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <vector>
#include <algorithm>
#include <map>

#include "ordered_unique.hpp"

using namespace std;
using namespace boost::icl;

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
    )
{
    auto sites = ordered_unique(site);

    auto site_cnt = sites.size();
    auto entry_cnt = duration_cnt * site_cnt;
    location.resize(entry_cnt);
    period.resize(entry_cnt);
    integrated.resize(entry_cnt);

    size_t entry_idx = 0;

    for (auto site_idx: sites) {
        split_interval_map<double,int> present;
        std::map<int,double> state;
        for (size_t move_idx = 0; move_idx < site.size(); ++move_idx) {
            int who = human[move_idx];
            double move_time = when[move_idx];

            if (site[move_idx] == site_idx) {
                auto who_iter = state.find(who);
                if (who_iter != state.end()) {
                    ; // Don't update time if moving from this site to this site.
                } else {
                    state[who] = move_time;
                }
            } else {
                auto away_iter = state.find(who);
                if (away_iter != state.end()) {
                    double istart = (*away_iter).second;
                    double ifinish = move_time;
                    present.add(
                        make_pair(
                            interval<double>::right_open(istart, ifinish),
                            1.0
                        )
                    );
                    state.erase(away_iter);
                } else {
                    ;  // You moved from one don't-care site to another.
                }
            }
        }
        // At the end, people may not have moved away from the site, so give them an end time.
        double finish_time = start_time + (duration_cnt + 1) * duration;
        for (const auto &arrival : state) {
            auto arrive_site = arrival.second;
            if (arrive_site <= finish_time) {
                present.add(
                    make_pair(
                        interval<double>::right_open(arrive_site, finish_time),
                        1.0
                    )
                );
            }
        }
        // Now tally up the times.
        for (int duration_idx = 0; duration_idx < duration_cnt; ++duration_idx) {
            interval_set<double> duration_set;
            duration_set.add(interval<double>::right_open(
                start_time + duration_idx * duration / duration_cnt,
                start_time + (duration_idx + 1) * duration / duration_cnt
                ));
            auto during = duration_set & present;
            double total{0};
            for (const auto& v: during) {
                total += v.second * (upper(v.first) - lower(v.first));
            }
            location[entry_idx] = site_idx;
            period[entry_idx] = duration_idx;
            integrated[entry_idx] = total;
            entry_idx++;
        }
    }
}
