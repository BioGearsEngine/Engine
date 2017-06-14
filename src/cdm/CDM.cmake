# Files in the project (Relative to this CMAKE file)
FILE(GLOB CDM_FILES "cpp/*.h" "cpp/*.cpp")
# Does not belong in this project
LIST(REMOVE_ITEM CDM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/CommonDataModelJNI.cpp)
LIST(REMOVE_ITEM CDM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/stdafx.cpp)#PreCompiled Header Generator, will add later
SOURCE_GROUP("" FILES ${CDM_FILES})
SET(SOURCE ${CDM_FILES})
# Circuit
FILE(GLOB CIRCUIT_FILES "cpp/circuit/*.h" "cpp/circuit/*.cpp" "cpp/circuit/*.inl")
SOURCE_GROUP("Circuit" FILES ${CIRCUIT_FILES})
LIST(APPEND SOURCE ${CIRCUIT_FILES})  
# Circuit/Electrical
FILE(GLOB CIRCUIT_ELECTRICAL_FILES "cpp/circuit/electrical/*.h" "cpp/circuit/electrical/*.cpp" "cpp/circuit/electrical/*.inl")
SOURCE_GROUP("Circuit\\Electrical" FILES ${CIRCUIT_ELECTRICAL_FILES})
LIST(APPEND SOURCE ${CIRCUIT_ELECTRICAL_FILES}) 
# Circuit/Fluid
FILE(GLOB CIRCUIT_FLUID_FILES "cpp/circuit/fluid/*.h" "cpp/circuit/fluid/*.cpp" "cpp/circuit/fluid/*.inl")
SOURCE_GROUP("Circuit\\Fluid" FILES ${CIRCUIT_FLUID_FILES})
LIST(APPEND SOURCE ${CIRCUIT_FLUID_FILES}) 
# Circuit/Thermal
FILE(GLOB CIRCUIT_THERMAL_FILES "cpp/circuit/thermal/*.h" "cpp/circuit/thermal/*.cpp" "cpp/circuit/thermal/*.inl")
SOURCE_GROUP("Circuit\\Thermal" FILES ${CIRCUIT_THERMAL_FILES})
LIST(APPEND SOURCE ${CIRCUIT_THERMAL_FILES}) 
# Compartment
FILE(GLOB COMPARTMENT_FILES "cpp/compartment/*.h" "cpp/compartment/*.cpp" "cpp/compartment/*.inl")
SOURCE_GROUP("Compartment" FILES ${COMPARTMENT_FILES})
LIST(APPEND SOURCE ${COMPARTMENT_FILES}) 
# Compartment/Fluid
FILE(GLOB COMPARTMENT_FLUID_FILES "cpp/compartment/fluid/*.h" "cpp/compartment/fluid/*.cpp" "cpp/compartment/fluid/*.inl")
SOURCE_GROUP("Compartment\\Fluid" FILES ${COMPARTMENT_FLUID_FILES})
LIST(APPEND SOURCE ${COMPARTMENT_FLUID_FILES}) 
# Compartment/Thermal
FILE(GLOB COMPARTMENT_THERMAL_FILES "cpp/compartment/thermal/*.h" "cpp/compartment/thermal/*.cpp" "cpp/compartment/thermal/*.inl")
SOURCE_GROUP("Compartment\\Thermal" FILES ${COMPARTMENT_THERMAL_FILES})
LIST(APPEND SOURCE ${COMPARTMENT_THERMAL_FILES}) 
# Compartment/Tissue
FILE(GLOB COMPARTMENT_TISSUE_FILES "cpp/compartment/tissue/*.h" "cpp/compartment/tissue/*.cpp" "cpp/compartment/tissue/*.inl")
SOURCE_GROUP("Compartment\\Tissue" FILES ${COMPARTMENT_TISSUE_FILES})
LIST(APPEND SOURCE ${COMPARTMENT_TISSUE_FILES}) 
# Compartment/Managers
FILE(GLOB COMPARTMENT_MANAGERS_FILES "cpp/compartment/managers/*.h" "cpp/compartment/managers/*.cpp")
SOURCE_GROUP("Compartment\\Managers" FILES ${COMPARTMENT_MANAGERS_FILES})
LIST(APPEND SOURCE ${COMPARTMENT_MANAGERS_FILES}) 
# Substance Quantity
FILE(GLOB SUBSTANCE_QUANTITY_FILES "cpp/compartment/substances/*.h" "cpp/compartment/substances/*.cpp" "cpp/compartment/substances/*.inl")
SOURCE_GROUP("Compartment\\Substances" FILES ${SUBSTANCE_QUANTITY_FILES})
LIST(APPEND SOURCE ${SUBSTANCE_QUANTITY_FILES}) 
# Patient
FILE(GLOB PATIENT_FILES "cpp/patient/*.h" "cpp/patient/*.cpp")
SOURCE_GROUP("Patient" FILES ${PATIENT_FILES})
FILE(GLOB PATIENT_ACTION_FILES "cpp/patient/actions/*.h" "cpp/patient/actions/*.cpp")
SOURCE_GROUP("Patient\\Actions" FILES ${PATIENT_ACTION_FILES})
FILE(GLOB PATIENT_ASSESSMENT_FILES "cpp/patient/assessments/*.h" "cpp/patient/assessments/*.cpp")
SOURCE_GROUP("Patient\\Assessments" FILES ${PATIENT_ASSESSMENT_FILES})
FILE(GLOB PATIENT_CONDITIONS_FILES "cpp/patient/conditions/*.h" "cpp/patient/conditions/*.cpp")
SOURCE_GROUP("Patient\\Conditions" FILES ${PATIENT_CONDITIONS_FILES})
LIST(APPEND SOURCE ${PATIENT_FILES}) 
LIST(APPEND SOURCE ${PATIENT_ACTION_FILES}) 
LIST(APPEND SOURCE ${PATIENT_ASSESSMENT_FILES}) 
LIST(APPEND SOURCE ${PATIENT_CONDITIONS_FILES}) 
# Properties
FILE(GLOB PROPERTY_FILES "cpp/properties/*.h" "cpp/properties/*.cpp" "cpp/properties/*.inl")
# Remove SEArray* files as arrays are not supported yet
FOREACH(f ${PROPERTY_FILES})	
	STRING(FIND ${f} "SEArray" idx)
