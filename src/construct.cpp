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

std::map<int,IllMap>
human_infection(const std::vector<IllType>& events, double end_time)
{
    std::map<int,IllMap> infection;
    std::map<int,std::tuple<double,double>> state;

    for (size_t eidx = 0; eidx < events.size(); ++eidx) {
        Human h;
        Kappa l;
        Time w;
        std::tie(h, l, w) = events[eidx];

        auto hiter = state.find(h);
        if (hiter != state.end()) {
            double prev_level;
            double prev_when;
            std::tie(prev_level, prev_when) = hiter->second;
            if (infection.find(h) == infection.end()) {
                std::cout << "new int_map for " << h << std::endl;
                infection[h] = IllMap();
            }
            std::cout << "add to int_map for " << h << " " << prev_when << " " << w << std::endl;
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
    for (const auto& skv: state) {
        std::cout << "state " << skv.first << " " << std::get<0>(skv.second) << " " <<
            std::get<1>(skv.second) << std::endl;
    }
    // Add intervals for states to the end of the duration.
    for (const auto& state_kv: state) {
        Human h = state_kv.first;
        Kappa prev_level;
        Time prev_when;
        tie(prev_level, prev_when) = state_kv.second;
        if (prev_when < end_time) {
            if (infection.find(h) == infection.end()) {
                std::cout << "late int_map for " << h << std::endl;
                infection[h] = IllMap();
            }
            infection.at(h) +=
                std::make_pair(
                    interval<double>::right_open(prev_when, end_time),
                    prev_level
                )
            ;
            std::cout << "adding " << h << " " << prev_when << " " << end_time
                << " " << interval_count(infection.at(h)) << std::endl;
        }
    }
    return(infection);
}


std::map<int,MoveMap>
human_location(const std::vector<LocType>& human, double end_time)
{
    std::map<int,MoveMap> location;
    std::map<int,std::tuple<Site,Time>> state;

    for (size_t eidx = 0; eidx < human.size(); ++eidx) {
        Human h;
        Site s;
        Time w;
        tie(h, s, w) = human[eidx];

        auto hiter = state.find(h);
        if (hiter != state.end()) {
            Site prev_site;
            Time prev_when;
            tie(prev_site, prev_when) = hiter->second;
            if (location.find(h) == location.end()) {
                location[h] = MoveMap();
            }
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
    // Add intervals to represent remaining at a site until the end time.
    for (const auto& state_kv: state) {
        Human h = state_kv.first;
        Site prev_site;
        Time prev_when;
        tie(prev_site, prev_when) = state_kv.second;
        if (prev_when < end_time) {
            if (location.find(h) == location.end()) {
                location[h] = MoveMap();
            }
            location[h] += make_pair(
                interval<Time>::right_open(prev_when, end_time),
                prev_site
            );
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
