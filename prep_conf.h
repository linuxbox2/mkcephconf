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
#ifndef PREP_CONF_H
#define PREP_CONF_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <streambuf>
#include <boost/format.hpp>
#include "mkcephconf_config.h"
#include <plustache/plustache_types.hpp>
#include <plustache/template.hpp>

class Mkcephconf_PrepConf
{
 public:
 Mkcephconf_PrepConf(Mkcephconf_Config& _cfg) : cfg(_cfg) {
    using namespace std;
    n_osds = cfg.pt.get<int>("mkcephconf.n_osds");
    string tpl_dir = cfg.pt.get<string>("mkcephconf.ceph_conf_template");
    sq(tpl_dir);
    string tpl = read_from_file(tpl_dir);
    tpl = subst_file(tpl);
    write_conf(tpl);
  }

  void write_conf(const std::string& conf) {
    using namespace std;
    confdir = cfg.pt.get<string>("ceph_conf.ceph_confdir");
    sq(confdir);
    bf::path p(confdir);
    bf::create_directories(p);
    p /= "ceph.conf";
    ofstream of(p.native(), ios::out|ios::trunc);
    of << conf;
    of.close();
  }

  void subst_i(PlustacheTypes::ObjectType& ctx,
	       const std::string& ik, const std::string& ok) {
    using namespace std;
    int i = cfg.pt.get<int>(ik);
    ctx[ok] = to_string(i);
  }

  void subst_s(PlustacheTypes::ObjectType& ctx,
	       const std::string& ik, const std::string& ok) {
    using namespace std;
    string s = cfg.pt.get<string>(ik);
    sq(s);
    ctx[ok] = s;
  }

  std::string subst_file(std::string& tpl) {
    using namespace std;
    PlustacheTypes::ObjectType ctx;
    ctx["title"] = "Mkcephconf";
    subst_s(ctx, "ceph_conf.ceph_dir", "ceph-dir");
    subst_s(ctx, "ceph_conf.ceph_data", "ceph-data");
    subst_s(ctx, "ceph_conf.var_log_ceph", "var-log-ceph");
    subst_s(ctx, "ceph_conf.var_run_ceph", "var-run-ceph");
    subst_i(ctx, "ceph_conf.max_open_files", "max-open-files");
    subst_i(ctx, "ceph_conf.xio_queue_depth", "xio-queue-depth");
    subst_i(ctx, "ceph_conf.xio_port_shift", "xio-port-shift");
    subst_s(ctx, "ceph_conf.rdma_bind_addr", "rdma-bind-addr");
    subst_s(ctx, "mon0.host", "mon-0-host");
    subst_s(ctx, "mon0.addr", "mon-0-addr");
    subst_i(ctx, "mon0.port", "mon-0-port");
    subst_s(ctx, "mds0.host", "mds-0-host");
    subst_s(ctx, "mds0.addr", "mds-0-addr");
    subst_i(ctx, "mds0.port", "mds-0-port");
    subst_s(ctx, "osd.fstype", "osd-fstype");
    subst_i(ctx, "osd.journal_size", "osd-journal-size");
    subst_i(ctx, "osd.bind_port_min", "osd-bind-port-min");

    int port_min = cfg.pt.get<int>("osd.bind_port_min");
    int port_stride = cfg.pt.get<int>("osd.bind_port_stride");
    string host = cfg.pt.get<string>("osd.host0");
    sq(host);

    PlustacheTypes::CollectionType osds;
    for (int ix = 0; ix < n_osds; ++ix) {
      PlustacheTypes::ObjectType osd;
      osd["osd-ix"] = to_string(ix);
      osd["osd-host"] = host;
      osd["osd-bind-port-min"] = to_string(port_min);
      osd["osd-bind-port-max"] = to_string(port_min+port_stride-1);
      osd["osd-dev"] = cfg.osd_devs[ix];
      osd["nl"] = "\n";
      osds.push_back(osd);
      port_min += port_stride;
    }

    Plustache::Context ctx2;
    ctx2.add("base_section", ctx);
    ctx2.add("osds", osds);

    Plustache::template_t t;
    return t.render(tpl, ctx2);
  }

  std::string read_from_file(const std::string& fname) {
    using namespace std;
    ifstream f(fname, ios::in);
    if (! f.is_open()) {
      throw
	bf::filesystem_error(fname,
			     make_error_code(no_such_file_or_directory));
    }
    string s((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
    return s;
  }

public:
  int n_osds;
  Mkcephconf_Config& cfg;
  std::string confdir;

};

#endif /* PREP_CONF_H */
