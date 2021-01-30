#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  Process(int pid);

  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();  // TODO: See src/process.cpp
  int RamMB() const;
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
  std::string user_;
  long uptime_;
  std::string command_;
};

#endif
