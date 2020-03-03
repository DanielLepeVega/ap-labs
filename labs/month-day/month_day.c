#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>

/* month_day function's prototype*/
static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

char months[13][12] = {
	"0", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

void month_day(int year, int yearday){
	int i = 1, leap = 0, pday = 0, pmonth = 0;
	bool cycle = true;
	leap = year%4 == 0 && year%100 !=0 || year%400 == 0;
	//printf("----------------");
	//printf("%d", i);
	//printf("leap: %d ", leap);

	//int tempPDay = atoi(*pday);
	//int tempMonth = atoi(*pmonth);
	//printf("tempDay: %d", tempPDay);
	//printf("tempMonth: %d", tempMonth);
	while(cycle){
		//printf("----------------%d", i);
		if(yearday > pday){
			//tempPDay += daytab[leap][i];
			//tempMonth++;
			pday = pday + daytab[leap][i];
			pmonth++;
			//printf("--***--------------");
			//printf("yearday: %d ", yearday);
			//printf("pday: %d ", pday);
			i++;
		}
		else{//hacer if  del para dias del 1 al 9
			//printf("yearday: %d ", yearday);
			//printf("pday: %d ", pday);
			//printf("pmonth: %d ", pmonth);
			//printf("-*-*-*");
			//pmonth--;
			printf(months[pmonth]);
			//pmonth = pmonth - 2;
			printf(" %d,", yearday - pday + daytab[leap][pmonth]);
			printf(" %d", year);
			cycle = false;
		}
	}
}

int main(int argc, char **argv[]) {
	int year = atoi(argv[1]),
		yearday = atoi(argv[2]);

	month_day(year, yearday);
	return 0;
}