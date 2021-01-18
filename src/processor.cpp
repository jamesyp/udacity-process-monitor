#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  curTotalJiffies = LinuxParser::TotalJiffies();
  curActiveJiffies = LinuxParser::ActiveJiffies();

  if (this->prevTotalJiffies == 0) {
    prevTotalJiffies = curTotalJiffies;
    prevActiveJiffies = curActiveJiffies;

    return 0.0;
  }

  long deltaTotal = curTotalJiffies - prevTotalJiffies;
  long deltaActive = curActiveJiffies - prevActiveJiffies;

  prevTotalJiffies = curTotalJiffies;
  prevActiveJiffies = curActiveJiffies;

  return (float)deltaActive / float(deltaTotal);
}
