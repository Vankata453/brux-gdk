/*===============*\
| FILE I/O SOURCE |
\*===============*/

#include <physfs.h>

#include "main.h"
#include "global.h"
#include "fileio.h"

/*  Initalize a PhysicsFS error. */
PhysFSError::PhysFSError(const std::string& message, const std::string& action) throw() :
  m_message()
{
  const PHYSFS_ErrorCode code = PHYSFS_getLastErrorCode();
  m_message = message + ": PHYSFS_" + action + " failed: " +
              PHYSFS_getErrorByCode(code) + " (" + std::to_string(code) + ")";
}

/** File system initialization/destruction. **/

void xyFSInit() {
  if (!PHYSFS_init(NULL))
    throw PhysFSError("Cannot initialize PhysicsFS", "init");
};

void xyFSDeinit() {
  if (!PHYSFS_deinit())
    throw PhysFSError("Cannot properly de-initialize PhysicsFS", "deinit");
};


/** General file system management functions. **/

void xyFSMount(const std::string& dir, bool prepend) {
  if (!PHYSFS_mount(dir.c_str(), NULL, !prepend))
    throw PhysFSError("Cannot mount '" + dir + "' directory", "mount");
};


std::string xyGetDir() {
  // Get the current working directory.
  return getcwd(0, 0);
}

std::string xyGetWriteDir() {
  return PHYSFS_getWriteDir();
};

std::string xyGetPrefDir(const std::string& org, const std::string& app) {
  const char* dir = PHYSFS_getPrefDir(org.c_str(), app.c_str());
  if (dir == NULL)
    throw PhysFSError("Error getting user-and-app specific directory", "getPrefDir");

  return dir;
};

void xySetWriteDir(const std::string& dir) {
  if (!PHYSFS_setWriteDir(dir.c_str()))
    throw PhysFSError("Error setting '" + dir + "' directory as write directory", "setWriteDir");

  try {
    // Mount the write directory, so it prepends to (overrides) files in the search path.
    xyFSMount(dir, true);
  }
  catch (const std::exception& err) {
    std::stringstream out;
    out << "Error mounting write directory: " << err.what();
    throw std::runtime_error(out.str());
  }
};


std::string xyFileRead(const std::string& file)
{
  // Check if the file exists.
  if (!PHYSFS_exists(file.c_str()))
    throw std::runtime_error("File '" + file + "' doesn't exist.");

  PHYSFS_file* handle = PHYSFS_openRead(file.c_str());
  const int length = PHYSFS_fileLength(handle);

  char* buffer = new char[length];
  if (PHYSFS_readBytes(handle, buffer, length) <= 0)
    throw PhysFSError("Cannot read any data from file '" + file + "'", "readBytes");

  const std::string str = buffer;
  PHYSFS_close(handle);
  return str;
};

void xyFileWrite(const std::string& file, const std::string& data)
{
  PHYSFS_file* handle = PHYSFS_openWrite(file.c_str());
  const int length = data.size();

  const char* buffer = data.c_str();
  if (PHYSFS_writeBytes(handle, buffer, length) < length)
    throw PhysFSError("Cannot write all data to file '" + file + "'", "writeBytes");

  PHYSFS_close(handle);
};

void xyFileAppend(const std::string& file, const std::string& data)
{
  // If the file currently exists, read its data.
  std::string file_data;
  if (PHYSFS_exists(file.c_str())) {
    file_data = xyFileRead(file);
  }

  // Write old and new data.
  xyFileWrite(file, file_data + data);
};

bool xyFileExists(const std::string& file) {
	return PHYSFS_exists(file.c_str());
};

bool xyLegacyFileExists(const std::string& file) {
  // This function should not be exposed, because it searches beyond PhysicsFS's search path.
  // Only used for checking if the initial Squirrel file exists.

  struct stat buff;
  return stat(file.c_str(), &buff) != -1;
}


SQInteger sqLsDir(HSQUIRRELVM v) {
	const char* dir;

	sq_getstring(v, 2, &dir);

  // Create array for results.
  sq_newarray(v, 0);

  // Read files and append to array.
	char **rc = PHYSFS_enumerateFiles(dir);
  if (rc == NULL)
  {
    std::stringstream err;
    err << "Error enumerating files in directory '" << dir << "'";
    throw PhysFSError(err.str(), "enumerateFiles");
  }
  char **i;

  for (i = rc; *i != NULL; i++)
  {
    sq_pushstring(v, *i, strlen(*i));
    sq_arrayappend(v, -2);
  }

  PHYSFS_freeList(rc);
	return 1;
};

SQInteger sqIsDir(HSQUIRRELVM v) {
	const char* dir;

	sq_getstring(v, 2, &dir);

  // Get file/directory stats.
  PHYSFS_Stat stat;
  PHYSFS_stat(dir, &stat);

	sq_pushbool(v, stat.filetype == PHYSFS_FILETYPE_DIRECTORY);

  return 1;
};


/** JSON encoding/decoding. **/

// Credit to Nova Storm for the JSON encoding and decoding functions

void sqDecodeJSONTable(HSQUIRRELVM v, cJSON *Item) {
	if(!Item)
		return;
	while(Item) {
		if(Item->str)
			sq_pushstring(v, Item->str, -1);
		switch(Item->type) {
			case cJSON_False:
				sq_pushbool(v, SQFalse);
				break;
			case cJSON_True:
				sq_pushbool(v, SQTrue);
				break;
			case cJSON_NULL:
				sq_pushnull(v);
				break;
			case cJSON_Number:
				if(Item->valueint == Item->valuedouble)
					sq_pushinteger(v, Item->valueint);
				else
					sq_pushfloat(v, Item->valuedouble);
				break;
			case cJSON_String:
				sq_pushstring(v, Item->valuestring, -1);
				break;
			case cJSON_Array:
				sq_newarray(v, 0);
				sqDecodeJSONTable(v, Item->child);
				break;
			case cJSON_Object:
				sq_newtable(v);
				sqDecodeJSONTable(v, Item->child);
				break;
		}
		if(Item->str)
			sq_newslot(v,-3,SQFalse);
		else
			sq_arrayappend(v, -2);
		Item = Item->next;
	}
};

SQInteger sqDecodeJSON(HSQUIRRELVM v) {
	const SQChar *Str;
	sq_getstring(v, 2, &Str);
	if(Str[0]!='{' && Str[0]!='[') {
		if(!strcmp(Str, "true"))
			sq_pushbool(v, SQTrue);
		else if(!strcmp(Str, "false"))
			sq_pushbool(v, SQFalse);
		else if(isdigit(Str[0]) || (Str[0]=='-' && isdigit(Str[1])))
			sq_pushinteger(v, strtol(Str, NULL, 0));
		else
			sq_pushstring(v, Str, -1);
		return 1;
	}

	cJSON *Root = cJSON_Parse(Str);
	if(!Root || !Root->child) {
		sq_pushnull(v);
		return 1;
	}
	sq_newtable(v);
	sqDecodeJSONTable(v, Root->child);
	cJSON_Delete(Root);
	return 1;
}
