CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(BioGears)
CMAKE_POLICY(SET CMP0015 NEW)

SET(env "" CACHE STRING "")

IF(env STREQUAL "")
SET(env "msvc" CACHE STRING "")
ENDIF()

IF(env STREQUAL "msvc")
    SET(XSD_VER "xsd-4.0.0-i686-windows" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.1" CACHE TYPE INTERNAL FORCE)
ELSEIF(env STREQUAL "mingw")
    SET(XSD_VER "xsd-4.0.0-i686-windows" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.2" CACHE TYPE INTERNAL FORCE)
ELSEIF(env STREQUAL "eclipsemingw")
    SET(XSD_VER "xsd-4.0.0-i686-windows" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.2" CACHE TYPE INTERNAL FORCE)
ELSEIF(env STREQUAL "xcode")
    SET(XSD_VER "xsd-4.0.0-i686-macosx" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.2" CACHE TYPE INTERNAL FORCE)
ELSEIF(env STREQUAL "unixMake")
    SET(XSD_VER "xsd-4.0.0-x86_64-linux-gnu" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.2" CACHE TYPE INTERNAL FORCE)
ELSEIF(env STREQUAL "raspberryPi")
    SET(XSD_VER "xsd-4.0.0-x86_64-linux-gnu" CACHE TYPE INTERNAL FORCE)
    SET(XERCES_VER "xerces-c-3.1.2" CACHE TYPE INTERNAL FORCE)
    SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
ELSE()
    MESSAGE(FATAL_ERROR "env variable has not been specified.  Make sure when calling cmake a valid environment is specified via -Denv=")
ENDIF()

SET(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo CACHE TYPE INTERNAL FORCE )

IF(MSVC)
	SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob2 /Oi /Od /RTC1" CACHE TYPE INTERNAL FORCE)
ENDIF()

IF(MINGW)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--kill-at -std=gnu++0x")#turn on C++11
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--kill-at")
ENDIF()

IF(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
ENDIF()

IF(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF()

SET(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE TYPE INTERNAL FORCE)
SET(CMAKE_C_STANDARD_LIBRARIES "" CACHE TYPE INTERNAL FORCE)

IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    MESSAGE( "64 bits compiler detected" )
    SET( EX_PLATFORM 64 )
    SET( EX_CONFIG "64" )
	SET(FIND_LIBRARY_USE_LIB64_PATHS ON)
ELSE( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
    MESSAGE( "32 bits compiler detected" )
    SET( EX_PLATFORM 32 )
    SET( EX_CONFIG "" )
	SET(FIND_LIBRARY_USE_LIB64_PATHS OFF)
ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

ADD_DEFINITIONS(-DXSD_CXX11)

MACRO(SET_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
	IF(MSVC)
		# Turn on Precompiled headers on the project
		# SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "/Yu")
		# MSVC project precompiled property really just switches the same switch on all the source files
		# It's a gui feature that does not map to anything going into the compiler
		# the compiler uses the precompiled switches on the file props not the project
		# It's just a gui shortcut/feature to apply settings on the source files faster
		
		#GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
		#SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
		SET(Sources ${${SourcesVar}})

		SET_SOURCE_FILES_PROPERTIES(${Sources}
									PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\"")  
		#/Fp\"${PrecompiledBinary}\"" <- Use this if you want to change the name&|location of the pch
		# I don't want to change it, the default naming convention is good
		SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
									PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\"")
		MESSAGE(Making ${PrecompiledSource} the precompiled generator for ${target})									
		# Add precompiled header to SourcesVar
		LIST(APPEND ${SourcesVar} ${PrecompiledSource})
		SOURCE_GROUP("" FILES ${PrecompiledSource})
	ENDIF(MSVC)
ENDMACRO(SET_PRECOMPILED_HEADER)

IF(UNIX)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\$ORIGIN")
ENDIF()

ADD_SUBDIRECTORY(schema)
ADD_SUBDIRECTORY(cdm)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(test)
