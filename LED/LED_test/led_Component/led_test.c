/**
 * @file gpioPin.c
 *
 * This is sample Legato CF3 GPIO app by using le_gpio.api.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless, Inc.
 */

/* Legato Framework */
#include "legato.h"
#include "interfaces.h"

static le_timer_Ref_t                       _positionTimerRef = NULL;

static int Pin23 = 23;

static void Pin23ChangeCallback(bool state, void *ctx){
    LE_INFO("State change %s", state?"TRUE":"FALSE");

    LE_INFO("Context pointer came back as %d", *(int *)ctx);
}

static void Pin23GpioSignal();

void SetTimerDelay(uint32_t delaySecond)
{
    if (!_positionTimerRef)
    {
        _positionTimerRef = le_timer_Create("positionTimer");     //create timer
        le_timer_SetRepeat(_positionTimerRef, 1);                 //set repeat to once

        //set callback function to handle timer expiration event
        le_timer_SetHandler(_positionTimerRef, Pin23GpioSignal);
    }

    le_clk_Time_t      interval = { delaySecond, 0 };             //set timer delay
    le_timer_SetInterval(_positionTimerRef, interval);

    LE_INFO("Next update in %d seconds...", delaySecond);

    //start timer
    le_timer_Start(_positionTimerRef);
}

static void Pin23GpioSignal()
{
    bool value = false;
    le_gpioPin23_SetInput(LE_GPIOPIN23_ACTIVE_LOW);
    value = le_gpioPin23_Read();
    LE_INFO("Pin23 read active: %d", value);

    le_gpioPin23_ChangeEventHandlerRef_t ref = le_gpioPin23_AddChangeEventHandler(LE_GPIOPIN23_EDGE_FALLING, Pin23ChangeCallback, &Pin23, 0);

    // Change the edge setting
    le_gpioPin23_SetEdgeSense(LE_GPIOPIN23_EDGE_BOTH);
    le_gpioPin23_DisableEdgeSense();
    le_gpioPin23_SetEdgeSense(LE_GPIOPIN23_EDGE_RISING);

    // Remove the handler
    le_gpioPin23_RemoveChangeEventHandler(ref);

    LE_INFO("Pin 23 is output = %s", le_gpioPin23_IsOutput()?"true":"false");
	
	SetTimerDelay(1);
}
// -------------------------------------------------------------------------------------------------
/**
 *  Pin-per-service GPIO pin21 as example
 */
// -------------------------------------------------------------------------------------------------

/*
static void Pin21GpioSignal()
{
    bool active;

    le_gpioPin21_Activate();
    le_gpioPin21_EnablePullUp();
    le_gpioPin21_Deactivate();

    le_gpioPin21_SetInput(1);
    active = le_gpioPin21_Read();
    LE_INFO("Pin21 read active: %d", active);

    le_gpioPin21_SetPushPullOutput(LE_GPIOPIN21_ACTIVE_HIGH, true);
    LE_INFO("Pin21 read PP - High: %d", le_gpioPin21_Read());

    le_gpioPin21_SetPushPullOutput(LE_GPIOPIN21_ACTIVE_LOW, true);
    LE_INFO("Pin21 read PP - Low: %d", le_gpioPin21_Read());

    le_gpioPin21_SetPushPullOutput(LE_GPIOPIN21_ACTIVE_HIGH, false);
    LE_INFO("Pin21 read PP - High: %d", le_gpioPin21_Read());

    le_gpioPin21_SetPushPullOutput(LE_GPIOPIN21_ACTIVE_LOW, false);
    LE_INFO("Pin21 read PP - Low: %d", le_gpioPin21_Read());
}
*/
//-------------------------------------------------------------------------------------------------
/**
 *  Pin-per-service GPIO pin22 as example
 */
// -------------------------------------------------------------------------------------------------
static void PinsReadConfig()
{
    //LE_INFO("Pin 21 active = %s", le_gpioPin21_IsActive()?"true":"false");

    

	le_gpioPin23_Edge_t edge = le_gpioPin23_GetEdgeSense();
    if (edge == LE_GPIOPIN23_EDGE_FALLING)
    {
        LE_INFO("Pin 23 edge sense = falling");
    }
    else if (edge == LE_GPIOPIN23_EDGE_RISING)
    {
        LE_INFO("Pin 23 edge sense = rising");
    }
    else if (edge == LE_GPIOPIN23_EDGE_BOTH)
    {
        LE_INFO("Pin 23 edge sense = both");
    }
    else if (edge == LE_GPIOPIN23_EDGE_NONE)
    {
        LE_INFO("Pin 23 edge sense = none");
    }

  /*  le_gpioPin21_Polarity_t pol = le_gpioPin21_GetPolarity();
    if (pol == LE_GPIOPIN21_ACTIVE_HIGH)
    {
        LE_INFO("Pin 21 polarity = ACTIVE_HIGH");
    }
    else
    {
        LE_INFO("Pin 21 polarity = ACTIVE_LOW");
    }

    LE_INFO("Pin 21 is input = %s", le_gpioPin21_IsInput()?"true":"false");

    le_gpioPin21_PullUpDown_t pud = le_gpioPin21_GetPullUpDown();
    if (pud == LE_GPIOPIN21_PULL_DOWN)
    {
        LE_INFO("Pin 21 pull up/down = down");
    }
    else if (pud == LE_GPIOPIN21_PULL_UP)
    {
        LE_INFO("Pin 21 pull up/down = up");
    }
    else
    {
        LE_INFO("Pin 21 pull up/down = none");
    }*/
}

/*static void SecondCallbackTest()
{
    // Try adding this twice
    // le_gpioPin22_AddChangeEventHandler(LE_GPIOPIN22_EDGE_RISING, Pin22ChangeCallback, &Pin22, 0);
    le_gpioPin22_AddChangeEventHandler(LE_GPIOPIN22_EDGE_FALLING, Pin22ChangeCallback, &Pin22, 0);
}
*/
COMPONENT_INIT
{
    LE_INFO("This is sample gpioctl Legato CF3 GPIO app by using le_gpio.api\n");

//    Pin21GpioSignal();
    Pin23GpioSignal();

    PinsReadConfig();

    // This should abort the test app
	
//	SecondCallbackTest();
	
	SetTimerDelay(1);
    return;
}

