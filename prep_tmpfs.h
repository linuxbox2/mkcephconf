#ifndef PREP_TMPFS_H
#define PREP_TMPFS_H

#include<string>
#include <cstdlib>
#include <boost/format.hpp>
#include "mkosd_config.h"

class MkOSD_Tmpfs
{
 public:
  MkOSD_Tmpfs(MkOSD_Config& cfg) {
    using namespace std;
    n_osds = cfg.pt.get<int>("mkosd.n_osds");
    tmpfs_size_mb = cfg.pt.get<int>("mkosd.tmpfs_size_mb");
    data_dir = cfg.pt.get<string>("mkosd.data_dir");
    sq(data_dir);

    bf::path p_data_dir(data_dir);
    cout << "data_dir:" << data_dir << " p:" << p_data_dir << endl;
    if (! bf::is_directory(p_data_dir)) {
      throw
	bf::filesystem_error(data_dir,
			     make_error_code(no_such_file_or_directory));
    }
    int osd_ix;
    string osd, cmd;
    umount_all(p_data_dir);
    for (osd_ix = 0; osd_ix < n_osds; ++osd_ix) {
      osd = string{"osd"} + to_string(osd_ix);
      bf::path osd_p(p_data_dir);
      osd_p /= osd;
      cout << "create osd: " << osd_p << endl;
      bf::create_directory(osd_p);
      mkfs_tmpfs(osd_p, tmpfs_size_mb);
    }
  } // MkOSD_Tmpfs(MkOSD_Config& cfg)

  void mkfs_tmpfs(const bf::path& p, int size) {
    using namespace std;
    boost::format f("mount -t tmpfs -o size=%1%m tmpfs %2%");
    f % size;
    string s = p.native();
    f % sq(s); // str()
    string cmd = f.str();
    //cout << cmd << endl;
    ::system(cmd.c_str());
  } // mkfs_tmpfs

  void umount_all(const bf::path& ddir) {
    using namespace std;
    if (bf::is_directory(ddir)) {
      string cmd;
      for (bf::directory_iterator d_iter(ddir);
	   d_iter != bf::directory_iterator(); ++ d_iter) {
	bf::path p(*d_iter);
	if (is_directory(p)) {
	  boost::format f("umount -f %1% 2>/dev/null");
	  f % p.native();
	  cmd = f.str();
	  //cout << "cmd: " << cmd << endl;
	  ::system(cmd.c_str());
	}
      }
    }
  } // umount_all

 public:
  int n_osds;
  int tmpfs_size_mb;
  std::string data_dir;
};

#endif /* PREP_TMPFS_H */
