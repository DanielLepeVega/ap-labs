#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
void mergeSort(void *v[], int leftEdge, int rightEdge,int (*comp)(void *, void *));

int numcmp(char *s1, char *s2){
    double val1, val2;
    val1 = atof(s1);
    val2 = atof(s2);

    if(val1 < val2){
        return -1;
    }
    else if(val1 > val2){
        return 1;
    }
    return 0;
}

void merge(void *v[], int leftEdge,int mid, int rightEdge,
    int (*comp)(void *, void *)){

    void** auxleftEdge = (void *) malloc((mid - leftEdge+1)*sizeof(void *));
    void** auxrightEdge = (void *) malloc((rightEdge - mid)*sizeof(void *));
    int j=0;
    for(int i =leftEdge ; i<=mid ; i++){
        auxleftEdge[j] = v[i];
        j++;
    }
    j=0;
    for(int i =mid+1 ; i<=rightEdge; i++){
        auxrightEdge[j] = v[i];
        j++;
    }
    int l=0,r=0;
    for(int i=leftEdge ; i<=rightEdge; i++){
        if(l > mid - leftEdge){
            v[i] = auxrightEdge[r];
            r++;
        }else if(r > rightEdge - (mid+1)){
            v[i] = auxleftEdge[l];
            l++;
        }else{
            int compare = ((* comp)( auxleftEdge[l], auxrightEdge[r]));
            if(compare>0){
                //rightEdge goes first.
                v[i] = auxrightEdge[r];
                r++;
            }else{
                //leftEdge goes first;
                v[i] = auxleftEdge[l];
                l++;
            }
        }
    }
    free(auxrightEdge);
    free(auxleftEdge);
}

void mergeSort(void *v[], int leftEdge, int rightEdge,int (*comp)(void *, void *))
{
    void merge(void *v[], int leftEdge,int mid, int rightEdge, int (*comp)(void *, void *));

    int mid;
    if (leftEdge >= rightEdge) /* do nothing if array contains */
        return; /* fewer than two elements */
    mid = ((rightEdge + leftEdge)/2);
    mergeSort(v, leftEdge, mid, comp);
    mergeSort(v, mid+1, rightEdge, comp);
    merge(v, leftEdge, mid, rightEdge, comp);
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
        }
        printf("\n"); 
    }   
}