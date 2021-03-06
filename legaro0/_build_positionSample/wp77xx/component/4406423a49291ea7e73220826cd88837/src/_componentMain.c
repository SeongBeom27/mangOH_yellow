/*
 * AUTO-GENERATED _componentMain.c for the fileUtils component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component log session variables.
le_log_SessionRef_t fileUtils_LogSession;
le_log_Level_t* fileUtils_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _fileUtils_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _fileUtils_Init(void)
{
    LE_DEBUG("Initializing fileUtils component library.");

    // Register the component with the Log Daemon.
    fileUtils_LogSession = log_RegComponent("fileUtils", &fileUtils_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_fileUtils_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
