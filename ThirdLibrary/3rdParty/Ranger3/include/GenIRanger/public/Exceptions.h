// Copyright 2016-2018 SICK AG. All rights reserved.

#ifndef GENIRANGER_EXCEPTIONS_H
#define GENIRANGER_EXCEPTIONS_H

#include <exception>
#include <string>

namespace GenIRanger
{
/** Generic GenIRanger Exception class. Thrown whenever an error occurs either
    in GenIRanger or GenICam. The messages from GenICam exceptions are wrapped
    into GenIRanger Exceptions.
*/
class GenIRangerException : public std::exception
{
public:
  GenIRangerException(const std::string& message);
};

/** Indicates that something went wrong during import configuration to device. */
class ImportException : public GenIRangerException
{
public:
  ImportException(const std::string& message);
};

/** Indicates that something went wrong during export configuration from device. */
class ExportException : public GenIRangerException
{
public:
  ExportException(const std::string& message);
};

/** Exception that indicates something wrong with the configuration file */
class ConfigurationFileException : public ImportException
{
public:
  ConfigurationFileException(const std::string& message);
};

/** Indicates that something went wrong while writing device log to disk. */
class DeviceLogException : public GenIRangerException
{
public:
  DeviceLogException(const std::string& message);
};

/** Indicates that there is some problem when saving data to disk. */
class SaveException : public GenIRangerException
{
public:
  SaveException(const std::string& message);
};

}
#endif