#include <el/ext.h>

#include "nt.h"
using namespace NT;

#pragma comment(lib, "ntdll")

namespace Ext {


SYSTEM_POWER_INFORMATION AFXAPI NtSystem::GetSystemPowerInformation() {
	SYSTEM_POWER_INFORMATION spi;
	NtCheck(NT::NtPowerInformation(SystemPowerInformation, 0, 0, &spi, sizeof spi));
	return spi;
}



} // Ext::
