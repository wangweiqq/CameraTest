// Copyright 2016-2018 SICK AG. All rights reserved.

#ifndef GENIRANGER_NODEUTIL_H
#define GENIRANGER_NODEUTIL_H

#include "GenICam.h"
#include <string>

namespace GenIRanger
{

namespace NodeUtil
{
  bool isInteger(const GenApi::CNodePtr& node);

  bool isEnumeration(const GenApi::CNodePtr& node);

  bool isConfigNode(const GenApi::CNodePtr& node);

  std::string getValueAsString(const GenApi::CNodePtr& node);
}
}
#endif
