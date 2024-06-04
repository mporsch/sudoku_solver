#pragma once

#include "args.h"

#include <iostream>

struct Usage
{
  const char* name;
};

std::ostream& operator<<(std::ostream&, const Usage&);

Args ParseArgs(int, char**);
