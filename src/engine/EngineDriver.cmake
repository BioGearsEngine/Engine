# Files in the project (Relative to this CMAKE file)
SOURCE_GROUP("" FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/BioGearsScenarioDriver.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/BioGearsScenarioDriver.h
                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Verification.cpp
                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Verification.h)
# The DLL we are building
ADD_EXECUTABLE(BioGearsScenarioDriver ${CMAKE_CURRENT_SOURCE_DIR}/cpp/BioGearsScenarioDriver.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/BioGearsScenarioDriver.h
                                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Verification.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/cpp/Verification.h)

SET_TARGET_PROPERTIES(BioGearsScenarioDriver PROPERTIES COMPILE_FLAGS -pthread LINK_FLAGS -pthread)

# Preprocessor Definitions and Include Paths
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cdm/cpp)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../schema/cpp)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cpp/include)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Eigen-3.3.1)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src)
TARGET_INCLUDE_DIRECTORIES(BioGearsScenarioDriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XSD_VER}/libxsd)

# Dependent Libraries
TARGET_LINK_LIBRARIES(BioGearsScenarioDriver CommonDataModel)
TARGET_LINK_LIBRARIES(BioGearsScenarioDriver BioGearsEngine)
# Generate cxx/hxx files from xsd
# Copy to the bin

IF(UNIX)
    SET(CMAKE_INSTLL_RPATH "${CMAKE_INSTALL_RPATH}:\$ORIGIN")
ENDIF()

SET(CONFIG_STRING)
IF(WIN32)
  SET(CONFIG_STRING ${CMAKE_CFG_INTDIR})
ELSE()
  STRING(TOLOWER ${CMAKE_BUILD_TYPE} CONFIG_STRING)
ENDIF()

ADD_CUSTOM_COMMAND(TARGET BioGearsScenarioDriver POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BioGearsScenarioDriver> ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CONFIG_STRING}${EX_CONFIG}
)

