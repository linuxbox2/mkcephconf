
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "mkosd_config.h"
#include "prep_tmpfs.h"

namespace po = boost::program_options;
using namespace std;

void usage()
{
  cout << "mkosd \n"
    "\t[--param_file=<path_to_params_ini>]\n"
       << endl;
}

int main(int argc, char* argv[])
{
  try {
    po::options_description desc("options");
    string param_file;

    desc.add_options()
      ("help", "print usage info")
      ("param_file",
       po::value<string>(&param_file)->default_value("params.ini"),
       "location of param ini file")
      ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      usage();
      goto out;
    }

    cout << "param file: " << param_file << endl;

    MkOSD_Config cfg(param_file);

  }

  catch(exception& e) {
    cerr << __func__ << " error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << __func__ << " unknown exception" << endl;
  }

 out:
  return 0;
}
