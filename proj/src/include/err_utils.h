/** @file err_utils.h */
#ifndef __ERR_UTILS_H__
#define __ERR_UTILS_H__

/** @addtogroup	util_grp
 * @{
 */

/** greeting duration */
#define GREET_LEN 3000000
/** greeting message */
#define GREET_MSG                                                           \
  "Welcome to Skane Royale edition\nIf you have any problems, the LOG_FILE " \
  "might help:"
/** goodbye message */
#define GOODBYE_MSG "Thank you for playing!\n"

/** full path to log file */
#define LOG_FILE "/tmp/skane_logs.txt"
/* #define log_file "skane_logs.txt" */

/**
 * @brief	Prints a warning message to, both, stderr and a logfile.
 *
 * @param fmt	Format string (printf style).
 * @param ...	Format elements (printf style).
 */
void warn(const char* fmt, ...);

/**
 * @brief	Prints a failure message to, both, stderr and a logfile
 *		and exits the program.
 *
 * @param fmt	Format string (printf style).
 * @param ...	Format elements (printf style).
 */
void die(const char* fmt, ...);

/** @brief Greets the player. */
void greet(void);

/** @brief Says goodbye to the player. */
void byebye(void);

/** @brief Clears the log file. */
void clrlogs(void);

/** @} */

#endif // __ERR_UTILS_H__
