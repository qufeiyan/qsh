/**
 * @file qsh.c
 * @author qufeiyan (2491411913@qq.com)
 * @brief command line parser on 'qsh' is implemented in this file.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "qsh.h"
#include "qsh_opt.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qsh_var.h"

#define EOL "\n"

const char *qsh_logo = {
#if !QSH_LOGO_SELECT_MINIMAL
    "         _            _            _       _"EOL  
    "        /\\ \\         / /\\         / /\\    / /\\"EOL
    "       /  \\ \\       / /  \\       / / /   / / /"EOL
    "      / /\\ \\ \\     / / /\\ \\__   / /_/   / / /"EOL
    "     / / /\\ \\ \\   / / /\\ \\___\\ / /\\ \\__/ / /"EOL
    "    / / /  \\ \\_\\  \\ \\ \\ \\/___// /\\ \\___\\/ /"EOL
    "   / / / _ / / /   \\ \\ \\     / / /\\/___/ /"EOL
    "  / / / /\\ \\/ / _   \\ \\ \\   / / /   / / /"EOL    
    " / / /__\\ \\ \\//_/\\__/ / /  / / /   / / /"EOL    
    "/ / /____\\ \\ \\\\ \\/___/ /  / / /   / / /"EOL       
    "\\/________\\_\\/ \\_____\\/   \\/_/    \\/_/"EOL       
#else
"           /"EOL   
" ___  ___ (___  Version    " QSH_VERSION EOL
"|   )|___ |   ) Copyright  " "(c) " QSH_COPYRIGHT EOL
"|__/| __/ |  /  Poweredby  " QSH_POWEREDBY EOL 
"    |           Build      " __DATE__" "__TIME__ EOL 
#endif
};

#ifndef container_of
#define container_of(ptr, type, member)   \
            ((type *)((char *)ptr - (size_t)(&((type *)0)->member))) 
#endif

#define QSH_CLEAR_COMMAND_LINE(shell)    do{                \
    if(shell->line_pos != 0){                               \
        memset(shell->line, 0, shell->line_pos);            \
        shell->line_pos = shell->line_curpos = 0;           \
        qsh_output("\033[2K\r");                            \
        qsh_output("%s%s", shell->prompt, shell->line);     \
    }                                                       \
}while(0)

typedef struct qsh_cmd qsh_cmd_t;

typedef struct qsh qsh_t;

qsh_t qshell;
DEFINE_QSH_VAR(qshell, qshell, qsh_var_read_qshell, NULL);

#if QSH_ENABLE_VARIABLE_TRACE
typedef struct qsh_var qsh_var_t;

qsh_var_t *qsh_fetch_var(qsh_t *sh, const char *var, size_t size){
    QSH_ASSERT(sh && var && size > 0);
    
    qsh_var_t *target = NULL;
    qsh_var_t *index; 

    for (index = sh->first; index < sh->last; ++index) {
        if (index->name[size] == 0 && 
            strncmp(index->name, var, size) == 0) {
            target = index;
            break;
        }
    }
    return target;
}

#endif

#if QSH_ENABLE_HISTORY_COMMAND

#define QSH_HISTORY_NULL(shell)  (shell->head == shell->tail && shell->full == 0) 
#define QSH_HISTORY_VALID(shell) ((shell->previous >= shell->tail && shell->previous < shell->head) \
                        || (shell->full))
#define QSH_HISTORY_COUNT(shell) (shell->full ? QSH_MAX_HISTORY_SIZE : (shell->head - shell->tail))
#define QSH_UP(shell)    do{                                          \
    shell->up_peak = (shell->previous == shell->tail ? 1 : 0);        \
    int psudo = (shell->previous - 1 >= 0) ?                          \
        (shell->previous - 1) : (QSH_MAX_HISTORY_SIZE - 1);               \
    shell->previous = ((psudo >= shell->tail && psudo < shell->head)  \
        || shell->full) ? psudo : shell->tail;                        \
}while(0)  

#define QSH_DOWN(shell)  do{                                          \
    shell->down_valley = (shell->previous ==  \
        (shell->head - 1) ? 1 : 0);  \
    int psudo = ((shell->previous + 1) & (QSH_MAX_HISTORY_SIZE - 1));     \
    shell->previous = ((psudo >= shell->tail && psudo < shell->head)  \
        || shell->full) ? psudo : shell->head - 1;                    \
}while(0)

#if QSH_ONLY_ENABLE_MINIMAL_HISTORY
/**
 * @brief Push current command to historical list.
 * 
 * @param qsh_cmd 
 * @return int 
 */
