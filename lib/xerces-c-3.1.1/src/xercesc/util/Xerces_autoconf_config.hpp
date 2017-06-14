#pragma once 

#if defined(_MSC_VER)
	#include "Xerces_autoconf_config.msvc.hpp"
#else//if (__GCC__) || (__GNUC__)
	#include "Xerces_autoconf_config.mingw.hpp"
#endif