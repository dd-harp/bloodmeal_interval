#include <map>
#include <vector>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "bloodmeal.hpp"

// The partial_enricher says the map must represent 0 instead of dropping it.

/* Translates from events to durations with illness level.
 */
std::map<Human,IllMap>
human_infection(const std::vector<IllType>& human, double end_time);



/* Translates from events to duration at site ID.
 */
std::map<Human,MoveMap>
human_location(const std::vector<LocType>& human, double end_time);


// Sort illness records by time
bool illcmp(const IllType& a, const IllType& b);


// Constructs a random set of illness input data.
template<typename INTERVAL,typename RNG>
std::vector<IllType>
make_illness(int human_cnt, const INTERVAL& duration, RNG& rng)
{
    boost::random::uniform_real_distribution<double> dist(lower(duration), upper(duration));
    std::vector<int> human_sick(human_cnt);
    for (int hinit_idx = 0; hinit_idx < human_cnt; ++hinit_idx) {
        human_sick[hinit_idx] = hinit_idx % 4;
    }
    std::vector<IllType> event;
    for (int sidx = 0; sidx < human_cnt; ++sidx) {
        if (human_sick[sidx] == 0) {
            // do nothing
            event.push_back(std::make_tuple(sidx, 0.0, lower(duration)));
        } else if (human_sick[sidx] == 1) {
            // get sick
            event.push_back(std::make_tuple(sidx, 0.0, lower(duration)));
            event.push_back(std::make_tuple(sidx, 0.6, dist(rng)));
            event.push_back(std::make_tuple(sidx, 1.0, dist(rng)));
        } else if (human_sick[sidx] == 2) {
            // recover
            event.push_back(std::make_tuple(sidx, 1.0, lower(duration)));
            event.push_back(std::make_tuple(sidx, 0.0, dist(rng)));
        } else if (human_sick[sidx] == 3) {
            // Just ill
            event.push_back(std::make_tuple(sidx, 1.0, lower(duration)));
        }
    }
    std::sort(event.begin(), event.end(), illcmp);
    return(event);
}


// Sort location records by time
bool loccmp(const LocType& a, const LocType& b);


// Constructs a random set of movement data.
template<typename INTERVAL,typename RNG>
std::vector<LocType>
make_moves(int human_cnt, int site_cnt,
    const INTERVAL& duration, RNG& rng)
{
    boost::random::uniform_real_distribution<double> dist(lower(duration), upper(duration));
    std::vector<int> mover(human_cnt);
    for (int hinit_idx = 0; hinit_idx < human_cnt; ++hinit_idx) {
        if (hinit_idx < human_cnt / 4) {
            mover[hinit_idx] = 0;
        } else {
            mover[hinit_idx] = 1;
        }
    }
    // Any place can be home.
    boost::random::uniform_int_distribution<int> home(0, site_cnt - 1);
    // Don't travel to lower-numbered sites.
    boost::random::uniform_int_distribution<int> dest(site_cnt / 4, site_cnt - 1);
    // Make between 0 and 5 trips during the interval.
    boost::random::uniform_int_distribution<int> trips(0, 5);

    std::vector<LocType> event;
    for (int sidx = 0; sidx < human_cnt; ++sidx) {
        if (mover[sidx] == 0) {
            // do nothing
            event.push_back(std::make_tuple(sidx, home(rng), lower(duration)));
        } else if (mover[sidx] == 1) {
            // travel
            event.push_back(std::make_tuple(sidx, home(rng), lower(duration)));
            int trip_cnt = trips(rng);
            for (int trip_idx = 0; trip_idx < trip_cnt; ++trip_idx) {
                event.push_back(std::make_tuple(sidx, dest(rng), dist(rng)));
            }
        }
    }
    sort(event.begin(), event.end(), loccmp);
    return(event);
}


// Checks the input to see that intervals cover the time period.
template<typename MAP>
bool intervals_are_complete(const MAP& imap, double start_time) {
    bool meets = true;
    std::cout << "intervals_are_complete" << std::endl;
    for (const auto& kv: imap) {
        Human h = kv.first;
        const auto& interval = kv.second;
        double last = start_time;
        for (const auto& ikv: interval) {
            double low = lower(ikv.first);
            double high = upper(ikv.first);
            std::cout << "h " << h << " [" << low << ", " << high
                << ")" << std::endl;
            if (low - last > 1e-9) {
                meets = false;
            }
            last = high;
        }
        if (start_time + 10.0 - last > 1e-9) {
            meets = false;
        }
    }
    return meets;
}


template<typename RNG>
std::tuple<std::vector<IllType>,std::vector<LocType>>
make_events(RNG& rng, Site human_cnt, Site site_cnt, double start_time) {
    auto duration = boost::icl::interval<Time>::right_open(
        start_time, start_time + 10.0);
    auto illness = make_illness(human_cnt, duration, rng);
    auto moves = make_moves(human_cnt, site_cnt, duration, rng);
    return(std::make_tuple(illness, moves));
}


// Print events
// T is a vector of tuples of events which have three parts.
template<typename T>
void show_events(const T& e, const std::string& name) {
    std::cout << "show_events " << name << std::endl;
    for (const auto& row: e) {
        std::cout << std::get<0>(row) << ", " << std::get<1>(row) <<
            ", " << std::get<2>(row) << std::endl;
    }
}


// Print intervals
// T is a map from human to an interval map. That map has keys and values.
template<typename T>
void show_intervals(const T& e, const std::string& name) {
    std::cout << "show_intervals " << name << std::endl;
    for (const auto& hi: e) {
        Human h = hi.first;
        const auto& inter = hi.second;
        for (const auto& iv: inter) {
            double low = boost::icl::lower(iv.first);
            double high = boost::icl::upper(iv.first);
            const auto& val = iv.second;
            std::cout << "h " << h << " [" << low << ", " << high <<
                ") " << val << std::endl;
        }
    }
}
