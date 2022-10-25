/**
 * @file qsh.h
 * @author qufeiyan (2491411913@qq.com)
 * @brief qsh' header file.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __QSH_H_
#define __QSH_H_

#include <stdint.h>
#include "qsh_opt.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NULL 
#define NULL ((void *)0)
#endif

#define QSH_VERSION     "1.0.0"
#define QSH_POWEREDBY   "qufeiyan"
#define QSH_COPYRIGHT   "2022"

/**
 * @defgroup qsh internal
 * @brief qsh command defination 
 * 
 * @{
 */ 
#define QSH_WEAK            __attribute__((weak))
#define QSH_USED            __attribute__ ((__used__))
#define QSH_SECTION(s)      __attribute__((section(s)))

#define DEFINE_QSH_CMD(name, cmd, desc)                                                 \
                const char __qsh_##cmd##_name[] = #name;                                \
                const char __qsh_##cmd##_desc[] = #desc;                                \
                QSH_USED const struct qsh_cmd __qsh_##cmd QSH_SECTION(".qsh.cmd.tab")=  \
                {                                                                       \
                    __qsh_##cmd##_name,                                                 \
                    __qsh_##cmd##_desc,                                                 \
                    (cmd_t )&cmd                                                        \
                };

#define QSH_ASSERT(exp)  do{                                        \
    if(!(exp)) qsh_assert_handler(#exp, __FUNCTION__, __LINE__);    \
}while(0)

#if QSH_ENABLE_VARIABLE_TRACE
#define DEFINE_QSH_VAR(name, desc, read, write)                                         \
                const char __qsh_##cmd##_name[] = #name;                                \
                const char __qsh_##cmd##_desc[] = #desc;                                \
                QSH_USED const struct qsh_var __qsh_##cmd QSH_SECTION(".qsh.var.tab")=  \
                {                                                                       \
                    __qsh_##cmd##_name,                                                 \
                    __qsh_##cmd##_desc,                                                 \
                    {                                                                   \
                        (varcb_t)read,                                                  \
                        (varcb_t)write                                                  \
                    }                                                                   \
                };
#else
#define DEFINE_QSH_VAR(name, desc, read, write)  
#endif

/**
 * @}
 * 
 */


/**
 * @ingroup qsh internal
 * @brief 
 * @{
 */

typedef int (*cmd_t)(int argc, char **argv);

struct qsh_cmd{
    const char* name;
    const char* desc;
    cmd_t func;
};

#if QSH_ENABLE_VARIABLE_TRACE 
typedef int (*varcb_t)(int argc, char **argv);

struct qsh_var{
    const char *name;
    const char* desc;
    struct {
        varcb_t read;
        varcb_t write;
    };
};
#endif

/**
 * @brief qsh defination.
 * 
 */
struct qsh{
    struct qsh_cmd *start, *end;

#if QSH_ENABLE_VARIABLE_TRACE    
    struct qsh_var *first, *last;
#endif

    char line[QSH_MAX_COMMAND_SIZE + 1];    /**< commnad buffer. */
    uint16_t line_pos;                  /**< valid positon of input string. */
    uint16_t line_curpos;               /**< current postion of input string */

    const char *prompt;                 /**< command prompt for input area. */
    int initialized;                    /**< indicates wether qsh is initialized.*/  

#if QSH_ENABLE_HISTORY_COMMAND
    struct{
        #if QSH_ONLY_ENABLE_MINIMAL_HISTORY
        struct qsh_cmd *history[QSH_MAX_HISTORY_SIZE]; /**< store the historical commands. */ 
        #else
        char history[QSH_MAX_HISTORY_SIZE][QSH_MAX_COMMAND_SIZE];
        #endif

        unsigned int head : 8;       /**< head and tail index in the historical commands. */
        unsigned int tail : 8;
        unsigned int previous: 8;    /**< latest index in the historical commands. */
        unsigned int up_peak : 1;    /**< whether the last record in the history has been accessed. */  
        unsigned int down_valley :1; /**< whether the lastest record in the history has been accessed. */  
        unsigned int full: 1;        /**< whether the upper limit of the history has been reached. */
    };
#endif

};

/**
 * @}
 * 
 */


 /**
  * @defgroup port
  * 
  * @{
  */

/**
 * @brief Callback function for character output
 *
 * @param str is the string to output
 */
void qsh_output(const char* str, ...);

int qsh_get_char(void);

/**
 * @brief get character without line buffer, and set echo off.
 * 
 * @return int 
 */
int qsh_getch(void);

int qsh_put_char(int ch);

int qsh_put_str(const char* str);

/**
 * @brief Handler for assertion.
 * 
 * @param exp is the assertion condition string.
 * @param func is the function name when assertion.
 * @param line is the file line number when assertion.
 */
void qsh_assert_handler(const char *exp, const char *func, int line);
/**
 * @}
 *
 */

void qsh_init(struct qsh *shell);
void qsh_symbol_init(struct qsh *shell);
void qsh_entry(void *);
int qsh_exit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif