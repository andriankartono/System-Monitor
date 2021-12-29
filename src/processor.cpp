#include "processor.h"

#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
//https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

float Processor::Utilization() { 
    long total_old, total_new, active_new, idle_old, idle_new;
    total_new = NewTotal();
    active_new = NewActive();
    idle_new = NewIdle();

    total_old = OldTotal();
    idle_old = OldIdle();
    
    Update(total_new, active_new, idle_new);

    float totald= total_new - total_old;
    float idled= idle_new - idle_old;
    float utilization= (totald-idled)/totald;
    return utilization;
}

long Processor::NewTotal() const{
    return LinuxParser::Jiffies();
}

long Processor::NewActive() const{
    return LinuxParser::ActiveJiffies();
}

long Processor::NewIdle() const{
    return LinuxParser::IdleJiffies();
}

long Processor::OldTotal() const{
    return total_;
}

long Processor::OldActive() const{
    return active_;
}

long Processor::OldIdle() const{
    return idle_;
}

void Processor::Update(long total, long active, long idle){
    total_=total;
    active_=active;
    idle_=idle;
}