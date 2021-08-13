#ifndef _BLOODMEAL_HPP_
#define _BLOODMEAL_HPP_ 1

typedef int Human;
typedef int Site;
typedef double Time;
typedef double Kappa;

typedef std::tuple<Human,Kappa,Time> IllType;
typedef std::tuple<Human,Site,Time> LocType;
typedef boost::icl::interval_map<Time,Kappa,boost::icl::partial_enricher> IllMap;
typedef boost::icl::interval_map<Time,Site,boost::icl::partial_enricher> MoveMap;

#endif
