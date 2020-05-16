/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef IMU_MESSAGES_H_INCLUDE_GUARD
#define IMU_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "6d14d7f2b815e817176e591c206d6e52"

#ifdef MK_TOOLS_BUILD
    extern const char** imu_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*imu_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "imu"
#endif


#define _MAX_MSG_SIZE 36

// Define the message type for communicating between client and server
typedef struct __attribute__((packed))
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_imu_ReadAccel 0
#define _MSGID_imu_ReadGyro 1


// Define type-safe pack/unpack functions for all enums, including included types

// Define pack/unpack functions for all structures, including included types


#endif // IMU_MESSAGES_H_INCLUDE_GUARD