# Calculate Bloodmeal from Humans, Mosquitoes, and Locations

We simulate human infection status, mosquito lifecycles, and human and mosquito movement.
It's difficult to combine information from those sources in order to determine new
infections. The algorithm is complicated and slow to do in R.
This project makes a function in C++ to do that combination, so that we can
call it from R. It takes advantage of the Boost interval containers to do much of the matching. They can [match intervals efficiently](https://www.boost.org/doc/libs/1_76_0/libs/icl/doc/html/boost_icl/examples/party.html).

These functions should answer the following questions.

1. What is the integrated count of humans at a site over a time interval?

2. What is the integrated total infectivity of humans at a site over a time interval?

3. If I assigned a bite to a human at a site, randomly placed over a time interval, to which human would it go? This would take a number from 0 to 1 and assign it to a person, according to time spent at the site.


# The Interface

The humans have infection states that change over time. It could be discrete time or continuous,
and the humans might have different parasite rates (PR), so we will give them a floating-point number
that changes at different time points.

Human PR   Time
===== ===  ====
1     0    0.0
2     0.3  0.0
1     0.9  3.7
2     0    200.0

Locations look similar. There are a fixed number of locations, and humans move from one location
to another at different times.

Human Location Time
===== ======== ====
1     3        0.0
7     2        3.5

The mosquitoes can take one of two main forms. Either you have a count at each site or you have
individual mosquitoes. Let's think about the cloud of mosquitoes. The input is a number of bites
by mosquitoes of each class, where M is uninfected, Y is infected, and Z is infectious.

Site Day M  Y  Z 
==== === == == ==
1    1   20 5  7
2    1   15 8  4

That means that there is a report of the number of mosquitoes at all sites at the same time step.

We want to return the following to Mosquitoes and to Humans. For mosquitoes, it's the number of
bites that were infectious because they landed on an infectious human, and the PR of the humans
bitten.

Site Day Infectious Bites PR
==== === ================ ===
1    1   5                8.3
2    1   7                4.2

For the humans, the output assigns infectious bites to individuals, according to the sites
they visited and the mosquitoes at those sites.

Human Time
===== ====
1     3.7
5     4.9
