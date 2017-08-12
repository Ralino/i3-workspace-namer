#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "NameFinder.h"

#include <json/json.h>
#include <istream>

namespace config_parser {

NameFinder readConfig(std::istream* config);

}

#endif //CONFIGPARSER_H
