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

#include "fsm.h"
#include "os_assert.h"

void fsm_init(struct fsm * const user)
{
    __ASSERT(NULL != user);

    user->on_user_init(user);
}

void fsm_dispatch(struct fsm * const user, struct event const * const e)
{
    __ASSERT(NULL != user);
    __ASSERT(NULL != e);

    user->state_handler(user, e);
}

void fsm_transform(struct fsm * const user,
                   uint32_t (*handler)
                   (void * const user, struct event const * const e))
{
    __ASSERT(NULL != user);
    __ASSERT(NULL != handler);

    user->state_handler = handler;
}
