#pragma once

#include <vector>
#include <map>
#include <string>
#include <regex.h>

class NameMapper;

class NameFinder
{
public:
  typedef long NameIndex;
  void addName(const std::string& name,
               const std::string& wm_class,
               const std::string& title_regex);

  std::string getName(const std::string& wm_class, const std::string& title) const;

private:
  std::map<std::string, NameMapper> m_wm_classes;
  std::vector<std::string> m_names;
};

class NameMapper
{
public:
  NameMapper();

  void addTitle(const std::string& title_regex, NameFinder::NameIndex name_index);

  NameFinder::NameIndex getName(const std::string& title) const;

private:
  struct NameMap
  {
    regex_t regex; //FIXME regfree(..)?
    NameFinder::NameIndex name_index;
  };

  std::vector<NameMap> m_titles;
  NameFinder::NameIndex m_default_name_index;
};