static int qsh_push_history(qsh_cmd_t* qsh_cmd){
    qsh_t *shell = &qshell;
    QSH_ASSERT(shell->initialized);
      
    if(shell->full == 1){
        if(shell->history[shell->head - 1] != qsh_cmd){
            shell->history[shell->head] = qsh_cmd;
            shell->head = ((shell->head + 1) & (QSH_MAX_HISTORY_SIZE - 1));
            shell->tail = ((shell->tail + 1) & (QSH_MAX_HISTORY_SIZE - 1)); //! discard the oldest command.
        }
    }else {
        if(shell->history[shell->head - 1] != qsh_cmd){
            shell->history[shell->head] = qsh_cmd;
            shell->head = ((shell->head + 1) & (QSH_MAX_HISTORY_SIZE - 1));
        }
        if(shell->head == shell->tail) shell->full = 1;
    }
    
    shell->previous = (shell->head - 1 >= 0) 
        ? (shell->head - 1) : (shell->head - 1 + QSH_MAX_HISTORY_SIZE);
    return 0;
}
#else
/**
 * @brief 
 * 
 * @return int 
 */
static int qsh_push_history(void){
    qsh_t *shell = &qshell;
    QSH_ASSERT(shell->initialized);
      
    if(shell->full == 1){
        if(memcmp(&shell->history[shell->head - 1], shell->line, QSH_MAX_COMMAND_SIZE)){
            memset(&shell->history[shell->head], 0, QSH_MAX_COMMAND_SIZE);
            memcpy(&shell->history[shell->head], shell->line, shell->line_pos);
            shell->head = ((shell->head + 1) & (QSH_MAX_HISTORY_SIZE - 1));
            shell->tail = ((shell->tail + 1) & (QSH_MAX_HISTORY_SIZE - 1)); //! discard the oldest command.
        }
    }else {
        if(memcmp(&shell->history[shell->head - 1], shell->line, QSH_MAX_COMMAND_SIZE)){
            memset(&shell->history[shell->head], 0, QSH_MAX_COMMAND_SIZE);
            memcpy(&shell->history[shell->head], shell->line, shell->line_pos);
            shell->head = ((shell->head + 1) & (QSH_MAX_HISTORY_SIZE - 1));
        }
        if(shell->head == shell->tail) shell->full = 1;
    }
    
    shell->previous = (shell->head - 1 >= 0) 
        ? (shell->head - 1) : (shell->head - 1 + QSH_MAX_HISTORY_SIZE);
    return 0;
}
#endif
#endif

/**
 * @brief find the target cmd in all of the commands.
 * 
 * @param sh is qsh instance.
 * @param cmd is name of the target command.
 * @param size is size of the cmd.
 * @return the target command method pointer to execute.
 */
static cmd_t *qsh_find(qsh_t *sh, const char *cmd, size_t size){
    if(!sh || !size) return NULL;

    cmd_t *target = NULL;
    qsh_cmd_t *index; 

    for (index = sh->start; index < sh->end; ++index) {
        if (index->name[size] == 0 && 
            strncmp(index->name, cmd, size) == 0) {
            target = &index->func;
        }
    }
    return target;
}

/**
 * @brief parse the command string.
 * 
 * @param cmd is the string to parse.
 * @param argv is the result string array after parsing.
 * @return the number of arguments include command name.
 */
static int qsh_parse(char *cmd, char *argv[QSH_MAX_COMMAND_ARGS]){
    QSH_ASSERT(cmd && argv);

    int argc = 0;
    char *ptr = NULL;
    ptr = strtok((char *)cmd, " ");
    while (ptr != NULL && argc < QSH_MAX_COMMAND_ARGS) {
        argv[argc++] = ptr;
        ptr = strtok(NULL, " ");
        if(ptr) ptr[-1] = '\0';
    }
    return argc;
}


/**
 * @brief execute command method in qsh.
 * 
 * @param cmd is the target command to execute
 * @param length is length of the target command.
 * @return result of executed command.
 */
