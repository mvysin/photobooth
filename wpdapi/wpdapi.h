// wpdapi.h

#pragma once

#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <vcclr.h>

#include <portabledeviceapi.h>
#include <portabledevice.h>

#pragma comment(linker, "/defaultlib:ole32.lib")
#pragma comment(linker, "/defaultlib:portabledeviceguids.lib")


#include "Util.h"
#include "Enum.h"
#include "Events.h"
#include "Device.h"
#include "Objects.h"
#include "Manager.h"