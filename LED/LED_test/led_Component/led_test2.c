#include <legato.h>
#include <stdio.h>
#include <stdlib.h>

#define SYS_GPIO_DIR "/sys/class/gpio"


void start()
{
	int port_num = 42;
	int val, i;

	char buff[256];
	FILE *fp;

	snprintf(buff, sizeof(buff), "%s/export", SYS_GPIO_DIR);
	fp = fopen(buff, "w");
	fprintf(fp, "%d\n", port_num);
	fclose(fp);

	
	snprintf(buff, sizeof(buff), "%s/gpio%d/direction", SYS_GPIO_DIR, port_num);
	fp = fopen(buff, "w");
	setvbuf(fp, NULL, _IONBF, 0);
	val = 0;
	for(i = 0; i < 10; i++)
	{
		fprintf(fp, "%d\n", val);
		val = !val;
	}

	fclose(fp);
}


COMPONENT_INIT
{
    LE_INFO("This is sample gpioctl Legato CF3 GPIO app by using le_gpio.api\n");
	start();
    return;
}

