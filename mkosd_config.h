#include<string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/exception.hpp>

namespace pt = boost::property_tree;
namespace bf = boost::filesystem;

using namespace std;
using boost::system::system_error;
using namespace boost::system::errc;

class MkOSD_Config
{
 public:
  MkOSD_Config(const string& param_file) {
    bf::path p(param_file);
    if (! bf::is_regular_file(p)) {
      throw
	bf::filesystem_error(param_file,
			     make_error_code(no_such_file_or_directory));
      // read in as a .ini file
      pt::ini_parser::read_ini(param_file, pt);
    }
  }
 public:
  pt::ptree pt;

};
