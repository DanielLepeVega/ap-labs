#include "logger.h"

int main() {
	infof("INFO Message %d\n", 1);
    warnf("WARN Message %d\n", 2);
    errorf("ERROR Message %d\n", 3);
    panicf("PANIC Message %d\n", 4);
    return 0;
}