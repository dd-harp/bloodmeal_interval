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

using namespace std;
using namespace boost::icl;
using namespace boost::random;


void test_humans_at_site_simple() {
    vector<int> human;
    vector<int> site;
    vector<double> when;
    // 1 in 3 at 0.0  [(0.0, 2.0), 3]
    human.push_back(1);
    site.push_back(3);
    when.push_back(0.0);
    // 2 in 1 at 0.0  [(0.0, 1.5), 1]
    human.push_back(2);
    site.push_back(1);
    when.push_back(0.0);
    // 1 in 2 at 3.0  [(3.0, Inf), 2]
    human.push_back(1);
    site.push_back(2);
    when.push_back(3.0);
    // 2 in 3 at 1.5  [(1.75, Inf), 3]
    human.push_back(2);
    site.push_back(3);
    when.push_back(1.75);
    auto locs = human_location(human, site, when);
    std::vector<int> sites = {1, 2, 3};

    std::vector<int> location;
    std::vector<int> period;
    std::vector<double> integrated;
    tie(location, period, integrated) = humans_at_site(0.0, 4.0, 8, sites, locs);
    for (size_t pidx = 0; pidx < location.size(); ++pidx) {
        cout << location[pidx] << " " << period[pidx] << " " << integrated[pidx] << endl;
    }
}


void test_make_level()
{
    vector<int> human;
    vector<double> level;
    vector<double> when;

    human.push_back(1);
    level.push_back(0.0);
    when.push_back(0.0);
    human.push_back(2);
    level.push_back(0.9);
    when.push_back(0.0);
    human.push_back(1);
    level.push_back(1.0);
    when.push_back(3.0);
    human.push_back(2);
    level.push_back(0.0);
    when.push_back(3.2);
    auto infections = human_infection(human, level, when);    
}


void test_construction()
{
    auto rng = boost::random::lagged_fibonacci607(2938742);
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

void test_set_intervals()
{
    auto v = Visitors();
    v.insert(3);
    v.insert(7);

    interval_map<double,Visitors> site;
    site.add(make_pair(interval<double>::right_open(0.0, 10.0), v));
    for (
        auto it = site.begin();
        it != site.end();
        ++it
    ) {
        auto when = it->first;
        auto& who = it->second;
        cout << when << ": " << who << endl; 
    }
}


void make_stuff() {
    auto rng = boost::random::lagged_fibonacci607(2938742);
    int human_cnt = 100;
    int site_cnt = 5;
    auto duration = interval<double>::right_open(0.0, 10.0);
    auto illness = make_illness(human_cnt, duration, rng);
    auto moves = make_moves(human_cnt, site_cnt, duration, rng);
}


int main(int argc, char* argv[])
{
    test_humans_at_site_simple();
    test_construction();
    test_set_intervals();

    return(0);
}
