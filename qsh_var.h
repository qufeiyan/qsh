/**
 * @file qsh_var.h
 * @author qufeiyan (2491411913@domain.com)
 * @brief This file declares the read/write interface for all variables to be traced.
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __QSH_VAR_H__
#define __QSH_VAR_H__

#include "qsh_opt.h"

#if QSH_ENABLE_VARIABLE_TRACE

/**
 * @brief Read/write interface of the variable "qshell".  
 * 
 * @see qshell
 */
int qsh_var_read_qshell(int argc, char **argv);
int qsh_var_write_qshell(int argc, char **argv);

/**------------------Enf of the "qshell"------------------*/


#endif //! end of the macro "QSH_ENABLE_VARIABLE_TRACE"
#endif //! end of the macro "__QSH_VAR_H__"