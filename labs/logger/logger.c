#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "logger.h"

int myMethVPrintF(const char *format, va_list arg){
	fflush(stdin); 
	actualTime();
	int r = vprintf(format, arg);
	printf("\033[0m"); // resetColor
	va_end(arg);
	return r;
}

int infof(const char *format, ...){
	fflush(stdin);
	actualTime(); 
	printf("\033[0m"); // resetColor
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int warnf(const char *format, ...){
	fflush(stdin);
	actualTime(); 
	printf("\033[0;33m"); // setColorToYellow
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int errorf(const char *format, ...){
	fflush(stdin);
	actualTime(); 
	printf("\033[0;31m"); // setColorRed
	va_list arg;
	va_start(arg, format);
	return myMethVPrintF(format, arg);
}

int panicf(const char *format, ...){
	fflush(stdin);
	actualTime(); 
	printf("\033[1;35m"); // setColorToBaldMagenta
	va_list arg;
	va_start(arg, format);
	myMethVPrintF(format, arg);
	abort();
	return 0;
}

int actualTime ()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  printf ( "Current local time and date: %s", asctime (timeinfo) );

  return 0;
}