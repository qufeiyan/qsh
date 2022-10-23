/**
 * @file qsh_port.c
 * @author qufeiyan (2491411913@qq.com)
 * @brief all methods implemented in this file is weak. 
 *         that means, allow users to define their own custom method to adapt
 *         to other embedded systems.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "qsh.h"
#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>


/**
 * @brief Default assert handler.
 * 
 * @param exp is the expression for assertion.
 * @param func is the function name when assertion.
 * @param line is the line number when assertion.
 */
QSH_WEAK void qsh_assert_handler(const char *exp, const char *func, int line){
    qsh_output("(%s) assertion failed at function :%s, line :%d \n", exp, func, line);
    while (1);
}

/**
 * @brief Default output function.
 * @note If user do not overload it, posix fputs in libc will be the default. 
 *
 * @param fmt
 */
QSH_WEAK void qsh_output(const char* fmt, ...){
    va_list args;
    char buffer[QSH_OUTPUT_BUFFER];

    va_start(args, fmt);
    vsnprintf(buffer, QSH_OUTPUT_BUFFER - 1, fmt, args);
    va_end(args);
    
    fputs(buffer, stdout);
}

/**
 * @brief Default method to get a character from input.
 * 
 * @return the character from input.
 */
QSH_WEAK int qsh_get_char(void){
    return fgetc(stdin);
}

/**
 * @brief Default method to get a character without line buffer from input.
 * 
 * @return the character from input. 
 */
QSH_WEAK int qsh_getch(void){
    int ch;
    struct termios tm, tm_store;
    tcgetattr(STDIN_FILENO, &tm);
    tm_store = tm;
    tm.c_lflag &= ~(ICANON | ECHO);
    // tm.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &tm);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &tm_store);
    return ch;
}

/**
 * @brief Default method to put a character to output. 
 * 
 * @param ch is the character.
 * @return the character to output.
 */
QSH_WEAK int qsh_put_char(int ch){
    return fputc(ch, stdout);
}

/**
 * @brief Default method to exit the 'qsh' instance.
 * 
 * @return  
 */
QSH_WEAK int qsh_exit(void){
    pthread_exit(NULL);
}