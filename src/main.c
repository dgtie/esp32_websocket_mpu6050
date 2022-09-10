
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

void wifi_init(void);
void http_server(void);
void my_timer_init(unsigned);
unsigned get_tick(void);
esp_err_t mpu6050_init(void);
void mpu6050_read(void);

//static const char *TAG = "badminton";

void app_main(void)
{
  unsigned tick;

  gpio_pad_select_gpio(2);  // config LED indicator
  gpio_set_direction(2, GPIO_MODE_OUTPUT);

  if (mpu6050_init() == ESP_OK) my_timer_init(200);
  else {
    my_timer_init(500);
    tick = get_tick();
    while (1) {            // blinks to indicate gyro fails initisation
      if (tick != get_tick()) {
        tick = get_tick();
        gpio_set_level(2, tick & 1 ? 1 : 0);
      }
      vTaskDelay(1);
    }
  }

  wifi_init();
  http_server();

  tick = get_tick();
  while (1) {               // try sending gyro data periodically
    if (tick != get_tick()) {
      tick = get_tick();
      mpu6050_read();
    }
    vTaskDelay(1);
  }

}