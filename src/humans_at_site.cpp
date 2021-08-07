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

std::tuple<std::vector<int>,std::vector<int>,std::vector<double>>
humans_at_site_orig(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when
    )
{
    auto sites = ordered_unique(site);

    auto site_cnt = sites.size();
    auto entry_cnt = duration_cnt * site_cnt;
    std::vector<int> location(entry_cnt);
    std::vector<int> period(entry_cnt);
    std::vector<double> integrated(entry_cnt);

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
    return(make_tuple(location, period, integrated));
}


// A table of site, interval, human-hours during interval.
std::tuple<std::vector<int>,std::vector<int>,std::vector<double>>
humans_at_site(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& sites,
    const std::map<int,interval_map<double,int>>& human
    )
{
    auto site_cnt = sites.size();
    auto entry_cnt = duration_cnt * site_cnt;
    std::vector<int> location(entry_cnt);
    std::vector<int> period(entry_cnt);
    std::vector<double> integrated(entry_cnt);

    size_t entry_idx = 0;

    for (auto site_idx: sites) {
        split_interval_map<double,int> present;

        for (const auto& human_locs: human) {
            int h = human_locs.first;
            const auto& locs = human_locs.second;
            for (const auto& loc : locs) {
                if (loc.second == site_idx) {
                    present.add(make_pair(loc.first, 1));
                }
            }
        }

        // Now tally up the times.
        for (int duration_idx = 0; duration_idx < duration_cnt; ++duration_idx) {
            auto duration_element = interval<double>::right_open(
                start_time + duration_idx * duration / duration_cnt,
                start_time + (duration_idx + 1) * duration / duration_cnt
                );
            auto during = present & duration_element;
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
    return(make_tuple(location, period, integrated));
}


// human: Map from human ID to interval map of site ID.
// infection: Map from human ID to interval map of PR level.
// Calculate the sum of the Parasite Rate at any site over the course
// of each duration of time.
std::tuple<std::vector<int>,std::vector<int>,std::vector<double>>
kappa_at_site(
    double start_time,
    double duration,
    int duration_cnt,
    const std::vector<int>& sites,
    const std::map<int,interval_map<double,int>>& human,
    const std::map<int,interval_map<double,double>>& infection
    )
{
    auto site_cnt = sites.size();
    auto entry_cnt = duration_cnt * site_cnt;
    std::vector<int> location(entry_cnt);
    std::vector<int> period(entry_cnt);
    std::vector<double> integrated(entry_cnt);

    size_t entry_idx = 0;

    for (auto site_idx: sites) {
        // The sum of the PR for all people at this site during this interval.
        split_interval_map<double,double> present;

        for (const auto& human_locs: human) {
            int h = human_locs.first;
            const auto& locs = human_locs.second;
            for (const auto& loc : locs) {
                if (loc.second == site_idx) {
                    auto parasite_rates = infection.at(h) & loc.first;
                    for (const auto& pr : parasite_rates) {
                        present.add(make_pair(pr.first, pr.second));
                    }
                }
            }
        }

        // Now tally up the times.
        for (int duration_idx = 0; duration_idx < duration_cnt; ++duration_idx) {
            auto duration_element = interval<double>::right_open(
                start_time + duration_idx * duration / duration_cnt,
                start_time + (duration_idx + 1) * duration / duration_cnt
                );
            auto during = present & duration_element;
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
    return(make_tuple(location, period, integrated));
}
