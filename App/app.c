// APP HEADER //
#include "app.h"

#define MY_GPIO_H_
#include "stm32f4_header.h"


static void Status_Led(void*);

int test = 0;

#define         SCHEDULER_TASK_COUNT  6
uint32_t        g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;

tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                        {&Debug_CMD_Line_Task,			(void *) 0,	50,		0, 	true},
						{&H_Bridge_Task,				(void *) 0,	1,		0, 	true},
                        {&FSP_Line_Task,				(void *) 0,	20, 	0, 	true},
						{&VOM_Task,						(void *) 0,	1000, 	0, 	false},
						{&impedance_task,				(void *) 0,	100, 	0, 	true},
                        {&Status_Led,					(void *) 0,	100000,	0,	false},
                };

void App_Main(void)
{
    CMD_Line_Task_Init();
    VS_Init();
    H_Bridge_Task_Init();
    FSP_Line_Task_Init();
    VOM_Task_Init();

    SchedulerInit(10000);

    while (1)
    {
    	test++;
        SchedulerRun();
    }
}

static void Status_Led(void*)
{

}

