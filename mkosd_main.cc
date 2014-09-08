
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "prep_tmpfs.h"

namespace po = boost::program_options;
using namespace std;

int main(int argc, char* argv[])
{
  try {
    po::options_description desc("options");
    string param_file;
    string output_dir;

    desc.add_options()
      ("param_file",
       po::value<string>(&param_file)->default_value("params.ini"),
       "location of param ini file")
      ("output_dir",
       po::value<string>(&output_dir)->default_value("/tmp/mkosd_dir"),
       "output directory")
      ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    cout << "param file: " << param_file << endl;
    cout << "output dir: " << output_dir << endl;
  }

  catch(exception& e) {
    cerr << __func__ << " error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    cerr << __func__ << " unknown exception" << endl;
  }

  return 0;
}
