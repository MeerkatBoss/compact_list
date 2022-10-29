#include <stdio.h>

#include "logger.h"
#include "list.h"

int main()
{
    add_logger({
        .name = "Console logger",
        .stream = stdout,
        .logging_level = LOG_ERROR,
        .settings_mask = LGS_KEEP_OPEN | LGS_USE_ESCAPE
    });
    add_logger({
        .name = "HTML logs",
        .stream = fopen("logs.html", "a"),
        .logging_level = LOG_ALL,
        .settings_mask = LGS_USE_HTML
    });

    compact_list list = list_ctor();

    push_back(&list, 10);
    list_iterator second = push_back(&list, 20);
    push_back(&list, 30);
    pop_front(&list);
    insert_after(&list, second, 40);
    list_iterator tail = list_end(&list);
    insert_before(&list, tail, 50);
    list_dump(&list, "dump1.png");
    log_message(MSG_TRACE, "Dumping list: <h2>List not linearized:</h2><br/>"
                "<img src=\"dump1.png\" style=\"width: 130em\"/><hr/>");

    LOG_PRINT_TRACE(linearize(&list), "linearized=%d", list.is_linear);

    list_dump(&list, "dump2.png");
    log_message(MSG_TRACE, "Dumping list: <h2>List linearized:</h2><br/>"
                "<img src=\"dump2.png\" style=\"width: 130em\"/><hr/>");

    list_dtor(&list);

    return 0;
}