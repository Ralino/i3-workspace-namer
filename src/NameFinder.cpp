#include "NameFinder.h"

#include <sys/types.h>

void NameFinder::addName(const std::string& name,
                         const std::string& wm_class,
                         const std::string& wm_name_regex)
{
  m_names.push_back(name);
  unsigned index = m_names.size() - 1;
  auto class_it = m_wm_classes.emplace(wm_class).first;
  class_it->second.addWMName(wm_name_regex, index);
}

std::string NameFinder::getName(const std::string& wm_class, const std::string& wm_name) const
{
  auto class_it = m_wm_classes.find(wm_class);
  if (class_it == m_wm_classes.end())
  {
    class_it = m_wm_classes.find("DEFAULT"); //FIXME
    if (class_it == m_wm_classes.end())
    {
      return "";
    }
  }

  long index = class_it->second.getName(wm_name);
  if (index < 0 || (unsigned) index >= m_names.size()) //FIXME
  {
    return wm_class;
  }

  return m_names[index];
}

NameMapper::NameMapper()
  : m_default_name_index(-1)
{
}

void NameMapper::addWMName(const std::string& wm_name_regex, unsigned name_index)
{
  if (wm_name_regex.empty())
  {
    m_default_name_index = name_index;
  }
  {
    NameMap nm;
    int res = regcomp(&nm.regex, wm_name_regex.c_str(), REG_EXTENDED | REG_NOSUB);
    if (res != 0)
    {
      size_t err_buff_size = regerror(res, &nm.regex, NULL, 0);
      char* err_buff = new char[err_buff_size];
      regerror(res, &nm.regex, err_buff, err_buff_size);
      throw std::runtime_error(err_buff); //TODO improve error handling
    }

    nm.name_index = name_index;

    m_wm_names.push_back(nm);
  }
}

long NameMapper::getName(const std::string& wm_name) const
{
  for (NameMap nm : m_wm_names)
  {
    int res = regexec(&nm.regex, wm_name.c_str(), 0, NULL, 0);
    if (res == 0)
    {
      return nm.name_index;
    }
  }
  //No match found
  return m_default_name_index;
}

NameMapper::NameMap::~NameMap()
{
  regfree(&regex); //FIXME needed?
}
