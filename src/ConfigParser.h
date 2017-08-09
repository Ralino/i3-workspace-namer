#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <json/json.h>
#include <istream>

namespace config_parser {

void readConfig(std::istream* config);

}
#endif //CONFIGPARSER_H
