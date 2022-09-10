#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER / 1000)  // convert counter value to milli-seconds

static unsigned tick;

static bool IRAM_ATTR timer_group_isr_callback(void *arg) {
    BaseType_t high_task_awoken = pdFALSE;
    tick++;
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}

void my_timer_init(unsigned alarm) {
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true,
    }; // default clock source is APB
    timer_init(0, 0, &config);
    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(0, 0, 0);
    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(0, 0, alarm * TIMER_SCALE);
    timer_enable_intr(0, 0);
    timer_isr_callback_add(0, 0, timer_group_isr_callback, 0, 0);
    timer_start(0, 0);
}

unsigned get_tick(void) { return tick; }
