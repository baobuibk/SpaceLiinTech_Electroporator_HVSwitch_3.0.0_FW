// APP HEADER //
#include "app.h"

#define MY_GPIO_H_
#include "stm32f4_header.h"


static void Status_Led(void*);


#define         SCHEDULER_TASK_COUNT  3
uint32_t        g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;

tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                        {&Debug_CMD_Line_Task,			(void *) 0,	50,		0, 	true},
						{&H_Bridge_Task,				(void *) 0,	1,	0, 	true},
                        {&Status_Led,					(void *) 0,	100000,	0,	false},
                };

void App_Main(void)
{
    CMD_Line_Task_Init();
    V_Switch_Driver_Init();
    H_Bridge_Task_Init();

    SchedulerInit(10000);

    while (1)
    {
        SchedulerRun();
    }
}

static void Status_Led(void*)
{

}

