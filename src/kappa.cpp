#include <map>
#include <boost/icl/interval_map.hpp>
#include <boost/icl/interval_set.hpp>
#include <algorithm>
#include "bloodmeal.hpp"

using namespace std;
using namespace boost::icl;

// Kappa is the sum of all PR at a site.
std::map<Site,interval_map<Time,Kappa>>
site_kappa(
    const std::vector<Site>& sites,
    const std::map<Human,interval_map<Time,Site>>& infection,
    const std::map<Human,interval_map<Time,Site>>& location
) {
    // infection and location are by-human, not by-site.
    std::map<Site,interval_map<Time,Kappa>> site_kappa;

    for (int site: sites) {
        interval_map<Time,Kappa> kappa;
        // Loop over all people who went anywhere.
        for (auto& loc_kv: location) {
            Human hid = loc_kv.first;
            const auto& loc_map = loc_kv.second;
            auto inf_iter = infection.find(hid);
            if (inf_iter != infection.end()) {
                const auto& inf_map = inf_iter->second;

                for (const auto& loc_int: loc_map) {
                    if (loc_int.second == site) {
                        kappa += inf_map & loc_int.first;
                    }
                }
            } // else no infection for this person ever
        }
        site_kappa[site] = kappa;
    }

    return(site_kappa);
}


std::map<Site,interval_map<Time,Kappa>>
duration_kappa(
    std::map<Site,interval_map<Time,Kappa>> site_kappa,
    interval_set<Time> durations
) {
    std::map<Site,interval_map<Time,Kappa>> by_site;

    for (const auto& site_iter: site_kappa) {
        Site site = site_iter.first;
        const auto& kappa_map = site_iter.second;

        interval_map<double,double> site_split;
        for (const auto& duration: durations) {
            auto in_duration = kappa_map & duration;
            Kappa total_kappa = 0.0;
            for (const auto& sum_iter: kappa_map) {
                const auto& interval = sum_iter.first;
                total_kappa += (upper(interval) - lower(interval)) * sum_iter.second;
            }
            site_split += make_pair(duration, total_kappa);
        }
        by_site[site] = site_split;
    }
    return(by_site);
}
