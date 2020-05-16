#include "legato.h"
#include "interfaces.h"
#include <curl/curl.h>
#include "position.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fileUtils.h"

//data path for location objects
#define GPS_LAT                             "lwm2m.6.0.0"
#define GPS_LONG                            "lwm2m.6.0.1"
#define GPS_ALTITUDE                        "lwm2m.6.0.2"
#define GPS_RADIUS                          "lwm2m.6.0.3"

//Global variables
static le_posCtrl_ActivationRef_t   		_posCtrlRef = NULL;
//static le_avdata_RequestSessionObjRef_t		_requestSessionRef = NULL;

char* timeToString(struct tm *t) {
  static char s[20];

  sprintf(s, "%04d-%02d-%02d-%02d-%02d-%02d",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
              t->tm_hour, t->tm_min, t->tm_sec
          );

  return s;
}

void curl_test(char* data)
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
      LE_INFO("success!!!!");
    }
    curl_easy_cleanup(curl); // curl_easy_init

  }
}

le_result_t imu_ReadAccel
(
    double* xPtr,
        ///< [OUT] Where the x-axis acceleration (m/s2) will be put if LE_OK is returned.
    double* yPtr,
        ///< [OUT] Where the y-axis acceleration (m/s2) will be put if LE_OK is returned.
    double* zPtr
        ///< [OUT] Where the z-axis acceleration (m/s2) will be put if LE_OK is returned.
)
{
    le_result_t r;

    double scaling = 0.0;
    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_accel_scale", &scaling);
    //r = file_ReadDouble("/driver/in_accel_scale", &scaling);
    if (r != LE_OK)
    {
        goto done;
    }

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_accel_x_raw", xPtr);
    //r = file_ReadDouble("/driver/in_accel_x_raw", xPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *xPtr *= scaling;
    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_accel_y_raw", yPtr);
    //r = file_ReadDouble("/driver/in_accel_y_raw", yPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *yPtr *= scaling;

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_accel_z_raw", zPtr);
    //r = file_ReadDouble("/driver/in_accel_z_raw", zPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *zPtr *= scaling;
    LE_INFO("Accel sensor : {\"x\":%lf, \"y\":%lf, \"z\":%lf}", *xPtr, *yPtr, *zPtr);
done:
    return r;
}

le_result_t imu_ReadGyro
(
    double* xPtr,
        ///< [OUT] Where the x-axis rotation (rads/s) will be put if LE_OK is returned.
    double* yPtr,
        ///< [OUT] Where the y-axis rotation (rads/s) will be put if LE_OK is returned.
    double* zPtr
        ///< [OUT] Where the z-axis rotation (rads/s) will be put if LE_OK is returned.
)
{
    le_result_t r;

    double scaling = 0.0;
    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_anglvel_scale", &scaling);
    //r = file_ReadDouble("/driver/in_anglvel_scale", &scaling);
    if (r != LE_OK)
    {
        goto done;
    }

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_anglvel_x_raw", xPtr);
    //r = file_ReadDouble("/driver/in_anglvel_x_raw", xPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *xPtr *= scaling;

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_anglvel_y_raw", yPtr);
    //r = file_ReadDouble("/driver/in_anglvel_y_raw", yPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *yPtr *= scaling;

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_anglvel_z_raw", zPtr);
    //r = file_ReadDouble("/driver/in_anglvel_z_raw", zPtr);
    if (r != LE_OK)
    {
        goto done;
    }
    *zPtr *= scaling;

    LE_INFO("Gyro sensor : {\"x\":%lf, \"y\":%lf, \"z\":%lf}", *xPtr, *yPtr, *zPtr);
done:
    return r;
}

le_result_t temperature_Read
(
    double* readingPtr
        ///< [OUT] Where the reading (in degrees C) will be put if LE_OK is returned.
)
{
    le_result_t r;

    double scaling = 0.0;
    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_temp_scale", &scaling);
    if (r != LE_OK)
    {
        LE_ERROR("Failed to read scale");
        goto done;
    }
    double offset = 0.0;
    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_temp_offset", &offset);
    if (r != LE_OK)
    {
        LE_ERROR("Failed to read offset (%s)", LE_RESULT_TXT(r));
        goto done;
    }

    r = file_ReadDouble("/sys/bus/i2c/devices/6-0068/iio:device0/in_temp_raw", readingPtr);
    if (r != LE_OK)
    {
        LE_ERROR("Failed to read raw value (%s)", LE_RESULT_TXT(r));
        goto done;
    }
    *readingPtr = (*readingPtr + offset) * scaling / 1000;
    LE_INFO("temperature : %f", *readingPtr - 10);
done:
    return r;
}

