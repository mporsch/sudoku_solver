#pragma once

#include <optional>
#include <string>

struct Args
{
  struct Template
  {
    using value_type = size_t;

    value_type width;
    value_type height;
    value_type blockWidth;
    value_type blockHeight;
  };

  bool help = false;
  bool stdinput = false;
  std::optional<Template> templ;
  std::string filepath;
};
