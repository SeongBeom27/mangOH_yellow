/*
 * AUTO-GENERATED _componentMain.c for the led_Component component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _led_Component_le_gpioPin23_ServiceInstanceName;
const char** le_gpioPin23_ServiceInstanceNamePtr = &_led_Component_le_gpioPin23_ServiceInstanceName;
void le_gpioPin23_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t led_Component_LogSession;
le_log_Level_t* led_Component_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _led_Component_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _led_Component_Init(void)
{
    LE_DEBUG("Initializing led_Component component library.");

    // Connect client-side IPC interfaces.
    le_gpioPin23_ConnectService();

    // Register the component with the Log Daemon.
    led_Component_LogSession = log_RegComponent("led_Component", &led_Component_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_led_Component_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
