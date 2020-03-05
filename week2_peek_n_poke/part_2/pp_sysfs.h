#ifndef _PP_SYSFS_H_
#define _PP_SYSFS_H_

/**
 * @brief Create the sysfs file
 * 
 * @return int  Success or error
 */
int pp_sysfs_init (void);

/**
 * @brief Remove the sysfs file
 * 
 * @return int  Success or error
 */
void pp_sysfs_cleanup (void);

#endif
