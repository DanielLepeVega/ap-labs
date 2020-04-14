#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

int myMethVPrintF(const char *format, va_list arg){
	int r = vprintf(format, arg);
	printf("\033[0m"); // Reset color
	va_end(arg);
	return r;
}

int infof(const char *format, ...){
	printf("\033[0m"); // Reset color
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int warnf(const char *format, ...){
	printf("\033[0;33m"); // Set color to yellow
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int errorf(const char *format, ...){
	printf("\033[0;31m"); // Set color red
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int panicf(const char *format, ...){
	printf("\033[1;35m"); // Set color to bald magenta
	va_list arg;
	va_start(arg, format);
	myMethVPrintF(format, arg);
	abort();
	return 0;
}