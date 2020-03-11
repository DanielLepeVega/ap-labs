#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
void mergeSort(void *v[], int left, int right,int (*comp)(void *, void *));

void mergeSort(void *v[], int left, int right,int (*comp)(void *, void *))
{
    void merge(void *v[], int left,int mid, int right,
        int (*comp)(void *, void *));

    int mid;
    if (left >= right) /* do nothing if array contains */
        return; /* fewer than two elements */
    mid = ((right+left)/2);
    mergeSort(v, left, mid, comp);
    mergeSort(v, mid+1, right, comp);
    merge(v, left, mid, right, comp);
}

void merge(void *v[], int left,int mid, int right,
    int (*comp)(void *, void *)){

    void** auxLeft = (void *) malloc((mid - left+1)*sizeof(void *));
    void** auxRight= (void *) malloc((right - mid)*sizeof(void *));
    int j=0;
    for(int i =left ; i<=mid ; i++){
        auxLeft[j] = v[i];
        j++;
    }
    j=0;
    for(int i =mid+1 ; i<=right ; i++){
        auxRight[j] = v[i];
        j++;
    }
    int l=0,r=0;
    for(int i=left ; i<=right ; i++){
        if(l > mid - left){
            v[i] = auxRight[r];
            r++;
        }else if(r > right- (mid+1)){
            v[i] = auxLeft[l];
            l++;
        }else{
            int compare = ((* comp)( auxLeft[l], auxRight[r]));
            if(compare>0){
                //right goes first.
                v[i] = auxRight[r];
                r++;
            }else{
                //left goes first;
                v[i] = auxLeft[l];
                l++;
            }
        }
    }
    free(auxRight);
    free(auxLeft);
}
int numcmp(char *s1, char *s2) {
    double val1, val2;
    val1 = atof(s1);
    val2 = atof(s2);

    if(val1 < val2) {
        return -1;
    }
    else if(val1 > val2){
        return 1;
    }
    return 0;
}

int main(int argc, char** argv){
    char numericValue = 0;
    if(argc > 1 && strcmp(argv[1], "-n")==0){
        numericValue = 1;
    }
    if(argc>1){
        mergeSort((void *)argv, numericValue? 2 : 1, argc-1, (int(*)(void *, void *)) numericValue? numcmp:strcmp);
        for(int i=2;i<argc;i++){
            printf("%s ", argv[i]);
        }printf("\n"); 
    }   
}