#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <numeric>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream stream(kOSPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream lstream(line);
      while (lstream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lstream(line);
    lstream >> os >> version >> kernel;
  }
  return kernel;
}

float LinuxParser::MemoryUtilization() {
  string line, key;
  float value, memTotal = 0.0, memAvailable = 0.0;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lstream(line);
      lstream >> key >> value;
      if (key == "MemTotal:") {
        memTotal = value;
      }
      if (key == "MemAvailable:") {
        memAvailable = value;
      }
      if (memTotal != 0.0 && memAvailable != 0.0) {
        break;
      }
    }
  }

  return (memTotal - memAvailable) / memTotal;
}

int LinuxParser::TotalProcesses() { return Pids().size(); }

int LinuxParser::RunningProcesses() {
  string line, key, value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lstream(line);

      while (lstream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }

  return 0;
}

long LinuxParser::UpTime() {
  long uptime_seconds;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lstream(line);
    lstream >> uptime_seconds;
  }

  return uptime_seconds;
}

long LinuxParser::TotalJiffies() {
  vector<long> jiffies = Jiffies();

  return std::accumulate(jiffies.cbegin(), jiffies.cend(), 0);
}

long LinuxParser::ActiveJiffies() { return TotalJiffies() - IdleJiffies(); }

long LinuxParser::IdleJiffies() { return Jiffies()[CPUStates::kIdle_]; }

vector<long> LinuxParser::Jiffies() {
  string line, label;
  vector<long> jiffies;
  long value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lstream(line);

    lstream >> label;
    while (lstream >> value) {
      jiffies.push_back(value);
    }
  }

  return jiffies;
}

// --------
// Processes
// --------

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
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);

  return pids;
}

string LinuxParser::User(int pid) {
  string line, username, _, passwdUid;
  string processUid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lstream(line);

      lstream >> username >> _ >> passwdUid;

      if (passwdUid == processUid) {
        return username;
      }
    }
  }

  return string();
}

string LinuxParser::Uid(int pid) {
  string line, label, uid;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lstream(line);
      lstream >> label >> uid;

      if (label == kUid_) {
        return uid;
      }
    }
  }

  return string();
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }

  return string();
}

int LinuxParser::RamKB(int pid) {
  string line, label, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lstream(line);
      lstream >> label >> value;

      if (label == kRam_) {
        return std::stoi(value);
      }
    }
  }

  return 0;
}

long LinuxParser::UpTime(int pid) {
  vector<string> values = StatFields(pid);
  if (values.size() == 0) {
    return 0;
  }

  long uTime = std::stol(values[StatFields::uTime]);
  long sTime = std::stol(values[StatFields::sTime]);

  long time = (uTime + sTime) / sysconf(_SC_CLK_TCK);
  return time;
}

long LinuxParser::StartTime(int pid) {
  vector<string> values = StatFields(pid);
  if (values.size() == 0) {
    return 0;
  }

  return std::stol(values[StatFields::startTime]) / sysconf(_SC_CLK_TCK);
}

vector<string> LinuxParser::StatFields(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);

    std::istringstream lstream(line);
    vector<string> result{std::istream_iterator<string>(lstream), {}};

    return result;
  }
  return vector<string>{};
}
