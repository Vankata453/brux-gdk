/*===============*\
| FILE I/O HEADER |
\*===============*/

#ifndef _FILEIO_H_
#define _FILEIO_H_

#include <string>
#include <vector>

/* A class that takes the last PhysicsFS error and converts it to a readable message. */
class PhysFSError final : public std::exception
{
private:
  std::string m_message;

public:
  PhysFSError(const std::string& message, const std::string& action) throw();

  const char* what() const throw() { return m_message.c_str(); }
};

/** File system initialization/destruction. **/
void xyFSInit();
void xyFSDeinit();

/** General file system management functions. **/
void xyFSMount(const std::string& dir, bool prepend);

std::string xyGetDir();
std::string xyGetWriteDir();
std::string xyGetPrefDir(const std::string& org, const std::string& app);
void xySetWriteDir(const std::string& dir);

std::string xyFileRead(const std::string& file);
void xyFileWrite(const std::string& file, const std::string& data);
void xyFileAppend(const std::string& file, const std::string& data);
bool xyFileExists(const std::string& file);
bool xyLegacyFileExists(const std::string& file);

SQInteger sqLsDir(HSQUIRRELVM v);
SQInteger sqIsDir(HSQUIRRELVM v);

/** JSON encoding/decoding. **/
void sqDecodeJSONTable(HSQUIRRELVM v, cJSON *Item);
SQInteger sqDecodeJSON(HSQUIRRELVM v);

#endif
