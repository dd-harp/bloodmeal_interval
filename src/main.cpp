#include <assert.h>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>

#include "humans_at_site.hpp"

using namespace std;
using namespace boost::icl;


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
    std::vector<int> location;
    std::vector<int> period;
    std::vector<double> integrated;
    humans_at_site(0.0, 4.0, 8, human, site, when, location, period, integrated);
    for (size_t pidx = 0; pidx < location.size(); ++pidx) {
        cout << location[pidx] << " " << period[pidx] << " " << integrated[pidx] << endl;
    }
}


typedef std::set<int> Visitors;

int main(int argc, char* argv[])
{
    test_humans_at_site_simple();

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
    return(0);
}
