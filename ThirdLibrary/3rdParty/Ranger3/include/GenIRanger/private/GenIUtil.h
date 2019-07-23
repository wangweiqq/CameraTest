// Copyright 2016-2018 SICK AG. All rights reserved.

#ifndef GENIRANGER_GENIUTIL_H
#define GENIRANGER_GENIUTIL_H

#include <string>

namespace GenIRanger
{

namespace GenIUtil
{

/** Writes the message to std::out if LOG_ENABLED is defined. Does not flush. */
void log(std::string message);

/** Throws a GenIRangerException is the provided message and writes it to
    std::out if LOG_ENABLED is defined. Appends endl and flushes.
*/
void throwAndLog(std::string message);

}
}
#endif
