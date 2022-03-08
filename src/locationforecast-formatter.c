#include <json-c/json.h>
#include <stdbool.h>
#include <stdio.h>

int locationforecast_formatter(void) {
  FILE *fp;
  char buffer[150000];

  struct json_object *json_obj, *properties_obj, *timeseries_obj, *time_obj,
      *data_obj, *instant_obj, *details_obj, *air_temperature_obj,
      *relative_humidity_obj, *wind_from_direction_obj, *wind_speed_obj,
      *wind_speed_gust_obj, *next_6_hours_obj, *summary_obj, *symbol_code_obj;

  struct json_object *timeseries;
  size_t n_timeseries;

  size_t exists;

  fp = fopen("/tmp/MET-weather-data.json", "r");
  if (!fp) {
    perror("Error");
    return (-1);
  }
  fread(buffer, 150000, 1, fp);
  fclose(fp);
  fp = NULL;

  json_obj = json_tokener_parse(buffer);

  exists = json_object_object_get_ex(json_obj, "properties", &properties_obj);
  if (exists == false) {
    fprintf(stderr, "properties not found\n");
    return -1;
  }
  exists = json_object_object_get_ex(properties_obj, "timeseries", &timeseries_obj);
  if (exists == false) {
    fprintf(stderr, "timeseries not found\n");
    return -1;
  }


  n_timeseries = json_object_array_length(timeseries_obj);
  for (int i = 0; i < n_timeseries; i++) {
    timeseries = json_object_array_get_idx(timeseries_obj, i);

    exists = json_object_object_get_ex(timeseries, "time", &time_obj);
    if (exists == false) {
      fprintf(stderr, "time not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(timeseries, "data", &data_obj);
    if (exists == false) {
      fprintf(stderr, "data not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(data_obj, "instant", &instant_obj);
    if (exists == false) {
      fprintf(stderr, "instant not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(instant_obj, "details", &details_obj);
    if (exists == false) {
      fprintf(stderr, "air_temperature not found\n");
      return -1;
    }

    exists = json_object_object_get_ex(details_obj, "air_temperature", &air_temperature_obj);
    if (exists == false) {
      fprintf(stderr, "air_temperature not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(details_obj, "relative_humidity", &relative_humidity_obj);
    if (exists == false) {
      fprintf(stderr, "relative_humidity not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(details_obj, "wind_from_direction", &wind_from_direction_obj);
    if (exists == false) {
      fprintf(stderr, "wind_from_direction not found\n");
      return -1;
    }
    exists =
        json_object_object_get_ex(details_obj, "wind_speed", &wind_speed_obj);
    if (exists == false) {
      fprintf(stderr, "wind_speed not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(details_obj, "wind_speed_of_gust",
                                       &wind_speed_gust_obj);
    if (exists == false) {
      fprintf(stderr, "wind_from_direction not found\n");
      return -1;
    }
    exists = json_object_object_get_ex(data_obj, "next_6_hours", &next_6_hours_obj);
    if (exists == false) {
      fprintf(stderr, "next_6_hours not found\n");
      return -1;
    }
    exists =
        json_object_object_get_ex(next_6_hours_obj, "summary", &summary_obj);
    if (exists == false) {
      fprintf(stderr, "summary not found\n");
      return -1;
    }
    exists =
        json_object_object_get_ex(summary_obj, "symbol_code", &symbol_code_obj);
    if (exists == false) {
      fprintf(stderr, "symbol_code not found\n");
      return -1;
    }

    printf("\n%s\n", json_object_get_string(time_obj));
    printf("\n\t%s\n\n", json_object_get_string(symbol_code_obj));
    printf("Temperature: %s⁰C\n", json_object_get_string(air_temperature_obj));
    printf("Relative humidity: %.1f%%\n", json_object_get_double(relative_humidity_obj));
    printf("Wind from direction: %.1f°\n", json_object_get_double(wind_from_direction_obj));
    printf("Wind speed: %.1fm/s\n", json_object_get_double(wind_speed_obj));
    printf("Wind speed of gust: %.1fm/s\n", json_object_get_double(wind_speed_gust_obj));
  }

  return 0;
}