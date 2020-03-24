include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* filecopy:  copy file ifp to file ofp */
void filecopy(FILE *ifp, FILE *ofp)
int filecopy(int ifp, int ofp)
{
    int c;

    while ((c = getc(ifp)) != EOF)
        putc(c, ofp);

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
    FILE *fp;
    void filecopy(FILE *, FILE *);
    int fp;
    int filecopy(int ifp, int ofp);
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1)  /* no args; copy standard input */
        filecopy(stdin, stdout);
        filecopy(0, 1);
    else
        while (--argc > 0)
            if ((fp = fopen(*++argv, "r")) == NULL) {
                fprintf(stderr, "%s: can′t open %s\n",
			prog, *argv);
            if ((fp = open(*++argv, O_RDONLY)) == -1) {
                fprintf(stderr, "%s: can′t open %s\n", prog, *argv);
                return 1;
            } else {
                filecopy(fp, stdout);
                fclose(fp);
                if(filecopy(fp, 1) < 0){
                    fprintf(stderr, "%s: error writing stdout\n", prog);
                    close(fp);
                    return 2;
                }
                close(fp);
            }

    if (ferror(stdout)) {
        fprintf(stderr, "%s: error writing stdout\n", prog);
        return 2;
    }

    return 0;
}