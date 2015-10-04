#include <dbus/dbus.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"

/* make sure we have proper permission */
#define CLIENT   "task.method.caller"
#define SERVER   "task.method.server"
#define INTERFCE "task.method.Chat"
#define OBJECT   "/task/method/Task"

void request(char *method_name)
{
    DBusMessage* msg;
    DBusMessageIter args;
    DBusConnection* conn;
    DBusError err;
    DBusPendingCall* pending;

    int ret;
    size_t len;
    char *buffer = NULL;
    char *param  = NULL;

    if (!method_name) return;
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) { 
        log_err("Connection Error (%s)", err.message); 
        dbus_error_free(&err);
    }

    if (NULL == conn) { 
        exit(1); 
    }

    /* Try best to become a primary!*/
    ret = dbus_bus_request_name(conn, CLIENT, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err)) { 
        log_err("Name Error (%s)", err.message); 
        dbus_error_free(&err);
    }

    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
        exit(1);
    }

    printf(">");
    while(getline(&buffer, &len, stdin) != -1) {
        msg = dbus_message_new_method_call(SERVER, OBJECT, 
                INTERFCE, method_name);
        
        if (NULL == msg) { 
            log("Message Null");
            exit(1);
        }

        dbus_message_iter_init_append(msg, &args);
        if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &buffer)){
            log_err("Out Of Memory!"); 
            exit(1);
        }

        if (!dbus_connection_send_with_reply (conn, msg, &pending, -1)) {
            log_err("Out Of Memory!"); 
            exit(1);
        }

        if (NULL == pending) { 
            log_err("Pending Call Null"); 
            exit(1); 
        }

        dbus_connection_flush(conn);
        log_dbg("Request sent to server");
        dbus_message_unref(msg);
        /* Warn! its a blocking call */
        dbus_pending_call_block(pending);
        /* time to retrieve the reply back */
        msg = dbus_pending_call_steal_reply(pending);
        if (NULL == msg) {
            log_err("Reply Null\n"); 
            exit(1); 
        }
        
        dbus_pending_call_unref(pending);
        if (!dbus_message_iter_init(msg, &args))
            log_err("Message has no arguments!"); 
        else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
            log_err("Argument is not String!"); 
        else
            dbus_message_iter_get_basic(&args, &param);

        log("Got Reply: %s", param);
        dbus_message_unref(msg);
        printf(">");
    }
}

void reply_to_method_call(DBusMessage* msg, DBusConnection* conn)
{
    DBusMessage* reply;
    DBusMessageIter args;
    dbus_uint32_t serial = 0;
    char* param = "";

    if (!dbus_message_iter_init(msg, &args))
        log_err("Message has no arguments!"); 
    else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) 
        log_err("Argument is not string!"); 
    else 
        dbus_message_iter_get_basic(&args, &param);

    log("Method called with %s", param);
    reply = dbus_message_new_method_return(msg);
    dbus_message_iter_init_append(reply, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &param)) { 
        log_err("Out Of Memory!"); 
        exit(1);
    }

    if (!dbus_connection_send(conn, reply, &serial)) {
        log_err("Out Of Memory!"); /* The only reason to Fail */
        exit(1);
    }

    dbus_connection_flush(conn);
    dbus_message_unref(reply);
}

void serve(char *method_name) 
{
    DBusMessage* msg;
    DBusConnection* conn;
    DBusError err;
    int ret;

    if (!method_name) return; 
    log("Listening for method calls [%s]", method_name);
    dbus_error_init(&err);
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if (dbus_error_is_set(&err)) { 
        log("Connection Error (%s)", err.message); 
        dbus_error_free(&err); 
    }

    if (NULL == conn) {
        log_err("Connection Null"); 
        exit(1); 
    }

    ret = dbus_bus_request_name(conn, SERVER, DBUS_NAME_FLAG_REPLACE_EXISTING , &err);
    if (dbus_error_is_set(&err)) { 
        log_err("Name Error (%s)", err.message); 
        dbus_error_free(&err);
    }

    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) { 
        log_err("Not Primary Owner (%d)", ret);
        exit(1); 
    }

    while (true) {
        dbus_connection_read_write(conn, 0);
        msg = dbus_connection_pop_message(conn);

        if (NULL == msg) { 
            sleep(1); 
            continue; 
        }

        log_dbg("Received\n");
        if (dbus_message_is_method_call(msg, INTERFCE, method_name)) 
            reply_to_method_call(msg, conn);

        dbus_message_unref(msg);
    }

}

int main(int argc, char** argv)
{
    if (2 > argc) {
        printf ("Syntax: %s [serve|request]\n", argv[0]);
        return 1;
    }

    if (0 == strcmp(argv[1], "serve"))
        serve("Echo");
    else if (0 == strcmp(argv[1], "request"))
        request("Echo");
    else {
        printf ("Syntax: %s [serve|request]\n", argv[0]);
        return 1;
    }

    return 0;
}
