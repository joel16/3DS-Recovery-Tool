#include "clock.h"
#include "screen.h"

void digitalTime(void)
{
	float width = 0;
	
	time_t unix_time = time(0);
	struct tm* time_struct = gmtime((const time_t*)&unix_time);
	int hours = time_struct->tm_hour;
	int minutes = time_struct->tm_min;
	int amOrPm = 0;
	
	if(hours < 12)
		amOrPm = 1;
	if(hours == 0)
		hours = 12;
	else if(hours > 12)
		hours = hours - 12;

	if ((hours >= 1) && (hours < 10))  
	{
		width = screen_get_string_width("0:00 XM", 0.41f, 0.41f);
		screen_draw_stringf((395 - width), 1, 0.41f, 0.41f, RGBA8(240, 242, 242, 255), "%2i:%02i %s", hours, minutes, amOrPm ? "AM" : "PM");
	}
	else
	{
		width = screen_get_string_width("00:00 XM", 0.41f, 0.41f);
		screen_draw_stringf((395 - width), 1, 0.41f, 0.41f, RGBA8(240, 242, 242, 255), "%2i:%02i %s", hours, minutes, amOrPm ? "AM" : "PM");
	}
}