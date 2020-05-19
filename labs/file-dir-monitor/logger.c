#define _XOPEN_SOURCE 500
#include "logger.h"
#include <limits.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int fd;
struct directory{
    int wd;
    char *path;
};
int current_directories;
struct directory **directories;
char *getPwdNoFile(const struct inotify_event *event){
    int i;
    for(i = 0; i < current_directories; i++){
        if(directories[i]->wd == event->wd){
            return directories[i]->path;
        }
    }
    return NULL;
}
char *getPwd(const struct inotify_event *event){
    int i;
    for(i = 0; i < current_directories; i++){
        if(directories[i]->wd == event->wd){
            int dirLen = strlen(directories[i]->path);
            char *path = calloc(dirLen + 1 + event->len, sizeof(char));
            strcpy(path, directories[i]->path);
            path[dirLen] = '/';
            strcpy(path + dirLen + 1, event->name);
            return path;
        }
    }
    return NULL;
}
void rmDir(char *path){
    int i;
    int found = -1;
    for(i = 0; i < current_directories; i++){
        if(found >= 0){
            directories[i-1] = directories [i];
        }else if(strcmp(directories[i]->path, path) == 0){
            free(directories[i]->path);
            free(directories[i]);
            directories[i] = NULL;
            found = i;
            current_directories--;
        }
    }
    if(found >= 0){
        if(found < current_directories){
            directories[current_directories-1] = directories [current_directories];
            directories [current_directories] = NULL;
        }
        if(current_directories/100 > (current_directories-1)/100){
            directories = realloc(directories, sizeof(struct directory *) * (current_directories - 100));
        }
        // warnf("Current dirs watched: %d\n", current_directories);
        // for(i = 0; i < current_directories; i++){
        //     warnf("Dir[%d](wd: %d): %s\n", i, directories[i]->wd, directories[i]->path);
        // }
    }
}
int addDirNotify(const char *path){
    int mask = IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_DELETE_SELF;
    int wd = inotify_add_watch(fd, path, mask);
    if(wd == -1){
        errorf("Error adding %s to inotify\n", path);
        return -1;
    }
    if(current_directories/100 > (current_directories-1)/100){
        directories = realloc(directories, sizeof(struct directory *) * (current_directories + 100));
    }
    directories[current_directories] = malloc(sizeof(struct directory));
    directories[current_directories]->wd = wd;
    directories[current_directories]->path = malloc((1 + strlen(path)) * sizeof(char));
    strcpy(directories[current_directories]->path, path);
    current_directories++;
    return 0;
}

int isDirectory(char *path){
    struct stat buf;
    stat(path, &buf);
    switch (buf.st_mode & S_IFMT) {
    case S_IFDIR:  infof("Directory");               return 1;
    case S_IFSOCK: infof("Socket");                  break;
    case S_IFLNK:  infof("Symlink");                 break;
    case S_IFREG:  infof("Normal file");            break;
    case S_IFIFO:  infof("FIFO/pipe");               break;
    default:       infof("Unknown type");            break;
    }
    return 0;
}

static int treeEntry(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    switch (typeflag) {
    case FTW_D: // Directory
        // infof("Directory: %s\n", fpath);
        return addDirNotify(fpath);
    case FTW_DNR: // Directory that can't be read
        errorf("This program doesn't have permissions to read the directory %s\n", fpath);
        return -1;
    case FTW_NS: // Error calling stat(), not symbolic
        warnf("Error calling stat, not a symbolic link\n");
        break;
    }
    return 0;
}

void manageEvent(const struct inotify_event *event){
    if(event->mask & IN_CREATE){
        char *path = getPwd(event);
        int isDir = isDirectory(path);
        infof(" %s created\n", path);
        if(isDir){
            addDirNotify(path);
        }
        free(path);
    }
    else if(event->mask & IN_DELETE){
        char *path = getPwd(event);
        infof("%s deleted\n", path);
        if(event->mask & IN_ISDIR) rmDir(path);
        free(path);
    }
    else if(event->mask & IN_MOVED_FROM){
        char *path = getPwd(event);
        infof("%s moved FROM directory %s\n", event->name, getPwdNoFile(event));
        if(event->mask & IN_ISDIR) rmDir(path);
        free(path);
    }
    else if(event->mask & IN_MOVED_TO){
        char *path = getPwd(event);
        int isDir = isDirectory(path);
        infof(" %s moved TO directory %s\n", event->name, getPwdNoFile(event));
        if(isDir){
            addDirNotify(path);
        }
        free(path);
    }
    infof("0x%08x\n", event->mask);  // gives 0x00000007
}

int main(int argc, char *argv[]){
    // Checking parameters
    if (argc != 2){
        errorf("Wrong parameters, usage: %s <directoryPath>\n", argv[0]);
        return -1;
    }
    // NFTW
    fd = inotify_init();
    directories = calloc(100, sizeof(struct directory *));
    if (fd == -1){
        errorf("Error calling inotify_init\n");
        return -1;
    }
    int flags = 0;
    flags |= FTW_PHYS;
    int err = nftw(argv[1], treeEntry, 20, flags);
    if(err){
        errorf("Error calling nftw\n");
    }
    warnf("Current dirs watched: %d\n", current_directories);
    // int i;
    // for(i = 0; i < current_directories; i++){
    //     warnf("Dir[%d](wd: %d): %s\n", i, directories[i]->wd, directories[i]->path);
    // }
    warnf("Configuration is done, listening to changes:\n");
    // Listening to changes
    int n = 0;
    int structSize = sizeof(struct inotify_event);
    int bufSize = structSize + NAME_MAX + 1;
    char *buf = calloc(bufSize, sizeof(char));
    const struct inotify_event *event;
    while (1){
        n = read(fd, buf, bufSize);
        event = (const struct inotify_event *)buf;
        lseek(fd, structSize + event->len - bufSize, SEEK_CUR);
        manageEvent(event);
    }
    return 0;
}