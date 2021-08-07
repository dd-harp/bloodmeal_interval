#include <iostream>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <boost/icl/interval_set.hpp>


#include "ordered_unique.hpp"
#include "construct.hpp"

using namespace std;
using namespace boost::icl;

std::map<int,interval_map<double,double>>
human_infection(
    const std::vector<int>& human,
    const std::vector<double>& level,
    const std::vector<double>& when
    )
{
    std::map<int,interval_map<double,double>> infection;
    auto humans = ordered_unique(human);
    for (auto hcreate : humans) {
        infection[hcreate] = interval_map<double,double>();
    }

    std::map<int,std::tuple<double,double>> state;
    for (size_t eidx = 0; eidx < human.size(); ++eidx) {
        auto h = human[eidx];
        auto l = level[eidx];
        auto w = when[eidx];

        auto hiter = state.find(h);
        if (hiter != state.end()) {
            double prev_level;
            double prev_when;
            std::tie(prev_level, prev_when) = hiter->second;
            infection[h].add(
                std::make_pair(
                    interval<double>::right_open(prev_when, w),
                    prev_level
                )
            );
            state[h] = std::make_tuple(l, w);
        } else {
            state[h] = std::make_tuple(l, w);
        }
    }
    return(infection);
}


std::map<int,interval_map<double,int>>
human_location(
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when
    )
{
    std::map<int,interval_map<double,int>> location;
    auto humans = ordered_unique(human);
    for (auto hcreate : humans) {
        location[hcreate] = interval_map<double,int>();
    }
    std::map<int,std::tuple<int,double>> state;
    for (size_t eidx = 0; eidx < human.size(); ++eidx) {
        auto h = human[eidx];
        auto s = site[eidx];
        auto w = when[eidx];

        auto hiter = state.find(h);
        if (hiter != state.end()) {
            int prev_site;
            double prev_when;
            tie(prev_site, prev_when) = hiter->second;
            location[h].add(
                make_pair(
                    interval<double>::right_open(prev_when, w),
                    s
                )
            );
            state[h] = std::make_tuple(s, w);
        } else {
            state[h] = std::make_tuple(s, w);
        }
    }
    return(location);
}


// For each site, which humans are at that site over any interval.
std::map<int,interval_map<double,set<int>>>
location_human(
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when
    )
{
    std::map<int,interval_map<double,set<int>>> location;
    auto sites = ordered_unique(site);
    for (auto screate : sites) {
        location[screate] = interval_map<double,set<int>>();
    }
    std::map<int,std::tuple<int,double>> state;
    for (size_t eidx = 0; eidx < human.size(); ++eidx) {
        auto h = human[eidx];
        auto s = site[eidx];
        auto w = when[eidx];

        auto hiter = state.find(h);
        if (hiter != state.end()) {
            int prev_site;
            double prev_when;
            tie(prev_site, prev_when) = hiter->second;
            location[prev_site].add(
                make_pair(
                    interval<double>::right_open(prev_when, w),
                    set<int>{h}
                )
            );
            state[h] = std::make_tuple(s, w);
        } else {
            state[h] = std::make_tuple(s, w);
        }
    }
    return(location);
}


bool loccmp(const LocType& a, const LocType& b) {
    return(get<2>(a) < get<2>(b));
}

bool illcmp(const IllType& a, const IllType& b) {
    return(get<2>(a) < get<2>(b));
}
