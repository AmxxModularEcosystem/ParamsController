#include <amxmodx>
#include <json>
#include <ParamsController>

#include "ParamsController/DefaultObjects/ParamType/Boolean"
#include "ParamsController/DefaultObjects/ParamType/Integer"
#include "ParamsController/DefaultObjects/ParamType/Float"
#include "ParamsController/DefaultObjects/ParamType/String"
#include "ParamsController/DefaultObjects/ParamType/RGB"
#include "ParamsController/DefaultObjects/ParamType/Model"
#include "ParamsController/DefaultObjects/ParamType/Sound"
#include "ParamsController/DefaultObjects/ParamType/Resource"
#include "ParamsController/DefaultObjects/ParamType/File"
#include "ParamsController/DefaultObjects/ParamType/Dir"
#include "ParamsController/DefaultObjects/ParamType/ChatMessage"
#include "ParamsController/DefaultObjects/ParamType/Time"
#include "ParamsController/DefaultObjects/ParamType/TimeInterval"
#include "ParamsController/DefaultObjects/ParamType/WeekDay"

DefaultObjects_ParamType_Register() {
    DefaultObjects_ParamType_Boolean_Register();
    DefaultObjects_ParamType_Integer_Register();
    DefaultObjects_ParamType_Float_Register();
    DefaultObjects_ParamType_String_Register();

    DefaultObjects_ParamType_RGB_Register();
    DefaultObjects_ParamType_Model_Register();
    DefaultObjects_ParamType_Sound_Register();
    DefaultObjects_ParamType_Resource_Register();
    DefaultObjects_ParamType_File_Register();
    DefaultObjects_ParamType_Dir_Register();
    DefaultObjects_ParamType_ChatMessage_Register();

    DefaultObjects_ParamType_Time_Register();
    DefaultObjects_ParamType_TimeInterval_Register();
    DefaultObjects_ParamType_WeekDay_Register();
}
