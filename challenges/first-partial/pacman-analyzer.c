#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "string.h"
#define REPORT_FILE "packages_report.txt"

void analizeLog(char *logFile, char *report);
void wrReport(int wr);

struct prog {
    char* nameOfF;
    char* insDate;
    char* updDate;
    int numUpd;
    char* rmDate;
};

struct prog progs[600];
int nIns, nUpgr, nCurr;

char* readLine(int rd){ //read all the line of the file
    int size = 50;
    int total = 0;
    char *buffer;
    buffer = calloc(size, sizeof(char));
    int n;
    // Read the files in buffersized waves 
    while((n = read(rd, buffer+total, size)) > 0){
        for(int i=total;i<total+n;i++){
            if(buffer[i] == '\n'){
                // Roll back the offset to after \n
                lseek(rd, i-(total+n)+1, SEEK_CUR);
                buffer[i] = '\0';
                return buffer;
            }
        }
        total+=n;
        buffer = realloc(buffer, total+size);
    }
    if(total>0){
        // Can't have the EOF, so go back 
        buffer[total] = '\0';
        return buffer;
    }
    return NULL;
}

char* getWord(char *str){ //get all the separated words
    int size;
    for(int i=0;i<strlen(str);i++){
        if(str[i] == ' ' || str[i] == '\n'){
            size = i;
            break;
        }
    }
    char *newStringW = calloc(size+1, sizeof(char));
    for(int i=0; i<size; i++){
        newStringW[i] = str[i];
    }
    newStringW[size] = '\0';
    return newStringW;
}

char* getDateOfProg(char *str){ //get all the dates
    int size;
    for(int i=0;i<strlen(str);i++){
        if(str[i] == ']'){
            size = i;
            break;
        }
    }
    char *newStringW = calloc(size+1, sizeof(char));
    for(int i=0; i<size; i++){
        newStringW[i] = str[i];
    }
    newStringW[size] = '\0';
    return newStringW;
}

void newProgs(char* line){ //create objects init
    progs[nIns].nameOfF = getWord(line+36);
    progs[nIns].insDate = getDateOfProg(line+1);
    nIns++;
    nCurr++;
}

void updtProgs(char* line){ //update objects properties
    char *nameOfF = getWord(line+35);
    for(int i=0;i<nIns;i++){
        if(strcmp(progs[i].nameOfF, nameOfF) == 0){
            progs[i].updDate = getDateOfProg(line+1);
            progs[i].numUpd++;
            nUpgr++;
            free(nameOfF);
            return;
        }
    }
    printf("Update prog: prog not found\n");
}

void rmProgs(char* line){//remove objects
    char *nameOfF = getWord(line+34);
    for(int i=0;i<nIns;i++){
        if(strcmp(progs[i].nameOfF, nameOfF) == 0){
            progs[i].rmDate = getDateOfProg(line+1);
            nCurr--;
            free(nameOfF);
            return;
        }
    }
    printf("Remove prog: prog not found\n");
}

void wrNum(int wr, int value){//write numbers
    char buffer[10];
    int size = sprintf(buffer, "%d\n", value);
    write(wr, buffer, size);
}

void wrProgInfo(int wr, struct prog p){//write the information of the program with the given format
    write(wr, "- Package Name        : ", 24);
    write(wr, p.nameOfF, strlen(p.nameOfF));
    write(wr, "\n  - Install date      : ", 25);
    write(wr, p.insDate, strlen(p.insDate));
    write(wr, "\n  - Last update date  : ", 25);
    if(p.updDate != NULL){
        write(wr, p.updDate, strlen(p.updDate));
    } else{
        write(wr, "-", 1);
    }
    write(wr, "\n  - How many updates  : ", 25);
    wrNum(wr, p.numUpd);
    write(wr, "  - Removal date      : ", 24);
    if(p.rmDate != NULL){
        write(wr, p.rmDate, strlen(p.rmDate));
    } else{
        write(wr, "-", 1);
    }
    write(wr, "\n", 1);
}

void wrReport(int wr) {//write the report with the given format
    write(wr, "Pacman Packages Report\n", 23);
    write(wr, "----------------------\n", 23);    
    write(wr, "- Installed packages : ", 23);
    wrNum(wr, nIns);
    write(wr, "- Removed packages   : ", 23);
    wrNum(wr, nIns - nCurr);
    write(wr, "- Upgraded packages  : ", 23);
    wrNum(wr, nUpgr);
    write(wr, "- Current installed  : ", 23);
    wrNum(wr, nCurr);

    write(wr, "\nList of packages\n----------------\n", 35);
    for(int i=0;i<nIns;i++){
        wrProgInfo(wr, progs[i]);
    }
    close(wr);
}

void analizeLog(char *logFile, char *report) {//my implementation where I use my logic of my functions
    printf("Generating Report from: [%s] log file\n", logFile);
    int rd = open(logFile, O_RDONLY);
    if(rd < 0) {
        printf("Error opening log file\n");
        return;
    }
    int wr = open(report, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(wr < 0) {
        printf("Error opening report file\n");
        return;
    }
    char *line;
    while((line = readLine(rd)) != NULL){
        char *action = getWord(line+26); // Thats where the action word is
        if(strcmp("installed", action) == 0){
            newProgs(line);
        } else if (strcmp("upgraded", action) == 0) {
            updtProgs(line);
        } else if (strcmp("removed", action) == 0) {
            rmProgs(line);
        }
        free(action);
        free(line);
    }

    close(rd);
    wrReport(wr);
    printf("Report is generated at: [%s]\n", report);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:./pacman-analizer.o pacman.log\n");
        return 1;
    }
    analizeLog(argv[1], REPORT_FILE);
    return 0;
}