#	MESSAGE(STATUS ${f} ${idx}) 
	IF(${idx} GREATER -1)
#		MESSAGE(STATUS Removing ${f}) 
		LIST(REMOVE_ITEM PROPERTY_FILES ${f})
	ENDIF()
ENDFOREACH()
SOURCE_GROUP("Properties" FILES ${PROPERTY_FILES})
LIST(APPEND SOURCE ${PROPERTY_FILES}) 
# Scenario
FILE(GLOB SCENARIO_FILES "cpp/scenario/*.h" "cpp/scenario/*.cpp")
SOURCE_GROUP("Scenario" FILES ${SCENARIO_FILES})
LIST(APPEND SOURCE ${SCENARIO_FILES}) 
FILE(GLOB SCENARIO_DATA_REQUEST_FILES "cpp/scenario/requests/*.h" "cpp/scenario/requests/*.cpp")
SOURCE_GROUP("Scenario\\Data Requests" FILES ${SCENARIO_DATA_REQUEST_FILES})
LIST(APPEND SOURCE ${SCENARIO_DATA_REQUEST_FILES}) 
# Engine
FILE(GLOB ENGINE_FILES "cpp/engine/*.h" "cpp/engine/*.cpp")
SOURCE_GROUP("Engine" FILES ${ENGINE_FILES})
LIST(APPEND SOURCE ${ENGINE_FILES})    
# Substance
FILE(GLOB SUBSTANCE_FILES "cpp/substance/*.h" "cpp/substance/*.cpp" "cpp/substance/*.inl")
SOURCE_GROUP("Substance" FILES ${SUBSTANCE_FILES})
LIST(APPEND SOURCE ${SUBSTANCE_FILES})
# System
FILE(GLOB SYSTEM_FILES "cpp/system/*.h" "cpp/system/*.cpp")
SOURCE_GROUP("System" FILES ${SYSTEM_FILES})
LIST(APPEND SOURCE ${SYSTEM_FILES}) 
# System\Environment
FILE(GLOB ENVIRONMENT_FILES "cpp/system/environment/*.h" "cpp/system/environment/*.cpp")
SOURCE_GROUP("System\\Environment" FILES ${ENVIRONMENT_FILES})
LIST(APPEND SOURCE ${ENVIRONMENT_FILES}) 
FILE(GLOB ENVIRONMENT_ACTION_FILES "cpp/system/environment/actions/*.h" "cpp/system/environment/actions/*.cpp")
SOURCE_GROUP("System\\Environment\\Actions" FILES ${ENVIRONMENT_ACTION_FILES})
LIST(APPEND SOURCE ${ENVIRONMENT_ACTION_FILES}) 
FILE(GLOB ENVIRONMENT_CONDITION_FILES "cpp/system/environment/conditions/*.h" "cpp/system/environment/conditions/*.cpp")
SOURCE_GROUP("System\\Environment\\Conditions" FILES ${ENVIRONMENT_CONDITION_FILES})
LIST(APPEND SOURCE ${ENVIRONMENT_CONDITION_FILES}) 
# System\Equipment
FILE(GLOB ANESTHESIA_FILES "cpp/system/equipment/Anesthesia/*.h" "cpp/system/equipment/Anesthesia/*.cpp")
SOURCE_GROUP("System\\Equipment\\Anesthesia" FILES ${ANESTHESIA_FILES})
FILE(GLOB ANESTHESIA_ACTION_FILES "cpp/system/equipment/Anesthesia/actions/*.h" "cpp/system/equipment/Anesthesia/actions/*.cpp")
SOURCE_GROUP("System\\Equipment\\Anesthesia\\Actions" FILES ${ANESTHESIA_ACTION_FILES})
FILE(GLOB ECG_FILES "cpp/system/equipment/ElectroCardioGram/*.h" "cpp/system/equipment/ElectroCardioGram/*.cpp")
SOURCE_GROUP("System\\Equipment\\ECG" FILES ${ECG_FILES})
FILE(GLOB INHALER_FILES "cpp/system/equipment/Inhaler/*.h" "cpp/system/equipment/Inhaler/*.cpp")
SOURCE_GROUP("System\\Equipment\\Inhaler" FILES ${INHALER_FILES})
FILE(GLOB INHALER_ACTION_FILES "cpp/system/equipment/Inhaler/actions/*.h" "cpp/system/equipment/Inhaler/actions/*.cpp")
SOURCE_GROUP("System\\Equipment\\Inhaler\\Actions" FILES ${INHALER_ACTION_FILES})

