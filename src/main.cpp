#include <assert.h>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <utility>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "humans_at_site.hpp"
#include "construct.hpp"
#include "show_type.hpp"
#include "bloodmeal.hpp"
#include "random.hpp"

using namespace std;
using namespace boost::icl;
using namespace boost::random;


// Given input data on illness, from the Human module, and input data on movement, from the
// location module, combine them into a single interval_map that records both location
// and illness level, splitting the intervals. So you get, for each human, a single record
// of where they were and how infectious they were.
HumanState
combine_human_intervals(
    const std::map<Human,IllMap>& ill,
    const std::map<Human,MoveMap>& move
) {
    HumanState li_map;
    for (const auto& move_kv: move) {
        Human h = move_kv.first;
        boost::icl::interval_map<Time,LocIll,boost::icl::partial_enricher> single;
        const MoveMap& mm = move_kv.second;
        for (const auto& mm_kv: mm) {
            const auto& duration = mm_kv.first;
            Site s = mm_kv.second;
            single += make_pair(duration, LocIll(s, 0));
        }
        if (ill.find(h) != ill.end()) {
            const IllMap& im = ill.at(h);
            for (const auto& im_kv: im) {
                const auto& duration = im_kv.first;
                Kappa k = im_kv.second;
                single += make_pair(duration, LocIll(0, k));
            }
        }
        li_map[h] = single;
    }
    return(li_map);
}


/*!
 *  Returns a map from a (site, time-step) pair to a set of integrated biting weights.
 *  The integrated biting weight is a tuple of (human, kappa, weight, duration).
 */
BitingWeightIMap
human_availability(
    const std::vector<Site>& sites,
    const std::vector<Human>& humans,
    const TimeStep& time_step,
    const HumanState& human
) {
    BitingWeightIMap bw_map;
    for (const auto& move_kv: human) {
        Human h = move_kv.first;
        Weight w = 1.0;
        const auto& trajectory = move_kv.second;
        //XXX
        int step_idx = 0;
        for (const auto& duration: time_step) {
            auto weights = BitingWeightSet();
            auto during = trajectory & duration;
            for (const auto& during_kv: during) {
                const auto& when = during_kv.first;
                const LocIll& loc_ill = during_kv.second;
                weights += {
                    std::make_tuple(h, loc_ill.ill, w, upper(when) - lower(when))
                    };
                }
            bw_map[make_tuple(loc_ill.loc,step_idx)] = weights;
            ++step_idx;
        }
    }
    return bw_map;
}


void test_humans_at_site_simple() {

    std::vector<LocType> human{
        {1, 3, 0.0},
        {2, 1, 0.0},
        {2, 3, 1.75},
        {1, 3, 3.0}
    };

    auto locs = human_location(human, 10.0);
    std::vector<Site> sites = {1, 2, 3};

    std::vector<Site> location;
    std::vector<int> period;
    std::vector<double> integrated;
    tie(location, period, integrated) = humans_at_site(0.0, 4.0, 8, sites, locs);
    for (size_t pidx = 0; pidx < location.size(); ++pidx) {
        cout << location[pidx] << " " << period[pidx] << " " << integrated[pidx] << endl;
    }
}


void test_make_level()
{
    vector<IllType> human{
        {1, 0.0, 0.0},
        {2, 0.9, 0.0},
        {1, 1.0, 3.0},
        {2, 0.0, 3.2}
    };

    auto infections = human_infection(human, 10.0);
}


void test_construction()
{
    auto rng = make_rng(292342432);
    cout << "rng type: " << type_name<decltype(rng)>() << endl;
    exponential_distribution<double> edist;
    double draw = edist(rng, 5.0);
    cout << "draw is " << draw << endl;

    vector<int> human_moves{0, 0, 1, 1, 5, 5, 10, 10};

    size_t human_cnt = human_moves.size();

    vector<int> human_location;
    vector<int> location;
    vector<double> when_location;
}


typedef std::set<int> Visitors;
// How do intervals work?
void test_set_intervals()
{
    auto v = Visitors();
    v.insert(3);
    v.insert(7);
    Visitors v2 = {2, 4};

    interval_map<double,Visitors> site;
    site.add(make_pair(interval<double>::right_open(0.0, 10.0), v));
    site.add(make_pair(interval<double>::right_open(10.0, 20.0), v2));
    for (
        auto it = site.begin();
        it != site.end();
        ++it
    ) {
        auto when = it->first;
        auto& who = it->second;
        cout << when << ": " << who << " " << interval_count(site) << endl; 
    }
}


int main(int argc, char* argv[])
{
    test_humans_at_site_simple();
    test_construction();
    test_set_intervals();
    auto rng = boost::random::lagged_fibonacci607(2938742);
    int human_cnt = 100;
    int site_cnt = 5;
    double start_time = 0.0;
    double end_time = start_time + 10.0;
    auto events = make_events(rng, human_cnt, site_cnt, start_time);
    auto ill = get<0>(events);
    show_events(ill, "ill");
    auto ill_interval = human_infection(ill, end_time);
    show_intervals(ill_interval, "ill");
    assert(intervals_are_complete(ill_interval, start_time));

    auto move = get<1>(events);
    show_events(move, "move");
    auto move_interval = human_location(move, end_time);
    assert(intervals_are_complete(move_interval, start_time));

    return(0);
}
