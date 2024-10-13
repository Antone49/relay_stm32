/****************************************************************************************************************************/
/*                                                                                                                          */
/*     Interface utilisateur                           lib_log.h                                                            */
/*     ---------------------                           .......                                                              */
/*                                                                                                                          */
/****************************************************************************************************************************/
/*                                                                                                                          */
/*   Mode d'utilisation :                                                                                                   */
/*   ------------------                                                                                                     */
/*                                                                                                                          */
/*   Remarques :                                                                                                            */
/*                                                                                                                          */
/*   Exemple :                                                                                                              */
/*                                                                                                                          */
/*                                                                                                                          */
/****************************************************************************************************************************/

#ifndef _LIB_LOG_H
#define _LIB_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>

/* Declaration des define utilisables ***************************************************************************************/

#define LIST_LOG_LEVEL                                                                                                                               \
    X(EMERG)                                                                                                                                         \
    X(ALERT)                                                                                                                                         \
    X(CRIT)                                                                                                                                          \
    X(ERROR)                                                                                                                                         \
    X(WARN)                                                                                                                                          \
    X(NOTICE)                                                                                                                                        \
    X(INFO)                                                                                                                                          \
    X(DEBUG)

#define X(name) LOG_LEVEL_##name,

typedef enum { LIST_LOG_LEVEL } log_level_e;

#undef X

#ifdef SOURCE_PATH_SIZE
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
#else
#define __FILENAME__ __FILE__
#endif

#define LOG(level, fmt, args...) log_write(level, "%s.%d: " fmt, __FILENAME__, __LINE__, ##args);

#define LOG_EMERG(fmt, args...)  LOG(LOG_LEVEL_EMERG, fmt, ##args)

#define LOG_ALERT(fmt, args...)  LOG(LOG_LEVEL_ALERT, fmt, ##args)

#define LOG_CRIT(fmt, args...)   LOG(LOG_LEVEL_CRIT, fmt, ##args)

#define LOG_ERROR(fmt, args...)  LOG(LOG_LEVEL_ERROR, fmt, ##args)

#define LOG_WARN(fmt, args...)   LOG(LOG_LEVEL_WARN, fmt, ##args)

#define LOG_NOTICE(fmt, args...) LOG(LOG_LEVEL_NOTICE, fmt, ##args)

#define LOG_INFO(fmt, args...)   LOG(LOG_LEVEL_INFO, fmt, ##args)

#ifdef DEBUG
#define LOG_DEBUG(fmt, args...) LOG(LOG_LEVEL_DEBUG, fmt, ##args)
#else
#define LOG_DEBUG(fmt, args...)
#endif

/****************************************************************************************************************************/
/*                                      log_write                                                                           */
/*                                                                                                                          */
/*  Description rapide   : Permet d'ajouter un log dans un fichier                                                          */
/*                                                                                                                          */
/*  Parametres d'entree  : - Niveau d'affichage                                                                             */
/*                         - Texte a ajouter                                                                                */
/*                                                                                                                          */
/*  Parametres de sortie : - Aucun                                                                                          */
/*                                                                                                                          */
/****************************************************************************************************************************/
void log_write(int level, char* format, ...);

void log_stdout_set_enable(bool enable);

char* log_level_name(log_level_e level);

#endif
/************************************************* FIN DE FICHIER ************************************************************/
