#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long curTotalJiffies = 0, curActiveJiffies = 0;
  long prevTotalJiffies = 0, prevActiveJiffies = 0;
};

#endif