//retrieve the current location, automatically get 2D or 3D location depending on the gnss fix state
position_location_type_t position_GetLocation
(
	double*     			dLatitude,
	double*     			dLongitude,
	int32_t*    			hAccuracy,
	int32_t*    			altitude,
	int32_t*    			vAccuracy,
	le_pos_FixState_t * 	fixStatePtr
)
{
	position_location_type_t	ret = POSITION_LOCATION_NO;    //0=KO, 1=2D, 2=3D
	le_pos_FixState_t			fixState = LE_POS_STATE_UNKNOWN;
	le_result_t					res;
	int32_t						latitude;
	int32_t						longitude;

	if (LE_OK == le_pos_GetFixState(&fixState))
	{
		LE_INFO("position fix state %d", fixState);

		if (LE_POS_STATE_FIX_3D == fixState)
		{
			res = le_pos_Get3DLocation(&latitude, &longitude, hAccuracy, altitude, vAccuracy);

			LE_INFO("le_pos_Get3DLocation %s",
					(LE_OK == res) ? "OK" : (LE_OUT_OF_RANGE == res) ? "parameter(s) out of range":"ERROR");
			LE_INFO("Get3DLocation latitude.%d, longitude.%d, hAccuracy.%d, altitude.%d, vAccuracy.%d", latitude, longitude, *hAccuracy, *altitude, *vAccuracy);

			if (LE_OK == res)
			{
				*dLatitude = (double)latitude/1000000.0;
				*dLongitude = (double)longitude/1000000.0;
				ret = POSITION_LOCATION_3D;
			}
		}
		else
		{
			res = le_pos_Get2DLocation(&latitude, &longitude, hAccuracy);

			LE_INFO("le_pos_Get2DLocation %s",
					(LE_OK == res) ? "OK" : (LE_OUT_OF_RANGE == res) ? "parameter(s) out of range":"ERROR");
			LE_INFO("Get2DLocation latitude.%d, longitude.%d, hAccuracy.%d",
					latitude, longitude, *hAccuracy);	

			if (LE_OK == res)
			{
				*dLatitude = (double)latitude/1000000.0;
				*dLongitude = (double)longitude/1000000.0;
				*altitude = 0;
				*vAccuracy = 0;
				ret = POSITION_LOCATION_2D;
				
				struct tm *t;
			    time_t timer;

				double temp_sample;
				double gyro_x,  gyro_y,  gyro_z;
				double accel_x, accel_y, accel_z;
			    timer = time(NULL);
			    t = localtime(&timer);
				int impact = 0;

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
				if(abs(gyro_x + gyro_y + gyro_z) > 0.2)
				{   
					impact = 1;
				}


				LE_INFO("** File write and read test **");

				FILE * fp;
				fp = fopen("r.json","w+");
				

				fprintf(fp, "isRegularData=true&dataString=");
				fprintf(fp, "[");	
				fprintf(fp, "{");	
				fprintf(fp, "\"tra_lat\": \"%f\",", *dLatitude);
			    fprintf(fp, "\"tra_lon\": \"%f\",", *dLongitude);	
                fprintf(fp, "\"tra_humidity\": \"%d\",", 50);
                fprintf(fp, "\"tra_temp\": \"%f\",", temp_sample-20);
			    fprintf(fp, "\"tra_datetime\": \"%s\",", timeToString(t));
			    fprintf(fp, "\"de_number\": \"%s\"", "legaro_proto0");
				fprintf(fp, "}");
				fprintf(fp, "]");	

				LE_INFO("Data read from the file is ==> %f , %f",*dLatitude,*dLongitude);

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


			    curl_test(buffer);

                fclose(fp);


			    free(buffer);
				FILE * fp2;
				fp2 = fopen("k.json","w+");

				fprintf(fp2, "isRegularData=false&dataString=");
				fprintf(fp2, "[");
				fprintf(fp2, "{");
				fprintf(fp2, "\"tra_impact\": \"%d\",", impact);
				fprintf(fp2, "\"tra_datetime\": \"%s\",", timeToString(t));
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

				curl_test(buffer2);

				fclose(fp2);

				free(buffer2);

			}
		}            
	}
	else
	{
		LE_INFO("Failed to GetFixState");
	}

	if (fixStatePtr)
	{
		*fixStatePtr = fixState;
	}

	return ret;
}


//a simple helper function to perform on-demand fix and post locatin to AirVantage
le_result_t position_PushLocation(le_pos_FixState_t * 	fixStatePtr)
{
	le_result_t res = LE_FAULT;

	double	latitude;
	double	longitude;
	int32_t	hAccuracy;
	int32_t	altitude;
	int32_t	vAccuracy;

	position_location_type_t ret = position_GetLocation(&latitude, &longitude, &hAccuracy, &altitude, &vAccuracy, fixStatePtr);


	if (POSITION_LOCATION_2D == ret)
	{
		res = LE_OK;
	} 
	return res;

}

//release positioning service and release session with AirVantage
void position_Stop()
{
	if (NULL != _posCtrlRef)
	{
		LE_INFO("Releasing positioning service");
		le_posCtrl_Release(_posCtrlRef);
	}
}

//Initialize the positioning service
void position_Start()
{
	_posCtrlRef = le_posCtrl_Request();
	if (NULL == _posCtrlRef)
	{
		LE_INFO("Cannot activate le_pos !");
	}
}

