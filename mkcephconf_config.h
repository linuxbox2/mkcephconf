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
#ifndef MKCEPHCONF_CONFIG_H
#define MKCEPHCONF_CONFIG_H

#include <string>
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

class Mkcephconf_Config
{
 public:
  Mkcephconf_Config(const std::string& param_file) {
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

#endif /* MKCEPHCONF_CONFIG_H */
