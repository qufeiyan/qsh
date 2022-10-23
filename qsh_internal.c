/**
 * @file qsh_internal.c
 * @author qufeiyan (2491411913@qq.com)
 * @brief  qsh's internal command implemented in this file.
 * @version 0.1
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "qsh.h"
#include "qsh_opt.h"
#include <string.h>
#include <unistd.h>

/**
 * @brief internal command.
 *
 * @defgroup internal 
 * @{
 */

/**
 * @brief clear the display area.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
static int qsh_clear(int argc, char **argv){
    qsh_output("\033c");
    return 0;
}
DEFINE_QSH_CMD(clr, qsh_clear, clear display area);

/**
 * @brief get qsh's info.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
static int qsh_info(int argc, char **argv)
{  
    extern const char *qsh_logo;
    #if !QSH_LOGO_SELECT_MINIMAL    
    #define QSH_MAX_LOGO_NEED_BUFFER  (256 * 2) 
    if(QSH_OUTPUT_BUFFER < QSH_MAX_LOGO_NEED_BUFFER){
        qsh_output("qsh output buffer will be overflow..., logo size: %d\n",
            strlen(qsh_logo));
    }
    qsh_output("%s", qsh_logo);
    qsh_output("\r\n%-10s %s %s\r\n","Build" ,__DATE__,  __TIME__);
    qsh_output("%-10s %s\r\n","Version", QSH_VERSION);
    qsh_output("%-10s %s\r\n", "Poweredby", QSH_POWEREDBY);
    qsh_output("%-10s %s\r\n", "Copyright  (c)", QSH_COPYRIGHT);
    #else
    qsh_output("%s\n", qsh_logo);
    #endif
    return 0;
}
DEFINE_QSH_CMD(info, qsh_info, display current version of qsh);

/**
 * @brief list all of the internal command.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
static int qsh_list(int argc, char **argv){
    extern struct qsh qshell;
    struct qsh *shell = &qshell;

    QSH_ASSERT(shell->initialized);

    struct qsh_cmd *index;
    qsh_output("--Qshell commands list:\n");
    qsh_output("-----------------------------------------------------\n");
    for (index = shell->start; index < shell->end; index++) {
        qsh_output("%-8s\t- %s\n", index->name, index->desc);
    }
    return 0;
}
DEFINE_QSH_CMD(list, qsh_list, list all internal commands);

/**
 * @brief quit qsh.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
static int qsh_quit(int argc, char **argv){
    qsh_exit();
    return 0;
}
DEFINE_QSH_CMD(quit, qsh_quit, quit qsh from current contex);


#ifdef ENABLE_HISTORY_COMMAND
/**
 * @brief display the historical commands.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
static int qsh_history(int argc, char **argv){
    extern struct qsh qshell;
    struct qsh *shell = &qshell;
    
    int size = shell->full ? MAX_HISTORY_SIZE : shell->head - shell->tail;
    qsh_output("historical commands :%d\n", size);
    if(size == 0) return 0;

    qsh_output("head: %d, tail: %d, full: %u, up_peak: %d, down_valley: %d\n", 
        shell->head, shell->tail, shell->full, shell->up_peak, shell->down_valley);

    int i, count = 0;
    if(shell->head > shell->tail){
        for(i = shell->tail; i < shell->head; ++i){
            #if ONLY_ENABLE_MINIMAL_HISTORY
            qsh_output("%d\t %s\n", ++count, shell->history[i]->name);
            #else
            qsh_output("%d\t %s\n", ++count, shell->history[i]);
            #endif
        }
    }else{
        for(i = shell->tail; i < MAX_HISTORY_SIZE; ++i){
           #if ONLY_ENABLE_MINIMAL_HISTORY
            qsh_output("%d\t %s\n", ++count, shell->history[i]->name);
            #else
            qsh_output("%d\t %s\n", ++count, shell->history[i]);
            #endif
        }

        for(i = 0; i < shell->head; ++i){
            #if ONLY_ENABLE_MINIMAL_HISTORY
            qsh_output("%d\t %s\n", ++count, shell->history[i]->name);
            #else
            qsh_output("%d\t %s\n", ++count, shell->history[i]);
            #endif
        }
    }

    return 0;
}
DEFINE_QSH_CMD(history, qsh_history, Display the commands history list);
#endif

static int qsh_set(int argc, char **argv){
    if(argc <= 1){
        qsh_output("Usage: set [VARIABLE] [VALUE]...\n");
    }


    return 0;
}
DEFINE_QSH_CMD(set, qsh_set, Set value for a variable);

static int qsh_get(int argc, char **argv){
    if(argc <= 1){
        qsh_output("Usage: get [VARIABLE]...\n");
    }

    return 0;
}
DEFINE_QSH_CMD(get, qsh_get, get value for a variable);

/**
 * @}
 * 
 */

