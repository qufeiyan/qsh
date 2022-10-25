/**
 * @file qsh_opt.h
 * @author qufeiyan (2491411913@qq.com)
 * @brief optional defination for qsh.
 * @version 0.1
 * @date 2022-10-14
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __QSH__OPT__
#define __QSH__OPT__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enable historical command.
 * 
 */
#define QSH_ENABLE_HISTORY_COMMAND          (1)
/**
 * @brief Only enable minimal history.
 * 
 */
#if QSH_ENABLE_HISTORY_COMMAND
#define QSH_ONLY_ENABLE_MINIMAL_HISTORY     (0)
#endif

/**
 * @brief The number of the arguments supported by one command.
 * 
 */
#define QSH_MAX_COMMAND_ARGS                (8)

/**
 * @brief The maximum size for each command.
 * 
 */
#define QSH_MAX_COMMAND_SIZE                (80)


#if QSH_ENABLE_HISTORY_COMMAND
/**
 * @brief The maximum number of the historical commands.
 * 
 * @note The size must be 2 the nth power.
 */
#define QSH_MAX_HISTORY_SIZE                (8)
#endif

/**
 * @brief The buffer size of the formated output. 
 * 
 */
#define QSH_OUTPUT_BUFFER                   (256 * 2)

/**
 * @brief The macro decide whether to display a smaller qsh's logo.  
 * @note  the biger logo needs QSH_OUTPUT_BUFFER is greater than or qual to 256 * 2.      
 */
#define QSH_LOGO_SELECT_MINIMAL             (1)

/**
 * @brief Whether to enable global variable tracking
 * 
 */
#define QSH_ENABLE_VARIABLE_TRACE           (1)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif





