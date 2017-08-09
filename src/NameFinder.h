#ifndef NAME_FINDER_H
#define NAME_FINDER_H

#include <vector>
#include <map>
#include <string>
#include <regex.h>

class NameMapper;

class NameFinder
{
public:
  NameFinder();

  void addName(const std::string& name,
               const std::string& wm_class_regex,
               const std::string& wm_name_regex);

  std::string getName(const std::string& wm_class, const std::string& wm_name) const;

private:
  std::map<std::string, NameMapper> wm_classes;
  std::vector<std::string> names;
};

class NameMapper
{
public:
  NameMapper();

  void addWMName(const std::string& wm_name_regex, unsigned name_index);

  long getName(const std::string& wm_name) const;

private:
  typedef struct
  {
    regex_t regex;
    unsigned name_index;
  } NameMap;

  std::vector<NameMap> wm_names;
  long default_name_index;
};

#endif //NAME_FINDER_H
