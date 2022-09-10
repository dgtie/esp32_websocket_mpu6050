#include "esp_log.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO           22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           21               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              0                /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          400000           /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                /*!< I2C master do not need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU6050_SENSOR_ADDR                 0x68             /*!< slave address for MPU6050 sensor */
#define MPU6050_CMD_START                   0x41             /*!< Command to set measure mode */
#define MPU6050_WHO_AM_I                    0x75             /*!< Command to read WHO_AM_I reg */
#define WRITE_BIT                           I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                            I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                        0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                       0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                             0x0              /*!< I2C ack value */
#define NACK_VAL                            0x1              /*!< I2C nack value */
#define LAST_NACK_VAL                       0x2              /*!< I2C last_nack value */

/**
 * Define the mpu6050 register address:
 */
#define SMPLRT_DIV      0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACCEL_CONFIG    0x1C
#define ACCEL_XOUT_H    0x3B
#define ACCEL_XOUT_L    0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H    0x3F
#define ACCEL_ZOUT_L    0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47
#define GYRO_ZOUT_L     0x48
#define PWR_MGMT_1      0x6B
#define WHO_AM_I        0x75  /*!< Command to read WHO_AM_I reg */

static const char *TAG = "i2c-simple-example";

/**
 * @brief Read a sequence of bytes from a MPU9250 sensor registers
 */
static esp_err_t register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief Write a byte to a MPU9250 sensor register
 */
static esp_err_t register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

/**
 * @brief i2c master initialization
 */

esp_err_t mpu6050_init(void)
{
  esp_err_t ret;
  uint8_t data[2];

  vTaskDelay(100 / portTICK_RATE_MS);   // keep this delay

  int i2c_master_port = I2C_MASTER_NUM;

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  i2c_param_config(i2c_master_port, &conf);

  ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
  if (ret != ESP_OK) return ret;

  ret = register_read(WHO_AM_I, data, 1);
  if (ret != ESP_OK) return ret;
  ESP_LOGI(TAG, "WHO_AM_I = %X", data[0]);

  register_write_byte(PWR_MGMT_1, 0);
  register_write_byte(SMPLRT_DIV, 7);
  register_write_byte(CONFIG, 6);
  register_write_byte(GYRO_CONFIG, 0x18);
  return register_write_byte(ACCEL_CONFIG, 1);

}

void ws_send(uint8_t *data);
void mpu6050_read(void) {
  uint8_t data[14];
//  double Temp;
  register_read(ACCEL_XOUT_H, data, 14);
  ws_send(data);
//  Temp = 36.53 + ((double)(int16_t)((data[6] << 8) | data[7]) / 340);
//  ESP_LOGI(TAG, "TEMP: %d.%d\n", (uint16_t)Temp, (uint16_t)(Temp * 100) % 100);
}
