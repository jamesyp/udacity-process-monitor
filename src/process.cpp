#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
  command_ = LinuxParser::Command(pid_);
}

int Process::Pid() { return pid_; }

string Process::User() { return user_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

int Process::RamMB() const { return LinuxParser::RamKB(pid_) / 1000; }

long int Process::UpTime() { return uptime_; }

string Process::Command() { return command_; }

bool Process::operator<(Process const& a) const { return RamMB() > a.RamMB(); }
