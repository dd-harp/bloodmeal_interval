#ifndef _BLOODMEAL_HPP_
#define _BLOODMEAL_HPP_ 1

// This header defines types for use throughout the program.
typedef int Human;
typedef int Site;
typedef double Time;
typedef double Kappa;  // Infectiousness of an animal.
typedef double Weight; // Biting weight of an animal.
typedef int Bites;
typedef int Infectious; // Whether a mosquito is infective.

// This class aggregates information about a person in a duration.
// It implements a += operator so that the Boost Interval Component Library
// can combine intervals with location and intervals with illness.
class LocIll
{
public:
    LocIll(Site l, Kappa i) : loc(l), ill(i) {}
    // When you add a LocIll to another LocIll, as the value of an interval_map,
    // it calls this +=, and this one is designed to combine location and illness information.
    LocIll& operator +=(const LocIll& right) {
        if (right.loc > this->loc) {
            this->loc = right.loc;
        }
        if (right.ill > this->ill) {
            this->ill = right.ill;
        }
        return *this;
    }
    bool operator==(const LocIll& right) {
        return(right.loc == this->loc && right.ill == this->ill);
    }
    Site loc;
    Kappa ill;
};

// Each entry in the illness events vector has these values.
typedef std::tuple<Human,Kappa,Time> IllType;
// Each entry in the location events vector has these values.
typedef std::tuple<Human,Site,Time> LocType;
// Total bites at a site in a time interval, both uninfectious and infectious.
typedef std::tuple<Site,Time,Bites,Bites> BiteRecord;
typedef boost::icl::interval_set<Time> TimeStep;
// These interval maps transform the story from events into durations of time.
typedef boost::icl::interval_map<Time,Kappa,boost::icl::partial_enricher> IllMap;
typedef boost::icl::interval_map<Time,Site,boost::icl::partial_enricher> MoveMap;
// Combines the movement with the illness.
typedef std::map<Human,
    boost::icl::interval_map<Time,LocIll,boost::icl::partial_enricher>> HumanState;
typedef boost::icl::interval_map<Time,std::tuple<Bites,Bites>,boost::icl::partial_enricher> BiteMap;
// Humans ready for sampling. The time is the duration spent at the site.
typedef std::tuple<Human,Kappa,Weight,Time> BitingWeight;
typedef std::set<BitingWeight> BitingWeightSet;
// Site -> Time step -> set of humans with biting weights and durations.
typedef std::map<std::tuple<Site,int>,BitingWeightSet> BitingWeightIMap;
// Output
typedef std::tuple<Human,Kappa,Infectious,Site,Time> Output;

#endif
