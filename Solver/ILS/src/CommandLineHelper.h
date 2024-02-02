#ifndef COMMAND_LINE_HELPER_H
#define COMMAND_LINE_HELPER_H

#include "Instance.h"

class CommandLineHelper
{
public:
    static Instance* getInstanceFromCommandLine(int argc, char* argv[] );       // Get params from Command Line  
private:
    static void printHelper();                                                  // Print expected formatting if 
};

#endif