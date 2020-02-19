#include <stdlib.h>
#include <stdio.h>

/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
        int fahr;
        if(argc > 2){
	    int start = atoi(argv[1]);
	    int end = atoi(argv[2]);
	    int steps = atoi(argv[3]);
            for (fahr=start; fahr<=end; fahr=fahr+steps){
                printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
	    }
        }
        else{
            float num_farenheit_degress = atof(argv[1]);
            printf("Fahrenheit: %3f, Celcius: %6.1f\n", num_farenheit_degress, (num_farenheit_degress-32)*(5.0/9.0));
        }    
        return 0;
}

