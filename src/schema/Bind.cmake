# Files in the project (Relative to this CMAKE file)

# There are two ways I generate the bind code:
# - Per XSD type : These are in the cpp/ folder
# - Per XSD file : These are in the cpp/min folder
# There are fewer files in the min folder but they are much larger
# And in working witn MinGW, they generate obj files with redundent types in them
# When the linker tries to process them all, it runs out of memory
# MSVC does not have any issue with these large files, and it is much fast to build the min folder
# So for MSVC I am going to build the bind DLL with the min generated source to speed up build times
# Maybe one day I will figure out how to properly get a gcc compiler to digest the min folder as well
# I tried precompiled headers in MinGW but that did not really work as I expected..
# Shoot me an email if you have any ideas (abray@ara.com)

FILE(GLOB BIND_FILES
    "cpp/bind/*.hxx"
	"cpp/bind/*.cxx"
)
IF(MSVC)
	FILE(GLOB_RECURSE MIN_BIND_FILES
		"cpp/bind/min/*.hxx"
		"cpp/bind/min/*.cxx"
	)
	SOURCE_GROUP("" FILES ${MIN_BIND_FILES})
	SET(SOURCE ${MIN_BIND_FILES})
ELSE(MSVC)
	SOURCE_GROUP("" FILES ${BIND_FILES})
	SET(SOURCE ${BIND_FILES})
ENDIF(MSVC)


# I use precompiled Headers in windows if the project contains the Per XSD type cxx files, this also speeds the build
#SET_PRECOMPILED_HEADER(biogears-cdm.hxx ${CMAKE_CURRENT_SOURCE_DIR}/cpp/bind/biogears-cdm.cxx SOURCE)
#IF(MINGW)# Generate precompiled header for mingw			   
# Generate gch
#ADD_CUSTOM_COMMAND(TARGET DataModelBindings PRE_BUILD
#				   COMMAND call ant -f \"${CMAKE_CURRENT_SOURCE_DIR}/buildXSD.xml\" compileBindGCH)					   
#ENDIF(MINGW)

# The DLL we are building
ADD_LIBRARY(DataModelBindings SHARED ${SOURCE})
# Preprocessor Definitions and Include Paths
TARGET_INCLUDE_DIRECTORIES(DataModelBindings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp/bind)	
TARGET_INCLUDE_DIRECTORIES(DataModelBindings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src)
TARGET_INCLUDE_DIRECTORIES(DataModelBindings PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XSD_VER}/libxsd)
SET(FLAGS)
LIST(APPEND FLAGS " -DXML_LIBRARY")
SET_TARGET_PROPERTIES(DataModelBindings PROPERTIES COMPILE_FLAGS "${FLAGS}" PREFIX "")

IF(APPLE)
    SET_TARGET_PROPERTIES(DataModelBindings PROPERTIES MACOSX_RPATH ON)
ENDIF()

IF(env STREQUAL "msvc")
	# Grab the code synthesis libs
	SET(SEARCH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src/lib${EX_CONFIG}/vc-12.0)
	FIND_LIBRARY(XSD_PATH NAMES xerces-c_3 PATHS ${SEARCH})		 																				
	FIND_LIBRARY(XSD_PATH_D NAMES xerces-c_3D PATHS ${SEARCH})											
	# Dependent Libraries
	TARGET_LINK_LIBRARIES(DataModelBindings debug ${XSD_PATH})#Only use Debug lib if we have an issue with code synthesis
	TARGET_LINK_LIBRARIES(DataModelBindings optimized ${XSD_PATH})
ELSE()

	IF(env STREQUAL "xcode")
		SET(XERCES_NAME "xerces-c")
    ELSEIF(env STREQUAL "unixMake" OR env STREQUAL "raspberryPi")
        SET(XERCES_NAME "xerces-c-3.1")
    ELSE()
		SET(XERCES_NAME "libxerces-c")
	ENDIF()
    
    IF(env STREQUAL "mingw")
        SET(SEARCH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src/.libs/mingw)
    ELSE()
        SET(SEARCH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src/.libs)
    ENDIF()

    FIND_LIBRARY(XERCES_PATH NAMES ${XERCES_NAME} PATHS ${SEARCH})	

	# Dependent Libraries
	TARGET_LINK_LIBRARIES(DataModelBindings ${XERCES_PATH})
ENDIF()

IF(WIN32)
	SET(EXECUTABLE_CALL "call")
ENDIF()

IF(APPLE)
	SET(ANT_PREFIX "/usr/local/bin/")
ENDIF()

# Generate cxx/hxx files from xsd
ADD_CUSTOM_COMMAND(TARGET DataModelBindings PRE_BUILD
                   COMMAND ${EXECUTABLE_CALL} ${ANT_PREFIX}ant -Denv=${env} -f \"${CMAKE_CURRENT_SOURCE_DIR}/buildXSD.xml\" compileCPP)					   

# Prep bin
SET(CONFIG_STRING)
IF(WIN32)
  SET(CONFIG_STRING ${CMAKE_CFG_INTDIR})
ELSE()
  STRING(TOLOWER ${CMAKE_BUILD_TYPE} CONFIG_STRING)
ENDIF()

ADD_CUSTOM_COMMAND(TARGET DataModelBindings POST_BUILD 
                   COMMAND ${EXECUTABLE_CALL} ${ANT_PREFIX}ant -Denv=${env} -f \"${CMAKE_CURRENT_SOURCE_DIR}/build.xml\" refreshBin
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:DataModelBindings> ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CONFIG_STRING}${EX_CONFIG})
