/*
 * app_fsm.c
 *
 *  Created on: Jun 14, 2019
 *      Author: liemlb
 *
 * An sample application to demonstrate FSM usage
 *
 */

#include "fsm.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

enum testState
{
    Keyboard = 0,
    Exit
};

char pin_code[] = "20122019*";

struct app_event
{
    struct event event;

    uint8_t key_code;
};

static uint32_t app_init(void * const user);

struct test
{
    struct fsm super;

    char *version;
};

static uint32_t app_begin(struct test * const user,
                          struct app_event const * const e);
static uint32_t app_login(struct test * const user,
                          struct app_event const * const e);
static uint32_t app_passed(struct test * const user,
                           struct app_event const * const e);

static void app_on_exit(void)
{
    printf("Exit test app!\n");
    exit(0);
}

struct app_event app_event;
struct test mApp = {
    .super.on_user_init = &app_init,
    .version = "2019.07.18"
};

int main(void)
{
    fsm_init((struct fsm * const ) &mApp);

    while (true) {
        if(-1 == system("/bin/stty raw")){}
        app_event.key_code = getchar();
        if(-1 == system("/bin/stty cooked")){}
        printf(" - %u\n", app_event.key_code);

        switch (app_event.key_code)
        {
        case 27:
            app_event.event.Signal = Exit;
            break;
        default:
            app_event.event.Signal = Keyboard;
            break;
        }

        fsm_dispatch((struct fsm * const ) &mApp,
                     (struct event const * const ) &app_event);
    }
    return 0;
}

static uint32_t app_init(void * const user)
{
    ((struct test *) user)->super.state_handler = (uint32_t (*)
            (void * const user, struct event const * const e)) &app_begin;

    printf("Application version: %s\n", ((struct test *) user)->version);
    printf("Press Esc key to exit.\n");
    printf("PIN code size: %ld\n", strlen(pin_code));
    printf("Press Enter key to begin enter PIN code: ");
    return 0;
}

static uint32_t app_begin(struct test * const user,
                          struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Exit:
        app_on_exit();
        break;
    case Keyboard:
        if (13 == e->key_code) {
            printf("Go to Login!\n");
            printf("Enter PIN code to log in: ");
            fsm_transform(&(user->super), (uint32_t (*)
                          (void * const user, struct event const * const e))
                          &app_login);
        } else {
            printf("Press Enter key to retry enter PIN code: ");
        }
        break;
    default:
        printf("Press Enter key to retry enter PIN code: ");
        break;
    }
    return 0;
}

static uint32_t app_login(struct test * const user,
                          struct app_event const * const e)
{
    static uint8_t i = 0;
    switch (e->event.Signal)
    {
    case Exit:
        app_on_exit();
        break;
    case Keyboard:
        if (e->key_code == pin_code[i]) {
            if (++i == strlen(pin_code)) {
                printf("Correct PIN code, Logged in!\n");
                fsm_transform(&(user->super), (uint32_t (*)
                              (void * const user, struct event const * const e))
                              &app_passed);
            } else {
                printf("Enter next PIN code to log in: ");
            }
        } else {
            i = 0;
            printf("Incorrect PIN code, go to Begin!\n");
            printf("Press Enter key to retry enter PIN code: ");
            fsm_transform(&(user->super), (uint32_t (*)
                          (void * const user, struct event const * const e))
                          &app_begin);
        }
        break;
    default:
        break;
    }
    return 0;
}

static uint32_t app_passed(struct test * const user,
                           struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Exit:
        app_on_exit();
        break;
    default:
        break;
    }
    return 0;
}
