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

  void addTitle(const std::string& title_regex, unsigned name_index);

  long getName(const std::string& title) const;

private:
  struct NameMap
  {
    regex_t regex; //FIXME regfree(..)?
    unsigned name_index;
  };

  std::vector<NameMap> m_titles;
  long m_default_name_index;
};

#endif //NAME_FINDER_H