LIST(APPEND SOURCE ${ANESTHESIA_FILES}) 
LIST(APPEND SOURCE ${ANESTHESIA_ACTION_FILES}) 
LIST(APPEND SOURCE ${ECG_FILES}) 
LIST(APPEND SOURCE ${INHALER_FILES}) 
LIST(APPEND SOURCE ${INHALER_ACTION_FILES}) 
LIST(APPEND SOURCE ${MECHANICAL_VENTILATOR_FILES}) 
LIST(APPEND SOURCE ${MECHANICAL_VENTILATOR_ACTION_FILES}) 
# System\Physiology
FILE(GLOB PHYSIOLOGY_FILES "cpp/system/physiology/*.h" "cpp/system/physiology/*.cpp")
SOURCE_GROUP("System\\Physiology" FILES ${PHYSIOLOGY_FILES})
LIST(APPEND SOURCE ${PHYSIOLOGY_FILES}) 
# Utils
FILE(GLOB UTILS_FILES "cpp/utils/*.h" "cpp/utils/*.cpp")
SOURCE_GROUP("Utils" FILES ${UTILS_FILES})
FILE(GLOB UTILS_TESTING_FILES "cpp/utils/testing/*.h" "cpp/utils/testing/*.cpp")
SOURCE_GROUP("Utils\\Testing" FILES ${UTILS_TESTING_FILES})
# Utils\UnitConversion
FILE(GLOB UTILS_UCE_FILES "cpp/utils/unitconversion/*.h" "cpp/utils/unitconversion/*.cpp")
LIST(REMOVE_ITEM UTILS_UCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/cpp/utils/unitconversion/UnitConversionDriver.cpp)
SOURCE_GROUP("Utils\\UnitConversion" FILES ${UTILS_UCE_FILES})
# Utils\TaskRunner
FILE(GLOB UTILS_TASKRUNNER_FILES "cpp/utils/TaskRunner/*.h" "cpp/utils/TaskRunner/*.cpp")
SOURCE_GROUP("Utils\\TaskRunner" FILES ${UTILS_TASKRUNNER_FILES})
LIST(APPEND SOURCE ${UTILS_TASKRUNNER_FILES}) 
#FILE(GLOB UTILS_XPSTL_FILES "cpp/utils/xpstl/*.h" "cpp/utils/xpstl/*.cpp")
#SOURCE_GROUP("Utils\\XPSTL" FILES ${UTILS_XPSTL_FILES})
LIST(APPEND SOURCE ${UTILS_FILES}) 
LIST(APPEND SOURCE ${UTILS_TESTING_FILES}) 
LIST(APPEND SOURCE ${UTILS_UCE_FILES}) 
#LIST(APPEND SOURCE ${UTILS_XPSTL_FILES}) 