static int qsh_exec(char *cmd, int length){
    QSH_ASSERT(cmd && length > 0);

    int ret;
    char *argv[QSH_MAX_COMMAND_ARGS];
    int argc;
    cmd_t *command;
    /* strim the beginning of command */
    while ((length > 0) && (*cmd  == ' ' || *cmd == '\t')){
        cmd++;
        length--;
    }

    if (length == 0) return 0;

    argc = qsh_parse(cmd, argv);
    if (argc == 0) return -1;

    /* exec this command */
    command = qsh_find(&qshell, argv[0], strlen(argv[0]));

    if(command == NULL) {
        qsh_output("%s: command not found.\n", argv[0]);
        return -1;
    }

    ret = (*command)(argc, argv);

#if QSH_ENABLE_HISTORY_COMMAND
    #if QSH_ONLY_ENABLE_MINIMAL_HISTORY
    qsh_push_history(container_of(command, qsh_cmd_t, func));
    #else
    qsh_push_history();
    #endif
#endif
    return ret;
}

void qsh_symbol_init(struct qsh *shell){
    QSH_ASSERT(shell);
    extern unsigned long __QSH_SHELL_CMD_START;
    extern unsigned long __QSH_SHELL_CMD_END;
    shell->start = (qsh_cmd_t *)&__QSH_SHELL_CMD_START;
    shell->end = (qsh_cmd_t *)&__QSH_SHELL_CMD_END;

#if QSH_ENABLE_VARIABLE_TRACE
    extern unsigned long __QSH_SHELL_VAR_START;
    extern unsigned long __QSH_SHELL_VAR_END;
    shell->first = (qsh_var_t *)&__QSH_SHELL_VAR_START;
    shell->last = (qsh_var_t *)&__QSH_SHELL_VAR_END;
#endif
}

void qsh_init(struct qsh *shell){
    QSH_ASSERT(shell);

    qsh_symbol_init(shell);
    shell->line_pos = shell->line_curpos = 0;
    shell->prompt = "-> $"; 
    shell->initialized = 1;
}

static void qsh_handle_direction(int ch){
    qsh_t *shell = &qshell;
    switch (ch) {        
        case 0x41:{
#if QSH_ENABLE_HISTORY_COMMAND
            if(QSH_HISTORY_NULL(shell)) return; 
            if(shell->up_peak){
                // qsh_output("%d\n", shell->previous);
                QSH_CLEAR_COMMAND_LINE(shell);
                shell->up_peak = !shell->up_peak;
                return;
            } 
        #if QSH_ONLY_ENABLE_MINIMAL_HISTORY
            memcpy(shell->line, shell->history[shell->previous]->name, 
                strlen(shell->history[shell->previous]->name) + 1);
        #else
            memcpy(shell->line, shell->history[shell->previous], QSH_MAX_COMMAND_SIZE);
        #endif
            shell->line_curpos = shell->line_pos = strlen(shell->line);
            qsh_output("\033[2K\r");
            qsh_output("%s%s", shell->prompt, shell->line);
            QSH_UP(shell);
#endif            
            break;
        }
        case 0x42:{
            if(QSH_HISTORY_NULL(shell)) return; 
            if(shell->down_valley){
                // qsh_output("%d\n", shell->previous);
                QSH_CLEAR_COMMAND_LINE(shell);
                shell->down_valley = !shell->down_valley;
                return;
            }
#if QSH_ENABLE_HISTORY_COMMAND
        #if QSH_ONLY_ENABLE_MINIMAL_HISTORY
            memcpy(shell->line, shell->history[shell->previous]->name, 
                strlen(shell->history[shell->previous]->name) + 1);
        #else
            memcpy(shell->line, shell->history[shell->previous], QSH_MAX_COMMAND_SIZE);
        #endif
            shell->line_curpos = shell->line_pos = strlen(shell->line);
            qsh_output("\033[2K\r");
            qsh_output("%s%s", shell->prompt, shell->line);
            QSH_DOWN(shell);
#endif            
            break;
        }
        case 0x43:{
            if(shell->line_curpos < shell->line_pos){
                qsh_put_char(shell->line[shell->line_curpos]);
                shell->line_curpos++;
            }
            break;
        }
        case 0x44:{
            if(shell->line_curpos > 0){
                qsh_put_char('\b');
                shell->line_curpos--;
            }
            break;
        }
        default:
            break;
    }
}

static void qsh_handle_back(void){
    qsh_t *shell = &qshell;
    /* note that shell->line_curpos >= 0 */
    if (shell->line_curpos == 0) return;

    shell->line_pos--;
    shell->line_curpos--;

    if (shell->line_pos > shell->line_curpos){
        int i;

        memmove(&shell->line[shell->line_curpos],
                    &shell->line[shell->line_curpos + 1],
                    shell->line_pos - shell->line_curpos);
        shell->line[shell->line_pos] = 0;

        qsh_output("\b%s  \b", &shell->line[shell->line_curpos]);

        /* move the cursor to the origin position */
        for (i = shell->line_curpos; i <= shell->line_pos; i++)
            qsh_output("\b");
    }else{
        qsh_output("\b \b");
        shell->line[shell->line_pos] = 0;
    }
}

