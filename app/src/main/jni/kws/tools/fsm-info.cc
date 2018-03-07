#include <stdio.h>

#include "fsm.h"

int main(int argc, char *argv[]) {
    const char *usage = "Showing details text information of fsm format file\n" 
                        "Usage: fsm-info fsm_file\n"
                        "eg: fsm-info in.fsm\n";

    if (argc != 2) {
        printf("%s", usage);
        return -1;
    }

    Fsm fsm(argv[1]);
    fsm.Info();
    return 0;
}

