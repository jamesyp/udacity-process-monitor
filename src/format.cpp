#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours, minutes;

  minutes = seconds / 60;
  seconds = seconds % 60;

  hours = minutes / 60;
  minutes = minutes % 60;

  std::ostringstream oss;
  oss << std::setfill('0') << hours << ":" << std::setw(2) << minutes << ":"
      << std::setw(2) << seconds;

  return oss.str();
}