static int qsh_str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2))
    {
        str ++;
        str2 ++;
    }

    return (str - str1);
}

static void qsh_handle_tab(char *prefix){
    qsh_t *shell = &qshell;

    cmd_t *target = NULL;
    qsh_cmd_t *index; 
    int len = 0;
    const char *pre = NULL;
    int i;

    qsh_put_char('\n');    
    if(prefix[0] == '\0'){
        qsh_output("--Qshell commands list:\n");
        qsh_output("-----------------------------------------------------\n");
        for (index = shell->start; index < shell->end; index++){ 
            qsh_output("%-16s - %s\n", index->name, index->desc);
        }
        qsh_output("%s%s", shell->prompt, prefix);
        return;
    }

    /* move the cursor to the beginning of line */
    for (i = 0; i < shell->line_curpos; i++)
        qsh_put_char('\b');

    for (index = shell->start; index < shell->end; ++index) {
        if (strncmp(index->name, prefix, shell->line_pos) == 0) {
            if (len == 0){
                pre = index->name;
                len = strlen(pre);
            }
            int comlen = qsh_str_common(pre, index->name);
            if(len > comlen) len = comlen;
            qsh_output("%-12s ", index->name);
        }
    }

    /* auto complete string */
    if (pre != NULL){
        qsh_put_char('\n');
        strncpy(prefix, pre, len);
    }
    qsh_output("%s%s", shell->prompt, prefix);
    /** 1. update new state when complete string.
     *  2. restore previous state when prefix string match nothing. */
    shell->line_curpos = shell->line_pos = strlen(prefix);
}

void qsh_entry(void *args){
    int ch;
    qsh_t *shell = &qshell;
    qsh_init(shell);

#if QSH_LOGO_SELECT_MINIMAL
    qsh_output("%s\n", qsh_logo);
#endif
    qsh_output(shell->prompt);

    enum key_stat{
        key_direction_butt,
        key_direction_wait,
        key_direction_func
    }stat = key_direction_butt;
    int ret;

    while(1){
        ch = qsh_getch();
        if (ch < 0){
            continue;
        }

        /* handle direciton key. */
        /**
         * @brief up:    0x1b 0x5b 0x41
         *        down:  0x1b 0x5b 0x42
         *        left:  0x1b 0x5b 0x44
         *        right: 0x1b 0x5b 0x43
         */
        if(ch == 0x1b){
            stat = key_direction_wait;
            continue;
        }else if(stat == key_direction_wait){
            if(ch == 0x5b){
                stat = key_direction_func;
                continue;
            }else{
                stat = key_direction_butt;
            }
        }else if(stat == key_direction_func){
            stat = key_direction_butt;
            qsh_handle_direction(ch);
            continue;
        }
        
        /* ignore null or invalid character */
        if (ch == '\0' || ch == 0xFF) continue;

        if (ch == '\t'){
            qsh_handle_tab(shell->line);
            continue;
        }

        if (ch == 0x7f || ch == 0x08){
            qsh_handle_back();
            continue;
        }

        if (ch == '\r' || ch == '\n'){
            qsh_put_char('\n');
            if(shell->line_pos != 0){
                qsh_exec(shell->line, shell->line_pos);
            }
            
            qsh_output(shell->prompt);
            shell->line_pos = shell->line_curpos = 0;
            memset(shell->line, 0, sizeof(shell->line));
            continue;
        }

        /** insert the character.*/
        if(shell->line_curpos < shell->line_pos){
            memmove(shell->line + shell->line_curpos + 1, 
                shell->line + shell->line_curpos, 
                shell->line_pos - shell->line_curpos);
            shell->line[shell->line_curpos] = ch;
            /** echo the new input, this operation will move cursor to the last.*/
            qsh_output("%s", shell->line + shell->line_curpos);
            /** restore the cursor. */
            int i;
            for (i = shell->line_curpos; i < shell->line_pos; ++i) {
                qsh_put_char('\b');
            }
        }else {
            shell->line[shell->line_pos] = ch;
            qsh_put_char(ch);
        }   

        ch = 0;     
        shell->line_curpos++;
        shell->line_pos++;
    }
}


