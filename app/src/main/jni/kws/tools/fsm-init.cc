#include <stdio.h>

#include <string>

#include "fsm.h"

int main(int argc, char *argv[]) {
    const char *usage = "Init fsm from topo file\n"
                        "Usage: fsm-init topo_file out_file\n"
                        "eg: fsm-copy topo_file out.fsm\n";
    if (argc != 3) {
        printf("%s", usage);
        return -1;
    }

    Fsm fsm;
    fsm.ReadTopo(argv[1]);
    fsm.Write(argv[2]);
    return 0;
}
