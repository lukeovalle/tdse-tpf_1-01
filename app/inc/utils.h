/*
 * utils.h
 *
 *  Created on: Feb 3, 2026
 *      Author: luke
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#define MIN(val1, val2) ( (val1) < (val2) ? (val1) : (val2) )
#define MAX(val1, val2) ( (val1) > (val2) ? (val1) : (val2) )
#define CLAMP(val, min, max) ( MIN(max, MAX(val, min)) )

#endif /* INC_UTILS_H_ */
