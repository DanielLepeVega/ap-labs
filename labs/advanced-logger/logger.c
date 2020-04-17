#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include "logger.h"

int logType; //stdout: 0 - syslog : 1

int actualTime ()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  printf ( "Current local time and date: %s", asctime (timeinfo) );

  return 0;
}

int myVPrintf(const char *format, va_list arg){
    actualTime();
    int ll = 0;
    if(logType){
        vsyslog(LOG_INFO, format, arg);
    }else{
        ll = vprintf(format, arg);
    }
	printf("\033[0m"); // Reset color
	va_end(arg);
	return ll;
}

int initLogger(char *logType) {
    actualTime();
    // If string is not null or empty it must apply as stdout option
    if(!logType || strcmp(logType, "stdout") == 0 || !logType[0]){
        if(loggerType == 1){
            printf("Initializing Logger on: stdout\n");
            loggerType = 0;
            closelog();
        } else{
            printf("Already initialized on: stdout\n");
        }
    }
    else if(strcmp(logType, "syslog") == 0){
        if(loggerType == 0){
            printf("Initializing Logger on: syslog\n");
            //const char *ident = "SYSLOG: ";
            openlog(NULL, LOG_NDELAY, LOG_USER);
            loggerType = 1;
        } else{
            printf("Already initialized on: syslog\n");
        }     
    }
    else{
        errorf("Logger option '%s' is unknown\n", logType);
        return -1;
    }
    return 0;
}

int infof(const char *format, ...){
    actualTime();
	printf("\033[0m"); // Reset color
	va_list arg;
	va_start(arg, format);
	return myVPrintf(format, arg);
}

int warnf(const char *format, ...){
    actualTime();
	printf("\033[0;33m"); // Set color to yellow
	va_list arg;
	va_start(arg, format);
	return myVPrintf(format, arg);
}

int errorf(const char *format, ...){
    actualTime();
	printf("\033[0;31m"); // Set color red
	va_list arg;
	va_start(arg, format);
	return myVPrintf(format, arg);
}

int panicf(const char *format, ...){
    actualTime();
	printf("\033[1;35m"); // Set color to bald magenta
	va_list arg;
	va_start(arg, format);
	myVPrintf(format, arg);
	abort();
	return -1;
}
