#include <console/console.h>

/* List containing all the registered consoles. */
static console_t *console_list;

/**
 * Calls the specified console and writes the text of spcified length.
 */
static void console_write(console_t *con, const uint8_t *text, size_t len)
{
    con->write(con, text, len);
}

void register_console(console_t *newcon)
{
    console_t *con;
    for (con = console_list; con != NULL; con = con->next) {
        if (newcon == con) {
            return;
        }
    }

    if (console_list == NULL) {
        newcon->next = console_list;
        console_list = newcon;
    }
    else {
        newcon->next = console_list->next;
        console_list->next = newcon;
    }
}

void unregister_console(console_t *delcon)
{
    console_t *con;

    /* console is at the head of the list */
    if (console_list == delcon) {
        console_list = delcon->next;
    }
    else {
        for (con = console_list; con != NULL; con = con->next) {
            if (con->next == delcon) {
                con->next = delcon->next;
                break;
            }
        }
    }

    if (delcon->exit) {
        delcon->exit(delcon);
    }
}
