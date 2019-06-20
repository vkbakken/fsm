/*
 * fsm.h
 *
 *  Created on: Jun 14, 2019
 *      Author: liemlb
 *
 * This module provide basic functions of the FSM:
 *     - Dispatch signal to current state.
 *     - Switch to others state.
 */

#ifndef FSM_H_INCLUDED
#define FSM_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>

struct event
{
	uint32_t Signal;
};

typedef uint32_t (*fsm_state_handler)(void * const user, struct event const * const e);

struct fsm
{
	fsm_state_handler state_handler;
	uint32_t (*on_user_init)(void * const user);
};

#ifdef __cplusplus
extern "C"
{
#endif

void fsm_init(struct fsm * const user);
void fsm_dispatch(struct fsm * const user, struct event const * const e);
void fsm_transform(struct fsm * const user,
					uint32_t (*handler)(void * const user,
										struct event const * const e));

#ifdef __cplusplus
}
#endif
#endif /* FSM_H_INCLUDED */
