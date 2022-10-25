/**
 * @file qsh_var.c
 * @author qufeiyan (2491411913@qq.com)
 * @brief This file defines the read/write interface for all variables to be traced.
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "qsh_var.h"
#include "qsh.h"

#if QSH_ENABLE_VARIABLE_TRACE

#define EOL "\n"
#define HOL "    "   //! indented characters. 


/**
 * @brief Read method of the variable "qshell" 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int qsh_var_read_qshell(int argc, char **argv){
    extern struct qsh qshell;
    struct qsh *shell = &qshell;
    const char *fmt = "%s = { "EOL
        HOL"start = %p, end = %p"EOL
        HOL"first = %p, last = %p"EOL
        HOL"line = %s, pos = %d, cur_pos = %d"EOL
        HOL"prompt = %s"EOL
#if QSH_ENABLE_HISTORY_COMMAND
        HOL"{"EOL
        HOL HOL"head = %d, tail = %d, full = %d"EOL
        HOL HOL"previous = %d, up_peak = %d, down_valley = %d"EOL 
        HOL"}"EOL
#endif
        "}"EOL;
    qsh_output(fmt, argv[0], shell->start, shell->end, shell->first, shell->last,
        shell->line, shell->line_pos, shell->line_curpos, shell->prompt
#if QSH_ENABLE_HISTORY_COMMAND
        , shell->head, shell->tail, shell->full, shell->previous, shell->up_peak,
        shell->down_valley
#endif        
    );
    return 0;
}

int qsh_var_write_qshell(int argc, char **argv){    
    return 0;
}
/** ---------------------------End of "qshell"-------------------------------*/

#endif //! end of the macro "QSH_ENABLE_VARIABLE_TRACE"

