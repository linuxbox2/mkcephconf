#ifndef MKOSD_CONFIG_H
#define MKOSD_CONFIG_H

#include<string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/exception.hpp>

namespace pt = boost::property_tree;
namespace bf = boost::filesystem;

using boost::system::system_error;
using namespace boost::system::errc;

static inline std::string& sq(std::string& s) { // strip quotes
  if (s.front() == '"') {
    s.erase(0, 1);
    if (s.back() == '"')
      s.erase(s.size()-1, 1);
  }
  return s;
}

class MkOSD_Config
{
 public:
  MkOSD_Config(const std::string& param_file) {
    bf::path p(param_file);
    if (! bf::is_regular_file(p)) {
      throw
	bf::filesystem_error(param_file,
			     make_error_code(no_such_file_or_directory));
    }
    // read in as a .ini file
    pt::ini_parser::read_ini(param_file, pt);
  }
 public:
  pt::ptree pt;
  std::vector<std::string>osd_devs;
};

#endif /* MKOSD_CONFIG_H */
