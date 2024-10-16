#include "logger.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

const char logFile[] = "/tmp/logfile.txt";
static int fd = -1;

void log_level_str(int level, char* buf)
{
	switch(level)
	{
	case LOG_ERROR:
		strcpy(buf, "Error");
		break;
	case LOG_DEBUG:
		strcpy(buf, "Debug");
		break;
	case LOG_MESSAGE:
		strcpy(buf, "Meaasge");
		break;
	default:
		strcpy(buf, "Unknown");
		break;
	}

	return ;
}

static int log_handle(int level, const char* fmt, va_list ap)
{
	char logBuffer[BUFFER_SIZE];
	char levelBuffer[16];
	int writeRet = 0;

	log_level_str(level, levelBuffer);
	snprintf(logBuffer, (BUFFER_SIZE - 1), "%s ", levelBuffer);
	vsnprintf(logBuffer + strlen(logBuffer), (BUFFER_SIZE - strlen(logBuffer) - 1), fmt, ap);
	if(level < DEFAULT_LOG_LEVEL)
		printf("%s", logBuffer);

	if(fd > 0)
		writeRet = write(fd, logBuffer, strlen(logBuffer));

	if(writeRet <= 0)
		printf("write to %s failed\n", logFile);

	return writeRet;
}

void log_error_deal(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	log_handle(LOG_ERROR, fmt, ap);
	va_end(ap);

	return ;
}

void log_debug_deal(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	log_handle(LOG_DEBUG, fmt, ap);
	va_end(ap);

	return ;
}

void log_message_deal(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	log_handle(LOG_MESSAGE, fmt, ap);
	va_end(ap);

	return ;
}

int log_system_init(void)
{
	int ret = 0;

	if(fd < 0)
	{
		fd = open(logFile, (O_WRONLY | O_APPEND | O_CREAT), (S_IRUSR | S_IWUSR));
		if(fd < 0){
			printf("open file %s failed\n", logFile);
			ret = -1;
		}
	}

	return ret;
}

void log_system_exit(void)
{
	if(fd > 0)
	{
		close(fd);
		fd = -1;
	}

	return ;
}
