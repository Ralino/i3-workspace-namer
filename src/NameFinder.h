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
  void addName(const std::string& name,
               const std::string& wm_class,
               const std::string& wm_name_regex);

  std::string getName(const std::string& wm_class, const std::string& wm_name) const;

private:
  std::map<std::string, NameMapper> m_wm_classes;
  std::vector<std::string> m_names;
};

class NameMapper
{
public:
  NameMapper();

  void addWMName(const std::string& wm_name_regex, unsigned name_index);

  long getName(const std::string& wm_name) const;

private:
  struct NameMap
  {
    ~NameMap();

    regex_t regex;
    unsigned name_index;
  };

  std::vector<NameMap> m_wm_names;
  long m_default_name_index;
};

#endif //NAME_FINDER_H
