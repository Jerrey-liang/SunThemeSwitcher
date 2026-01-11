#include "Config.h"
#include "AutoRun.h"

#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <vector>

void SelfUninstall();
std::string GetConfigDir();
bool DeleteConfigDirectory();
