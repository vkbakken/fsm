/*
 * test_traffic_light.c
 *
 *  Created on: Jun 20, 2019
 *      Author: liemlb
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "workq.h"
#include "fsm.h"

pthread_t thread_tick, thread_app;

uint8_t timer;

bool timeout = false;

enum testState
{
    Keyboard = 0,
    Time,
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

enum lightTime
{
    Red = 30,
    Yellow = 10,
    Green = 30
};

static uint32_t app_begin(struct test * const user,
                          struct app_event const * const e);
static uint32_t traffic_light_red(struct test * const user,
                                  struct app_event const * const e);
static uint32_t traffic_light_yellow(struct test * const user,
                                     struct app_event const * const e);
static uint32_t traffic_light_green(struct test * const user,
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

#define rtc_get_tick() tick
#define rtc_set_cmp(val)

WORKQ_DECLARE(main);
WORKQ_ITEM_DECLARE(item3);

static uint32_t tick, next_execute_time;

void timing_handler(void);

void fun3(struct workq_item *item)
{
    workq_cancel(&wq_main, item);
    app_event.event.Signal = Time;
    fsm_dispatch((struct fsm * const ) &mApp,
                 (struct event const * const ) &app_event);
}

static void *tick_thread(void *data)
{
    while (true) {
        usleep(100000);
        if (next_execute_time == tick) {
            timing_handler();
        }

        tick++;

    }

    return NULL;
}

int main(void)
{
    workq_init(&wq_main);
    workq_item_init(&wqi_item3, fun3);
    fsm_init((struct fsm * const ) &mApp);
    pthread_create(&thread_app, NULL, tick_thread, NULL);

    while (true) {
        app_event.key_code = getchar();
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

    while (1) {

    }

    return 0;
}

void timing_handler(void)
{
    uint32_t next_exec_dt = 0;
    uint32_t rtc_begin = 0;
    uint32_t ct = 0, rt = 0;
    recursive:
    rtc_begin = rtc_get_tick();
    ct = next_execute_time;
    next_exec_dt = workq_iterate(&wq_main, ct);

    next_execute_time += next_exec_dt;

    rt = ct + (rtc_get_tick() - rtc_begin);

    if ((next_execute_time - ct > rt - ct) && (next_execute_time - rt > 3)) {
//      rtc_set_cmp(0x00FFFFFF & (rtc_get_tick() + (next_execute_time - rt)));
        rtc_set_cmp(0xFFFFFFFF & (rtc_begin + next_exec_dt));
    } else if (next_exec_dt == 0) {
        //turn off RTC ?
    } else {
        goto recursive;
    }
}

static uint32_t app_init(void * const user)
{
    ((struct test *) user)->super.state_handler =
            (uint32_t (*)
            (void * const user, struct event const * const e)) &app_begin;

    printf("Application version: %s\n", ((struct test *) user)->version);
    printf("Press Exit key to exit.\n");
    printf("Press Enter key to begin traffic light app!\n");

    return 0;
}

static uint32_t app_begin(struct test * const user,
                          struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Time:
        break;
    case Exit:
        app_on_exit();
        break;
    case Keyboard:
        if ('r' == e->key_code) {
            printf("Press Enter key to switch the traffic light.\n");
            printf("RED\n");
            fsm_transform(&(user->super), (uint32_t (*)
                          (void * const user, struct event const * const e))
                          &traffic_light_red);
            next_execute_time = tick
                    + workq_post_delayed(&wq_main, &wqi_item3, Red, tick);
        } else if (10 == e->key_code) {

        } else {
            printf("%c - %u\n", e->key_code, e->key_code);
        }
        break;
    default:
        break;
    }
    return 0;
}

static uint32_t traffic_light_red(struct test * const user,
                                  struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Time:
        printf("YELLOW\n");
        fsm_transform(&(user->super), (uint32_t (*)
                      (void * const user, struct event const * const e))
                      &traffic_light_yellow);
        workq_post_delayed(&wq_main, &wqi_item3, Yellow, tick);
        break;
    case Exit:
        app_on_exit();
        break;
    case Keyboard:
        if ('s' == e->key_code) {
            workq_cancel(&wq_main, &wqi_item3);
            printf("YELLOW - forced switch\n");
            fsm_transform(&(user->super), (uint32_t (*)
                          (void * const user, struct event const * const e))
                          &traffic_light_yellow);
            next_execute_time = tick
                    + workq_post_delayed(&wq_main, &wqi_item3, Yellow, tick);
        } else if (10 == e->key_code) {

        } else {
            printf("%c - %u\n", e->key_code, e->key_code);
        }
        break;
    default:
        break;
    }
    return 0;
}

static uint32_t traffic_light_yellow(struct test * const user,
                                     struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Time:
        printf("GREEN\n");
        fsm_transform(&(user->super), (uint32_t (*)
                      (void * const user, struct event const * const e))
                      &traffic_light_green);
        workq_post_delayed(&wq_main, &wqi_item3, Green, tick);
        break;
    case Exit:
        app_on_exit();
        break;
    case Keyboard:
        break;
    default:
        break;
    }
    return 0;
}

static uint32_t traffic_light_green(struct test * const user,
                                    struct app_event const * const e)
{
    switch (e->event.Signal)
    {
    case Time:
        printf("RED\n");
        fsm_transform(&(user->super), (uint32_t (*)
                      (void * const user, struct event const * const e))
                      &traffic_light_red);
        workq_post_delayed(&wq_main, &wqi_item3, Red, tick);
        break;
    case Keyboard:
        if ('s' == e->key_code) {
            workq_cancel(&wq_main, &wqi_item3);
            printf("RED - forced switch\n");
            fsm_transform(&(user->super), (uint32_t (*)
                          (void * const user, struct event const * const e))
                          &traffic_light_red);
            next_execute_time = tick
                    + workq_post_delayed(&wq_main, &wqi_item3, Red, tick);
        } else if (10 == e->key_code) {

        } else {
            printf("%c - %u\n", e->key_code, e->key_code);
        }
        break;
    case Exit:
        app_on_exit();
        break;
    default:
        break;
    }
    return 0;
}
