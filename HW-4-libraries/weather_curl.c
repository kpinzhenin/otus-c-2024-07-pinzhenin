#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <cjson/cJSON.h>

// data storage struct
struct MemoryStruct{
    char *memory;
    size_t size;
};

char* json_Get_request_string(cJSON *json);
char* json_Get_weather_string(cJSON *json);
void json_check_not_NULL(cJSON *json, char* error_msg);

void curl_request_build(char *res, char *city_name);

// hunder func from example
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp){
	size_t realsize = size * nmemb;
	userp->memory = realloc(userp->memory, userp->size + realsize + 1);
	if (userp->memory == NULL){
		printf("Not enough memory!\n");
		return 0; // out of memory!
	}
	memcpy(&(userp->memory[userp->size]), contents, realsize);
	userp->size += realsize;
	userp->memory[userp->size] = '\0'; // null-terminate the string
	return realsize;
}

int main(int argc, char *argv[]){
	// check input
	if (argc != 2){
		printf("Error. Input shoud contain one argument\n");
		exit(1);
	}
	if (argv[1] == NULL){
		printf("Error. NULL pointer to location name");
		exit(1);
	}

	// calc request size
	size_t req_str_len = strlen("https://wttr.in/") + strlen("?format=j1") + strlen(argv[1]);
	char request[req_str_len + 1]; // +1 for 0 terminator
	// build request string
	curl_request_build(request, argv[1]);

	CURL *curl;
	CURLcode res;
	struct MemoryStruct chunk;

	// init Struct
	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl){
		//for current worked link
		curl_easy_setopt(curl, CURLOPT_URL, request);

		// set Call-back function 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		// request to URL
		res = curl_easy_perform(curl);
		if (res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else{
			// parse data
			cJSON *json = cJSON_Parse(chunk.memory);

			if (json == NULL){
				printf("Error parsing JSON\n");
			}
			else
			{
				// get city into request for check valid city
				char *json_request_array_str = json_Get_request_string(json);

				if(strcmp(json_request_array_str, "\"Ban Not, Vietnam\"") == 0)
					printf("Error. not valid city name \n");
				else
				{
					// get discription about weather
					cJSON *current_condittion_array = cJSON_GetArrayItem( cJSON_GetObjectItem(json, "current_condition"), 0);
					json_check_not_NULL(current_condittion_array, "Error. NULL pointer to item \"current_condition\"");
					// get temperature in celsium
					cJSON *json_item_temp_c = cJSON_GetObjectItem(current_condittion_array, "temp_C");
					json_check_not_NULL(json_item_temp_c, "Error. NULL pointer to item \"temp_C\"");
					char *json_temp_c_string = cJSON_Print(json_item_temp_c);

					// get weather discription
					char *json_weather_disc = json_Get_weather_string(current_condittion_array);

					// get wind speed
					char *json_windSpeed_string = cJSON_Print(cJSON_GetObjectItem(current_condittion_array, "windspeedKmph"));

					json_check_not_NULL(current_condittion_array, "Error. NULL pointer to item \"windspeedKmph\"");

					printf("discription: %s, temp: %s°C windspeed: %skmph\n", json_weather_disc, json_temp_c_string, json_windSpeed_string);
					// clear printed message memory
					free(json_temp_c_string);
					free(json_weather_disc);
					free(json_windSpeed_string);
				}
				cJSON_Delete(json); // clean json memory
			}
		}
		// clean other memory
		curl_easy_cleanup(curl);
	}
	free(chunk.memory); //clean chunk any case
	curl_global_cleanup();
	return 0;
}

char* json_Get_request_string(cJSON *json){
	cJSON *json_request_array = cJSON_GetObjectItem(json, "request");
	// check valid city name, for bad request wttr return "Bad not, Vietnam" item in obj "request"
	json_check_not_NULL(json_request_array,"Error. NULL pointer to item \"request\"");

	// get it
	cJSON *json_query_city = cJSON_GetObjectItem(cJSON_GetArrayItem(json_request_array,0),"query");
	json_check_not_NULL(json_query_city, "Error. NULL pointer to item \"query\"");

	// extract request string
	return cJSON_Print(json_query_city);
}

char* json_Get_weather_string(cJSON *json){
	// weather discription is object inside the array value of "weatherDesc" string

	// at first get item, which is array  
	cJSON *json_weather_array = cJSON_GetObjectItem(json, "weatherDesc");

	// go to the "weatherDesc" array
	cJSON *json_weatherItem = cJSON_GetArrayItem(json_weather_array, 0);

	// return weather discription
	return cJSON_Print(cJSON_GetObjectItem(json_weatherItem, "value"));
}

void json_check_not_NULL(cJSON *json, char* error_msg){
	if(json == NULL)
	{
		printf("%s\n", error_msg);
		cJSON_Delete(json);
		exit(1);
	}
}

void curl_request_build(char *res, char *city_name){
	res = stpcpy(res,"https://wttr.in/");
	res = stpcpy(res, city_name);
	res = stpcpy(res, "?format=j1");
	
}

