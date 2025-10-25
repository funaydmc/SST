#pragma once

#define LOG_CRIT(fmt, ...) printf("[Critical] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) printf("[Error] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) printf("[Warning] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(fmt, ...) printf("[Info] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_TRACE(fmt, ...) printf("[Trace] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#ifdef _DEBUG
#define LOG_DEBUG(fmt, ...) printf("[Debug] [%s:%i] " ## fmt ## "\n",  __FILE__, __LINE__, __VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)
#endif