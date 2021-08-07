#include <map>
#include <vector>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/split_interval_map.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>

std::map<int,boost::icl::interval_map<double,double>>
human_infection(
    const std::vector<int>& human,
    const std::vector<double>& level,
    const std::vector<double>& when
    );

std::map<int,boost::icl::interval_map<double,int>>
human_location(
    const std::vector<int>& human,
    const std::vector<int>& site,
    const std::vector<double>& when
    );




typedef std::tuple<int,double,double> IllType;
bool illcmp(const IllType& a, const IllType& b);


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
        if (sidx == 0) {
            // do nothing
            event.push_back(std::make_tuple(sidx, 0.0, lower(duration)));
        } else if (sidx == 1) {
            // get sick
            event.push_back(std::make_tuple(sidx, 0.0, lower(duration)));
            event.push_back(std::make_tuple(sidx, 1.0, dist(rng)));
        } else if (sidx == 2) {
            // recover
            event.push_back(std::make_tuple(sidx, 1.0, lower(duration)));
            event.push_back(std::make_tuple(sidx, 0.0, dist(rng)));
        } else if (sidx == 3) {
            // Just ill
            event.push_back(std::make_tuple(sidx, 1.0, lower(duration)));
        }
    }
    std::sort(event.begin(), event.end(), illcmp);
    return(event);
}


typedef std::tuple<int,int,double> LocType;
bool loccmp(const LocType& a, const LocType& b);


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
    boost::random::uniform_int_distribution<int> home(0, site_cnt - 1);
    boost::random::uniform_int_distribution<int> dest(site_cnt / 4, site_cnt - 1);
    boost::random::uniform_int_distribution<int> trips(0, 5);

    std::vector<LocType> event;
    for (int sidx = 0; sidx < human_cnt; ++sidx) {
        if (sidx == 0) {
            // do nothing
            event.push_back(std::make_tuple(sidx, home(rng), lower(duration)));
        } else if (sidx == 1) {
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
