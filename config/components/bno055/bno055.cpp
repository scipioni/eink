
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "bno055.h"

/** BNO055 ID **/
//#define BNO055_ID (0xA0)

namespace esphome {
namespace bno055 {

static const char *const TAG = "bno055";

const uint8_t BNO055_REGISTER_WHO_AM_I = 0x75;
const uint8_t BNO055_REGISTER_POWER_MANAGEMENT_1 = 0x6B;
const uint8_t BNO055_REGISTER_GYRO_CONFIG = 0x1B;
const uint8_t BNO055_REGISTER_ACCEL_CONFIG = 0x1C;
const uint8_t BNO055_REGISTER_ACCEL_XOUT_H = 0x3B;
const uint8_t BNO055_CLOCK_SOURCE_X_GYRO = 0b001;
const uint8_t BNO055_SCALE_2000_DPS = 0b11;
const float BNO055_SCALE_DPS_PER_DIGIT_2000 = 0.060975f;
const uint8_t BNO055_RANGE_2G = 0b00;
const float BNO055_RANGE_PER_DIGIT_2G = 0.000061f;
const uint8_t BNO055_BIT_SLEEP_ENABLED = 6;
const uint8_t BNO055_BIT_TEMPERATURE_DISABLED = 3;
const float GRAVITY_EARTH = 9.80665f;
const uint8_t BNO055_CHIP_ID_ADDR = 0x00;
const uint8_t BNO055_ID = 0xA0;

void BNO055Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up BNO055...");

  // can take 850 ms to boot!
  delay(850); // in ms


  uint8_t id;
  if (!this->read_byte(BNO055_CHIP_ID_ADDR, &id) ||
      (id != BNO055_ID)) {
    this->mark_failed();
    return;
  }
  ESP_LOGCONFIG(TAG, "Started");
  return; // scipio


  uint8_t who_am_i;
  if (!this->read_byte(BNO055_REGISTER_WHO_AM_I, &who_am_i) ||
      (who_am_i != 0x68 && who_am_i != 0x70 && who_am_i != 0x98)) {
    this->mark_failed();
    return;
  }

  ESP_LOGV(TAG, "  Setting up Power Management...");
  // Setup power management
  uint8_t power_management;
  if (!this->read_byte(BNO055_REGISTER_POWER_MANAGEMENT_1, &power_management)) {
    this->mark_failed();
    return;
  }
  ESP_LOGV(TAG, "  Input power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  // Set clock source - X-Gyro
  power_management &= 0b11111000;
  power_management |= BNO055_CLOCK_SOURCE_X_GYRO;
  // Disable sleep
  power_management &= ~(1 << BNO055_BIT_SLEEP_ENABLED);
  // Enable temperature
  power_management &= ~(1 << BNO055_BIT_TEMPERATURE_DISABLED);
  ESP_LOGV(TAG, "  Output power_management: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(power_management));
  if (!this->write_byte(BNO055_REGISTER_POWER_MANAGEMENT_1, power_management)) {
    this->mark_failed();
    return;
  }

  ESP_LOGV(TAG, "  Setting up Gyro Config...");
  // Set scale - 2000DPS
  uint8_t gyro_config;
  if (!this->read_byte(BNO055_REGISTER_GYRO_CONFIG, &gyro_config)) {
    this->mark_failed();
    return;
  }
  ESP_LOGV(TAG, "  Input gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  gyro_config &= 0b11100111;
  gyro_config |= BNO055_SCALE_2000_DPS << 3;
  ESP_LOGV(TAG, "  Output gyro_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(gyro_config));
  if (!this->write_byte(BNO055_REGISTER_GYRO_CONFIG, gyro_config)) {
    this->mark_failed();
    return;
  }

  ESP_LOGV(TAG, "  Setting up Accel Config...");
  // Set range - 2G
  uint8_t accel_config;
  if (!this->read_byte(BNO055_REGISTER_ACCEL_CONFIG, &accel_config)) {
    this->mark_failed();
    return;
  }
  ESP_LOGV(TAG, "    Input accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  accel_config &= 0b11100111;
  accel_config |= (BNO055_RANGE_2G << 3);
  ESP_LOGV(TAG, "    Output accel_config: 0b" BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(accel_config));
  if (!this->write_byte(BNO055_REGISTER_ACCEL_CONFIG, accel_config)) {
    this->mark_failed();
    return;
  }
}
void BNO055Component::dump_config() {
  ESP_LOGCONFIG(TAG, "BNO055:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with BNO055 failed!");
  }
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Acceleration X", this->accel_x_sensor_);
  LOG_SENSOR("  ", "Acceleration Y", this->accel_y_sensor_);
  LOG_SENSOR("  ", "Acceleration Z", this->accel_z_sensor_);
  LOG_SENSOR("  ", "Gyro X", this->gyro_x_sensor_);
  LOG_SENSOR("  ", "Gyro Y", this->gyro_y_sensor_);
  LOG_SENSOR("  ", "Gyro Z", this->gyro_z_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

void BNO055Component::update() {
  ESP_LOGV(TAG, "    Updating BNO055...");
  
  return; // scipio
  
  uint16_t raw_data[7];
  if (!this->read_bytes_16(BNO055_REGISTER_ACCEL_XOUT_H, raw_data, 7)) {
    this->status_set_warning();
    return;
  }
  auto *data = reinterpret_cast<int16_t *>(raw_data);

  float accel_x = data[0] * BNO055_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_y = data[1] * BNO055_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;
  float accel_z = data[2] * BNO055_RANGE_PER_DIGIT_2G * GRAVITY_EARTH;

  float temperature = data[3] / 340.0f + 36.53f;

  float gyro_x = data[4] * BNO055_SCALE_DPS_PER_DIGIT_2000;
  float gyro_y = data[5] * BNO055_SCALE_DPS_PER_DIGIT_2000;
  float gyro_z = data[6] * BNO055_SCALE_DPS_PER_DIGIT_2000;

  ESP_LOGD(TAG,
           "Got accel={x=%.3f m/s², y=%.3f m/s², z=%.3f m/s²}, "
           "gyro={x=%.3f °/s, y=%.3f °/s, z=%.3f °/s}, temp=%.3f°C",
           accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z, temperature);

  if (this->accel_x_sensor_ != nullptr)
    this->accel_x_sensor_->publish_state(accel_x);
  if (this->accel_y_sensor_ != nullptr)
    this->accel_y_sensor_->publish_state(accel_y);
  if (this->accel_z_sensor_ != nullptr)
    this->accel_z_sensor_->publish_state(accel_z);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);

  if (this->gyro_x_sensor_ != nullptr)
    this->gyro_x_sensor_->publish_state(gyro_x);
  if (this->gyro_y_sensor_ != nullptr)
    this->gyro_y_sensor_->publish_state(gyro_y);
  if (this->gyro_z_sensor_ != nullptr)
    this->gyro_z_sensor_->publish_state(gyro_z);

  this->status_clear_warning();
}
float BNO055Component::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace bno055
}  // namespace esphome
