#include <stdio.h>
#include <string.h>
//#include "music.h"
#include "common.h"
#include "ui.h"
#include "task.h"
#include "fs.h"

int main()
{
    main_init();
    while(1) run_cmd_interface();
    return 0;
}
