#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::cout;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

#define HZ 1000;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = std::stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line1, line2, line3, line4;
  string key1, key2;
  float MemTotal, MemFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    std::getline(stream, line1);
    std::istringstream linestream(line1);
    linestream>>key1>>MemTotal;
    linestream>>key2>>MemFree;
    return (MemTotal-MemFree)/MemTotal;
  }
  return  0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string TimeLine;
  string uptime, idletime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream, TimeLine);
    std::istringstream linestream(TimeLine);
    linestream>>uptime>>idletime;
    long long_uptime = std::stol(uptime);
    return long_uptime;
  }
  return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
//details: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  vector<string> values;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream>>value){
      values.push_back(value);
    }

    long uptime= std::stol(values[13]);
    long stime= std::stol(values[14]);
    long cutime= std::stol(values[15]);
    long cstime= std::stol(values[16]);
    //long starttime= std::stol(values[21]);

    long totaltime= uptime + stime + cutime + cstime;
    long seconds = totaltime / sysconf(_SC_CLK_TCK);
    return seconds;
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto jiffies= CpuUtilization();
  long total_active_jiffies= std::stol(jiffies[CPUStates::kUser_]) + std::stol(jiffies[CPUStates::kNice_]) + std::stol(jiffies[CPUStates::kSystem_])
  + std::stol(jiffies[CPUStates::kIRQ_]) + std::stol(jiffies[CPUStates::kSoftIRQ_]) + std::stol(jiffies[CPUStates::kSteal_]);
  return total_active_jiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto jiffies=CpuUtilization();
  long total_idle_jiffies= std::stol(jiffies[CPUStates::kIdle_]) + std::stol(jiffies[CPUStates::kIOwait_]);
  return total_idle_jiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, cpu, value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>>cpu;
    while(linestream>>value){
      jiffies.push_back(value);
    }
    return jiffies;
  }
  return {}; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, line;
  int totalproc;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream>>key>>totalproc;
      if(key=="processes"){
        return totalproc;
      }
    }
  }
  return 0; 
  }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int procs_running; 
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream>>key>>procs_running;
      if(key=="procs_running"){
        return procs_running;
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command (int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    return line;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
        std::istringstream linestream(line);
        linestream>>key>>value;
        if(key=="VmSize:"){
          //to return in kb uncomment this line
          //return value;
          long mem= std::stol(value);
          mem/=1000;
          return to_string(mem);
        }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, uid;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream>>key>>uid;
      if(key=="Uid:"){
        return uid;
      }
    }

  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user,x,id;
  string uid=Uid(pid);
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      linestream>>user>>x>>id;
      if(id==uid){
        return user;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, value;
  vector<string> values;
  //long starttime=0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    while(linestream>>value){
      values.push_back(value);
    }

    long startTime = (values[21]=="")? 0:std::stol(values[21]);
    return UpTime() - startTime/sysconf(_SC_CLK_TCK);

  }

  return 0; 
}

float LinuxParser::CpuUtilization(int pid) {
  string line,value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);

    while(linestream>>value){
        values.push_back(value);
    }

    float utime = (values[13]=="")? 0:std::stol(values[13]);
    float stime = (values[14]=="")? 0:std::stol(values[14]);
    float cutime = (values[15]=="")? 0:std::stol(values[15]);
    float cstime = (values[16]=="")? 0:std::stol(values[16]);

    float total_time = (utime + stime +cutime + cstime)/sysconf(_SC_CLK_TCK);

    float uptime = UpTime(pid);
    //uptime= (uptime-0>0.001)? 0.001:uptime;
    return total_time/uptime;
  }
  return 0;
}
