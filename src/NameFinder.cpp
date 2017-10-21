#include "NameFinder.h"

#include <sys/types.h>

void NameFinder::addName(const std::string& name,
                         const std::string& wm_class,
                         const std::string& title_regex)
{
  m_names.push_back(name);
  unsigned index = m_names.size() - 1;
  auto class_it = m_wm_classes.emplace(wm_class, NameMapper{}).first;
  class_it->second.addTitle(title_regex, index);
}

std::string NameFinder::getName(const std::string& wm_class, const std::string& title) const
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

  long index = class_it->second.getName(title);
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

void NameMapper::addTitle(const std::string& title_regex, unsigned name_index)
{
  if (title_regex.empty())
  {
    m_default_name_index = name_index;
  }
  {
    NameMap nm;
    int res = regcomp(&nm.regex, title_regex.c_str(), REG_EXTENDED | REG_NOSUB);
    if (res != 0)
    {
      size_t err_buff_size = regerror(res, &nm.regex, NULL, 0);
      std::vector<char> err_buff(err_buff_size);
      regerror(res, &nm.regex, err_buff.data(), err_buff_size);
      throw std::runtime_error(err_buff.data()); //TODO improve error handling
    }

    nm.name_index = name_index;

    m_titles.push_back(nm);
  }
}

long NameMapper::getName(const std::string& wm_name) const
{
  for (NameMap nm : m_titles)
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
