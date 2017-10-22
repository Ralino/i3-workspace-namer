#pragma once

#include "NameFinder.h"

#include <json/json.h>
#include <istream>

namespace config_parser {

NameFinder readConfig(std::istream* config);

}
