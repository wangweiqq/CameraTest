// Copyright 2016-2018 SICK AG. All rights reserved.

#ifndef GENIRANGER_CONFIGREADER_H
#define GENIRANGER_CONFIGREADER_H

#include <istream>
#include <map>
#include <set>
#include <string>
#include <vector>

/** CSV reader that is closely connected to NodeExporter and its format. */
namespace GenIRanger
{
typedef std::map<std::string, std::string> NodeValueMap;

class ConfigReader
{
public:
  ConfigReader(std::istream& csv);
  ~ConfigReader();

  bool hasValue(std::string& key);
  std::string getValue(std::string& key);
  /** Returns all keys that exists in the configuration file. */
  std::set<std::string> getKeys();

private:
  // Node name (and selectors) is the key
  NodeValueMap mNodeValueMap;
};

}


#endif