#include "legato.h"
#include "interfaces.h"
#include <curl/curl.h>
#include "position.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global variable

static le_timer_Ref_t						_positionTimerRef = NULL;  
//static le_data_RequestObjRef_t RequestRef = NULL;
char* TimeToString(struct tm *t) {
  static char s[20];

  sprintf(s, "%04d-%02d-%02d-%02d-%02d-%02d",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
              (t->tm_hour), t->tm_min, t->tm_sec
          );

  return s;
}

void Curl_test(char* data)
{
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();

  struct curl_slist *list = NULL;

if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://bnrtracker.dreammug.com/_API/saveDataFromString.php");
   // curl_easy_setopt(curl, CURLOPT_URL, "http://54.180.83.60:8000");

    curl_easy_setopt(curl, CURLOPT_POST, 1L); //POST option
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);

    res = curl_easy_perform(curl);
    curl_slist_free_all(list); // C

    if(res != CURLE_OK){
      LE_INFO("curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
    }
    else{
      LE_INFO("gps is not work , data connect success!!!!");
    }
    curl_easy_cleanup(curl); // curl_easy_init

  }
}

void OnDemandFix();

//Function to start the timer with a specified delay (seconds)
void SetTimerDelay(uint32_t	delaySecond)
{
	if (!_positionTimerRef)
	{
		_positionTimerRef = le_timer_Create("positionTimer");     //create timer
		le_timer_SetRepeat(_positionTimerRef, 1);                 //set repeat to once

		//set callback function to handle timer expiration event
		le_timer_SetHandler(_positionTimerRef, OnDemandFix);
	}	

	le_clk_Time_t      interval = { delaySecond, 0 };             //set timer delay
	le_timer_SetInterval(_positionTimerRef, interval);

	LE_INFO("Next update in %d seconds...", delaySecond);
	
	//start timer
	le_timer_Start(_positionTimerRef);
}

//function called by the above timer upon expiration
//this function call helper lib's position_PushLocation to push the current location to AirVantage
//the timer delay varies depending on the gnss fix state
void OnDemandFix()
{
	le_pos_FixState_t 	fixState;

	int impact;
	position_PushLocation(&fixState);

	if (LE_POS_STATE_FIX_3D == fixState)
	{
		SetTimerDelay(40);	//make delay longer as we get 3Dfix
	}
	else if (LE_POS_STATE_FIX_2D == fixState)
	{
		SetTimerDelay(10);
	}
	else
	{
		// location isn't fixed
		SetTimerDelay(10);
		struct tm *t;
		time_t timer;
		double temp_sample;
		double gyro_x,  gyro_y,  gyro_z;
	    double accel_x, accel_y, accel_z;
		timer = time(NULL);
		t = localtime(&timer);
        le_result_t temp_result = temperature_Read(&temp_sample);
        if (temp_result == LE_OK)
        {
            LE_INFO("Get Temperature sensor Success!!!!!!!!!");
        }

        le_result_t gyro_result = imu_ReadGyro(&gyro_x, &gyro_y, &gyro_z);
        if (gyro_result == LE_OK)
        {
            LE_INFO("Get Gyro sensor Success!!!!!!!!!");
        }

        le_result_t accel_result = imu_ReadAccel(&accel_x, &accel_y, &accel_z);
        if (accel_result == LE_OK)
        {
            LE_INFO("Get Accel sensor Success!!!!!!!!!");
        }

		impact = 0;
		if(abs(gyro_x + gyro_y + gyro_z) > 0.2)
		{
			impact = 1;
			LE_INFO("gyro sum value : %d", abs(gyro_x + gyro_y + gyro_z));
		}
		FILE * fp;
		fp = fopen("k.json","w+");
		
		fprintf(fp, "isRegularData=true&dataString=");
		fprintf(fp, "[");
		fprintf(fp, "{");
		fprintf(fp, "\"tra_lat\": \"%s\",", "*****");
		fprintf(fp, "\"tra_lon\": \"%s\",", "*****");
		fprintf(fp, "\"tra_humidity\": \"%d\",", 60);
		fprintf(fp, "\"tra_temp\": \"%f\",", temp_sample-20);
		fprintf(fp, "\"tra_datetime\": \"%s\",", TimeToString(t));
		fprintf(fp, "\"de_number\": \"%s\"", "legaro_proto0");
		fprintf(fp, "}");
		fprintf(fp, "]");

		char *buffer;
		int size;
		int number;

		fseek(fp, 0, SEEK_END);
		size = ftell(fp);

		buffer = malloc(size + 1);
		memset(buffer, 0, size + 1);

		fseek(fp, 0, SEEK_SET);
		number = fread(buffer, size, 1, fp);

		LE_INFO("%d", number);
		LE_INFO("%s\n", buffer);

		Curl_test(buffer);

		fclose(fp);

		free(buffer);


        FILE * fp2;
        fp2 = fopen("k.json","w+");

        fprintf(fp2, "isRegularData=false&dataString=");
        fprintf(fp2, "[");
        fprintf(fp2, "{");
        fprintf(fp2, "\"tra_impact\": \"%d\",", impact);
        fprintf(fp2, "\"tra_datetime\": \"%s\",", TimeToString(t));
        fprintf(fp2, "\"de_number\": \"%s\"", "legaro_proto0");
        fprintf(fp2, "}");
        fprintf(fp2, "]");

        char *buffer2;
        int size2;
        int number2;

        fseek(fp2, 0, SEEK_END);
        size2 = ftell(fp2);

        buffer2 = malloc(size2 + 1);
        memset(buffer2, 0, size2 + 1);

        fseek(fp2, 0, SEEK_SET);
        number2 = fread(buffer2, size2, 1, fp2);

        LE_INFO("%d", number2);
        LE_INFO("%s\n", buffer2);

        Curl_test(buffer2);

        fclose(fp2);

        free(buffer2);
	}
}


//main start
COMPONENT_INIT
{
	le_sig_Block(SIGTERM);
	le_sig_SetEventHandler(SIGTERM, position_Stop); //call position_Stop upon program exit
	
    position_Start();
    SetTimerDelay(10);
}
