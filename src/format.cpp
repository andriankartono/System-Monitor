#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours, mins, secs;
    hours=seconds/3600;
    int min_sec=seconds%3600;
    mins=min_sec/60;
    secs=min_sec%60;

    //int to string conversion:
    string hour_str=to_string(hours);
    string min_str=to_string(mins);
    string sec_str=to_string(secs);
    return hour_str+":"+min_str+":"+sec_str; 
}