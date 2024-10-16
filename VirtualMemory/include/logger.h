#ifndef __LOGGER_H__
#define __LOGGER_H__

#define BUFFER_SIZE (4096)
#define DEFAULT_LOG_LEVEL (1)
#define LOG_ERROR (0)
#define LOG_DEBUG (1)
#define LOG_MESSAGE (2)

int log_system_init(void);
void log_system_exit(void);
void log_error_deal(const char* fmt, ...);
void log_debug_deal(const char* fmt, ...);
void log_message_deal(const char* fmt, ...);

// used for others
#define log_error(fmt, ...) \
	log_error_deal("[%s]-%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_debug(fmt, ...) \
	log_debug_deal("[%s]-%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_message(fmt, ...) \
	log_message_deal("[%s]-%d: " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif