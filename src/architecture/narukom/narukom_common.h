#ifndef NARUKOM_COMMON_H
#define NARUKOM_COMMON_H
/**
In this file we use all the system wide definitions for our system

*/

/**

Definitions for topic tree subscription
0: subscribe only on topic
1: subscribe on topic and also to all nodes across the path to root

*/
enum FilterState{ Rejected = 0, Accepted,Unknown};
#define ON_TOPIC 0
#define ABOVE_TOPIC 1
#define BELOW_TOPIC 2
#define ALL -1

#endif