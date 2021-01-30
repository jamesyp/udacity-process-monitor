#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>

#include "process.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  processes_.clear();

  auto pids = LinuxParser::Pids();
  for (vector<int>::iterator pid = pids.begin(); pid != pids.end(); pid++) {
    processes_.push_back(Process(*pid));
  }

  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() {
  if (kernel == "") {
    kernel = LinuxParser::Kernel();
  }

  return kernel;
}

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {
  if (os == "") {
    os = LinuxParser::OperatingSystem();
  }

  return os;
}

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
