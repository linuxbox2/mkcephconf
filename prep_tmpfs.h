/*
 * Simple ceph.conf automation.
 *
 * Copyright (C) 2014 CohortFS, LLC
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */
#ifndef PREP_TMPFS_H
#define PREP_TMPFS_H

#include <string>
#include <vector>
#include <cstdlib>
#include <boost/format.hpp>
#include <plustache/plustache_types.hpp>
#include <plustache/template.hpp>
#include "mkcephconf_config.h"

class Mkcephconf_Tmpfs
{
 public:
 Mkcephconf_Tmpfs(Mkcephconf_Config& _cfg) : cfg(_cfg) {
    using namespace std;
    n_osds = cfg.pt.get<int>("mkcephconf.n_osds");
    tmpfs_size_mb = cfg.pt.get<int>("mkcephconf.tmpfs_size_mb");
    data_dir = cfg.pt.get<string>("mkcephconf.data_dir");
    sq(data_dir);
    string tplf = cfg.pt.get<string>("mkcephconf.osd_sh_template");
    sq(tplf);
    string tpl = cfg.read_from_file(tplf);

    bf::path p_data_dir(data_dir);
    cout << "data_dir:" << data_dir << " p:" << p_data_dir << endl;

    Plustache::Context ctx2;
    PlustacheTypes::CollectionType mkfs;
    int osd_ix;
    string osd, cmd;

    umount_all(ctx2, p_data_dir);
    for (osd_ix = 0; osd_ix < n_osds; ++osd_ix) {
      osd = string{"osd"} + to_string(osd_ix);
      bf::path osd_p(p_data_dir);
      osd_p /= osd;
      cout << "create osd: " << osd_p << endl;
      mkfs_tmpfs(mkfs, osd_p, tmpfs_size_mb);
    }
    ctx2.add("mktmpfs", mkfs);

    Plustache::template_t t;
    string sh = t.render(tpl, ctx2);
    write_sh(sh);

  } // Mkcephconf_Tmpfs(Mkcephconf_Config& cfg)

  void mkfs_tmpfs(PlustacheTypes::CollectionType& mkfs, const bf::path& p,
		  int size) {
    using namespace std;
    PlustacheTypes::ObjectType mk;
    string s = p.native();
    sq(s);
    boost::format f("mkdir -p %1%");
    f % s;
    mk["mkdir"] = f.str();
    boost::format f1("mount -t tmpfs -o size=%1%m tmpfs %2%");
    f1 % size;
    f1 % s; // str()
    mk["mount"] = f1.str();
    // backing for lo device
    boost::format f2("dd if=/dev/zero of=%1% bs=%2% count=%3%");
    s += "/backing";
    cfg.osd_devs.push_back(s);
    f2 % s;
    f2 % int(1024*1024);
    f2 % size;
    mk["dd"] = f2.str();
    mk["nl"] = "\n";
    mkfs.push_back(mk);
  } // mkfs_tmpfs

  void umount_all(Plustache::Context& ctx2, const bf::path& ddir) {
    using namespace std;
    string cmd;
    boost::format f("mkdir -p %1%");
    f % ddir.native();
    ctx2.add("mkdatadir", f.str());

    PlustacheTypes::CollectionType ums;
    if (bf::is_directory(ddir)) {
      for (bf::directory_iterator d_iter(ddir);
	   d_iter != bf::directory_iterator(); ++ d_iter) {
	bf::path p(*d_iter);
	if (is_directory(p)) {
	  PlustacheTypes::ObjectType um;
	  boost::format f("umount -f %1% 2>/dev/null");
	  f % p.native();
	  cmd = f.str();
	  //cout << "cmd: " << cmd << endl;
	  um["umount_cmd"] = cmd;
	  ums.push_back(um);
	}
      }
    }
    ctx2.add("umount", ums);
  } // umount_all

  void write_sh(const std::string& sh) {
    using namespace std;
    string confdir = cfg.pt.get<string>("ceph_conf.ceph_confdir");
    sq(confdir);
    bf::path p(confdir);
    bf::create_directories(p);
    p /= "osds.sh";
    ofstream of(p.native(), ios::out|ios::trunc);
    of << sh;
    of.close();
  }

 public:
  int n_osds;
  int tmpfs_size_mb;
  std::string data_dir;
  Mkcephconf_Config& cfg;
};

#endif /* PREP_TMPFS_H */
