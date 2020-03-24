#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* fileCopy:  copy file ifp to file ofp */
int fileCopy(int ifp, int ofp)
{
    int n;
    char c[100];
    while ((n = read(ifp, c, sizeof(c))) > 0)
        if (write(ofp, c, n) < 0)
            return -1;
    return 0;
}

/* cat:  concatenate files, version 2 */
int main(int argc, char *argv[])
{
    int fp;
    int fileCopy(int ifp, int ofp);
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1)  /* no args; copy standard input */
        fileCopy(0, 1);
    else
        while (--argc > 0)
            if ((fp = open(*++argv, O_RDONLY)) == -1) {
                fprintf(stderr, "%s: can′t open %s\n", prog, *argv);
                return 1;
            } else {
                if(fileCopy(fp, 1) < 0){
                    fprintf(stderr, "%s: error writing stdout\n", prog);
                    close(fp);
                    return 2;
                }
                close(fp);
            }
    return 0;
}