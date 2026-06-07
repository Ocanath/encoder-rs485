/*
 * encoder-params.h
 *
 *  Created on: Jun 6, 2026
 *      Author: redux
 */

#ifndef INC_DARTT_MAP_H_
#define INC_DARTT_MAP_H_

#include "fds.h"

typedef struct dartt_map_t
{
	int32_t angle;

	uint16_t sin;
	uint16_t cos;	//these pad to 1

	uint32_t action_flag;

	fds_t fds;

	uint32_t tick;
}dartt_map_t;

extern dartt_map_t gl_dp;

#endif /* INC_DARTT_MAP_H_ */