# Do this before ADD_LIBRARY as it may or may not add files to the SOURCE
SET_PRECOMPILED_HEADER(stdafx.h ${CMAKE_CURRENT_SOURCE_DIR}/cpp/stdafx.cpp SOURCE)
#IF(MINGW)# Generate precompiled header for mingw			   
# Generate gph
#ADD_CUSTOM_COMMAND(TARGET CommonDataModel PRE_BUILD
#				   COMMAND call ant -f \"${CMAKE_CURRENT_SOURCE_DIR}/buildXSD.xml\" compileCDMGCH)					   
#ENDIF(MINGW)
# The DLL we are building
ADD_LIBRARY(CommonDataModel SHARED ${SOURCE})
# Preprocessor Definitions and Include Paths
# Common Compile Flags
SET(CDM_FLAGS)
SET(CDM_FLAGS "${CDM_FLAGS} -D EIGEN_MPL2_ONLY")
SET(CDM_FLAGS "${CDM_FLAGS} -D COMMONDATAMODEL_EXPORTS")
SET(CDM_FLAGS "${CDM_FLAGS} -D UNICODE")
SET(CDM_FLAGS "${CDM_FLAGS} -D _UNICODE")
if(MSVC)	
	SET(CDM_FLAGS "${CDM_FLAGS} -Zm215")
	TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/dirent)#Platform generic file system utility
ENDIF(MSVC)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../schema/cpp)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Eigen-3.3.1)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/log4cpp/include)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XERCES_VER}/src)
TARGET_INCLUDE_DIRECTORIES(CommonDataModel PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${XSD_VER}/libxsd)
	
#MESSAGE(${CDM_FLAGS})			   
IF((env STREQUAL "msvc") AND (EX_PLATFORM EQUAL 64))
#Make sure to prepend a space
	SET(CDM_FLAGS  "${CDM_FLAGS} /bigobj")
ENDIF()	
#MESSAGE(${CDM_FLAGS})	
SET_TARGET_PROPERTIES(CommonDataModel PROPERTIES COMPILE_FLAGS "${CDM_FLAGS}" PREFIX "")												
IF(APPLE)
    SET_TARGET_PROPERTIES(CommonDataModel PROPERTIES MACOSX_RPATH ON)
ENDIF()

# Dependent Libraries
TARGET_LINK_LIBRARIES(CommonDataModel log4cpp)
TARGET_LINK_LIBRARIES(CommonDataModel DataModelBindings)# will include DataModelBindings dependent libs too!

IF (APPLE)
    ADD_CUSTOM_COMMAND(TARGET CommonDataModel
        POST_BUILD COMMAND
        ${CMAKE_INSTALL_NAME_TOOL} -delete_rpath "@executable_path/../Java/release64"
        $<TARGET_FILE:CommonDataModel>
        || true)

    ADD_CUSTOM_COMMAND(TARGET CommonDataModel
        POST_BUILD COMMAND
        ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "@executable_path/../Java/release64"
        $<TARGET_FILE:CommonDataModel>
        || true)
ENDIF()

# Copy to the bin
SET(CONFIG_STRING)
IF(WIN32)
  SET(CONFIG_STRING ${CMAKE_CFG_INTDIR})
ELSE()
  STRING(TOLOWER ${CMAKE_BUILD_TYPE} CONFIG_STRING)
ENDIF()

ADD_CUSTOM_COMMAND(TARGET CommonDataModel POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CommonDataModel> ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CONFIG_STRING}${EX_CONFIG}
)
