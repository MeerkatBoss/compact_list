#include <stdio.h>

#include "logger.h"
#include "list.h"

int main()
{
    add_default_file_logger();
    add_logger({
        .name = "Console logger",
        .stream = stdout,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_KEEP_OPEN | LGS_USE_ESCAPE
    });

    compact_list list = list_ctor();

    push_back(&list, 10);
    list_iterator second = push_back(&list, 20);
    push_back(&list, 30);
    pop_front(&list);
    insert_after(&list, second, 40);
    list_dump(&list, "dump1.png");
    linearize(&list);
    list_dump(&list, "dump2.png");

    list_dtor(&list);

    return 0;
}