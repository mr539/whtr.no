#include <json-c/json.h>
#include <stdbool.h>
#include <stdio.h>
#include "degrees-to-compass-diretions.h"
#include "met-data-fetcher.h"
#include "write-file.h"
#include "reformat-timestamp.h"

int locationforecast_formatter(void) {
	char *buffer = NULL;
	if ((buffer = read_file(TMPFILE)) == NULL) {
		fprintf(stderr, "Error reading file.\n");
		return -1;
	}

	struct json_object *json_obj, *properties_obj, *timeseries_obj, *time_obj,
		*data_obj, *instant_obj, *instant_details_obj, *details_obj, *air_temperature_obj,
		*wind_from_direction_obj, *wind_speed_obj, *wind_speed_gust_obj, *next_6_hours_obj, 
		*next_1_hours_obj, *summary_obj, *symbol_code_obj, *precipitation_amount_obj;

	struct json_object *timeseries;

	size_t n_timeseries;
	size_t object;

	json_obj = json_tokener_parse(buffer);

	object = json_object_object_get_ex(json_obj, "properties", &properties_obj);
	if (object == false) {
		fprintf(stderr, "properties not found\n");
		return -1;
	}
	object = json_object_object_get_ex(properties_obj, "timeseries", &timeseries_obj);
	if (object == false) {
		fprintf(stderr, "timeseries not found\n");
		return -1;
	}

	n_timeseries = json_object_array_length(timeseries_obj);
	for (int i = 0; i < n_timeseries; i++) {
		timeseries = json_object_array_get_idx(timeseries_obj, i);

		object = json_object_object_get_ex(timeseries, "time", &time_obj);
		if (object == false) {
			fprintf(stderr, "time not found (%s)\n", json_object_get_string(time_obj));
			return -1;
		}
		object = json_object_object_get_ex(timeseries, "data", &data_obj);
		if (object == false) {
				fprintf(stderr, "data not found (%s)\n", json_object_get_string(time_obj));
				return -1;
		}
		object = json_object_object_get_ex(data_obj, "instant", &instant_obj);
		if (object == false) {
				fprintf(stderr, "instant not found (%s)\n", json_object_get_string(time_obj));
				return -1;
		}
		object = json_object_object_get_ex(instant_obj, "details", &instant_details_obj);
		if (object == false) {
			fprintf(stderr, "air_temperature not found (%s)\n", json_object_get_string(time_obj));
			return -1;

		}
		object = json_object_object_get_ex(instant_details_obj, "air_temperature", &air_temperature_obj);
		if (object == false) {
			fprintf(stderr, "air_temperature not found (%s)\n", json_object_get_string(time_obj));
			return -1;
		}
		object = json_object_object_get_ex(instant_details_obj, "wind_from_direction", &wind_from_direction_obj);
		if (object == false) {
			fprintf(stderr, "wind_from_direction not found (%s)\n", json_object_get_string(time_obj));
			return -1;
		}
		object = json_object_object_get_ex(instant_details_obj, "wind_speed", &wind_speed_obj);
		if (object == false) {
				fprintf(stderr, "wind_speed not found (%s)\n", json_object_get_string(time_obj));
				return -1;
		}

		object = json_object_object_get_ex(instant_details_obj, "wind_speed_of_gust", &wind_speed_gust_obj);

		object = json_object_object_get_ex(data_obj, "next_6_hours", &next_6_hours_obj);
		if (object != false) {
			object = json_object_object_get_ex(next_6_hours_obj, "summary", &summary_obj);
			if (object == false) {
					fprintf(stderr, "summary not found (%s)\n", json_object_get_string(time_obj));
					return -1;
			}
			object = json_object_object_get_ex(next_6_hours_obj, "details", &details_obj);
			if (object==false) {
					fprintf(stderr, "details not found (%s)\n", json_object_get_string(time_obj));
					return -1;
			}
		} else if ((object = json_object_object_get_ex(data_obj, "next_1_hours", &next_1_hours_obj)) != false) {
			object = json_object_object_get_ex(next_1_hours_obj, "summary", &summary_obj);
			if (object == false) {
					fprintf(stderr, "summary not found (%s)\n", json_object_get_string(time_obj));
					return -1;
			}
			object = json_object_object_get_ex(next_1_hours_obj, "details", &details_obj);
			if (object==false) {
					fprintf(stderr, "details not found (%s)\n", json_object_get_string(time_obj));
					return -1;
			}
		} else continue;

		object = json_object_object_get_ex(summary_obj, "symbol_code", &symbol_code_obj);
		if (object == false) {
				fprintf(stderr, "symbol_code not found (%s)\n", json_object_get_string(time_obj));
				return -1;
		}
		object = json_object_object_get_ex(details_obj, "precipitation_amount", &precipitation_amount_obj);
		if (object==false) {
			fprintf(stderr, "precipitation_amount not found (%s)\n", json_object_get_string(time_obj));
			return -1;
		}

		/* Timestamp*/
		struct timestamp *timestamp = reformat_timestamp((char *)json_object_get_string(time_obj), timezone);
		printf("%d:%d %d.%d.%d\n", timestamp->hour, timestamp->minute, timestamp->day, timestamp->month, timestamp->year);
		free(timestamp);
		/* Symbol code */
		printf("\t%s\n", json_object_get_string(symbol_code_obj));
		/* Temperature */
		printf("\t%s ⁰C\n", json_object_get_string(air_temperature_obj));
		/* Wind direction */
		printf("\t%s ", degrees_to_compass_directions(
					json_object_get_double(wind_from_direction_obj)
				)
		);
		/* Wind speed and gust */
		printf("%.1f ", json_object_get_double(wind_speed_obj));
		if (json_object_get_double(wind_speed_gust_obj))
			printf("(%.1f) ", json_object_get_double(wind_speed_gust_obj));
		printf("m/s\n");
		/* Percipitation amount */
		printf("\t%.1f mm\n", json_object_get_double(precipitation_amount_obj));
	}

	free(buffer);
	return 0;
}
