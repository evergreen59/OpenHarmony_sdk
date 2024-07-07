/*
 * This is an automatically generated HDF config file. Do not modify it manually.
 */

#ifndef HCS_CONFIG_HDF_MACRO_TEST_HEADER_H
#define HCS_CONFIG_HDF_MACRO_TEST_HEADER_H

#include "hdf_base.h"

// hcs node macros: root
#define root_exists 1
#define root_nodeName "root"
#define root_foreach_child(func) \
    func(root_platform); \
    func(root_device_info); \
    func(root_lcdkit_config); \
    func(root_backlightConfig); \
    func(root_input_config); \
    func(root_wlan_config); \
    func(root_sensorConfig); \
    func(root_accel_bmi160_chip_config); \
    func(root_als_bh1745_chip_config); \
    func(root_gyro_bmi160_chip_config); \
    func(root_barometer_bmp180_chip_config); \
    func(root_hall_ak8789_chip_config); \
    func(root_magnetic_lsm303_chip_config); \
    func(root_proximity_apds9960_chip_config); \
    func(root_vibratorConfig); \
    func(root_linearVibratorConfig); \
    func(root_audio_info); \
    func(root_fingerprint_info); \
    func(root_data_type_test); \
    func(root_fingerprint_three); \
    func(root_gpio_config); \
    func(root_i2c_config); \
    func(root_pwm_test); \
    func(root_spi_test); \
    func(root_sdio_config); \
    func(root_emmc_config); \
    func(root_uart_test); \
    func(root_regulator_test_config); \
    func(root_rtc_config); \
    func(root_adc_config); \
    func(root_watchdog_config);

#define root_foreach_child_vargs(func, ...) \
    func(root_platform, __VA_ARGS__); \
    func(root_device_info, __VA_ARGS__); \
    func(root_lcdkit_config, __VA_ARGS__); \
    func(root_backlightConfig, __VA_ARGS__); \
    func(root_input_config, __VA_ARGS__); \
    func(root_wlan_config, __VA_ARGS__); \
    func(root_sensorConfig, __VA_ARGS__); \
    func(root_accel_bmi160_chip_config, __VA_ARGS__); \
    func(root_als_bh1745_chip_config, __VA_ARGS__); \
    func(root_gyro_bmi160_chip_config, __VA_ARGS__); \
    func(root_barometer_bmp180_chip_config, __VA_ARGS__); \
    func(root_hall_ak8789_chip_config, __VA_ARGS__); \
    func(root_magnetic_lsm303_chip_config, __VA_ARGS__); \
    func(root_proximity_apds9960_chip_config, __VA_ARGS__); \
    func(root_vibratorConfig, __VA_ARGS__); \
    func(root_linearVibratorConfig, __VA_ARGS__); \
    func(root_audio_info, __VA_ARGS__); \
    func(root_fingerprint_info, __VA_ARGS__); \
    func(root_data_type_test, __VA_ARGS__); \
    func(root_fingerprint_three, __VA_ARGS__); \
    func(root_gpio_config, __VA_ARGS__); \
    func(root_i2c_config, __VA_ARGS__); \
    func(root_pwm_test, __VA_ARGS__); \
    func(root_spi_test, __VA_ARGS__); \
    func(root_sdio_config, __VA_ARGS__); \
    func(root_emmc_config, __VA_ARGS__); \
    func(root_uart_test, __VA_ARGS__); \
    func(root_regulator_test_config, __VA_ARGS__); \
    func(root_rtc_config, __VA_ARGS__); \
    func(root_adc_config, __VA_ARGS__); \
    func(root_watchdog_config, __VA_ARGS__);


// hcs node macros: root/platform
#define root_platform_exists 1
#define root_platform_nodeName "platform"
#define root_platform_foreach_child(func) \
    func(root_platform_gpio_config); \
    func(root_platform_i2c_config); \
    func(root_platform_watchdog_config); \
    func(root_platform_pwm_config); \
    func(root_platform_uart_config); \
    func(root_platform_sdio_config); \
    func(root_platform_emmc_config); \
    func(root_platform_spi_config); \
    func(root_platform_adc_config); \
    func(root_platform_regulator_config);

#define root_platform_foreach_child_vargs(func, ...) \
    func(root_platform_gpio_config, __VA_ARGS__); \
    func(root_platform_i2c_config, __VA_ARGS__); \
    func(root_platform_watchdog_config, __VA_ARGS__); \
    func(root_platform_pwm_config, __VA_ARGS__); \
    func(root_platform_uart_config, __VA_ARGS__); \
    func(root_platform_sdio_config, __VA_ARGS__); \
    func(root_platform_emmc_config, __VA_ARGS__); \
    func(root_platform_spi_config, __VA_ARGS__); \
    func(root_platform_adc_config, __VA_ARGS__); \
    func(root_platform_regulator_config, __VA_ARGS__);


// hcs node macros: root/platform/gpio_config
#define root_platform_gpio_config_exists 1
#define root_platform_gpio_config_nodeName "gpio_config"
#define root_platform_gpio_config_foreach_child(func) \
    func(root_platform_gpio_config_controller_0x120d0000);

#define root_platform_gpio_config_foreach_child_vargs(func, ...) \
    func(root_platform_gpio_config_controller_0x120d0000, __VA_ARGS__);


// hcs node macros: root/platform/gpio_config/controller_0x120d0000
#define root_platform_gpio_config_controller_0x120d0000_exists 1
#define root_platform_gpio_config_controller_0x120d0000_nodeName "controller_0x120d0000"
#define root_platform_gpio_config_controller_0x120d0000_match_attr_exists 1
#define root_platform_gpio_config_controller_0x120d0000_match_attr "linux_gpio_adapter"
#define root_platform_gpio_config_controller_0x120d0000_groupNum_exists 1
#define root_platform_gpio_config_controller_0x120d0000_groupNum 12
#define root_platform_gpio_config_controller_0x120d0000_bitNum_exists 1
#define root_platform_gpio_config_controller_0x120d0000_bitNum 8
#define root_platform_gpio_config_controller_0x120d0000_regBase_exists 1
#define root_platform_gpio_config_controller_0x120d0000_regBase 302841856
#define root_platform_gpio_config_controller_0x120d0000_regStep_exists 1
#define root_platform_gpio_config_controller_0x120d0000_regStep 4096
#define root_platform_gpio_config_controller_0x120d0000_irqStart_exists 1
#define root_platform_gpio_config_controller_0x120d0000_irqStart 48
#define root_platform_gpio_config_controller_0x120d0000_irqShare_exists 1
#define root_platform_gpio_config_controller_0x120d0000_irqShare 0

// hcs node macros: root/platform/i2c_config
#define root_platform_i2c_config_exists 1
#define root_platform_i2c_config_nodeName "i2c_config"
#define root_platform_i2c_config_foreach_child(func) \
    func(root_platform_i2c_config_controller_0x120b0000); \
    func(root_platform_i2c_config_controller_0x120b1000); \
    func(root_platform_i2c_config_controller_0x120b2000); \
    func(root_platform_i2c_config_controller_0x120b3000); \
    func(root_platform_i2c_config_controller_0x120b4000); \
    func(root_platform_i2c_config_controller_0x120b5000); \
    func(root_platform_i2c_config_controller_0x120b6000); \
    func(root_platform_i2c_config_controller_0x120b7000);

#define root_platform_i2c_config_foreach_child_vargs(func, ...) \
    func(root_platform_i2c_config_controller_0x120b0000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b1000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b2000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b3000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b4000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b5000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b6000, __VA_ARGS__); \
    func(root_platform_i2c_config_controller_0x120b7000, __VA_ARGS__);

#define root_platform_i2c_config_match_attr_exists 1
#define root_platform_i2c_config_match_attr "linux_i2c_adapter"

// hcs node macros: root/platform/i2c_config/controller_0x120b0000
#define root_platform_i2c_config_controller_0x120b0000_exists 1
#define root_platform_i2c_config_controller_0x120b0000_nodeName "controller_0x120b0000"
#define root_platform_i2c_config_controller_0x120b0000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b0000_bus 0

// hcs node macros: root/platform/i2c_config/controller_0x120b1000
#define root_platform_i2c_config_controller_0x120b1000_exists 1
#define root_platform_i2c_config_controller_0x120b1000_nodeName "controller_0x120b1000"
#define root_platform_i2c_config_controller_0x120b1000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b1000_bus 1

// hcs node macros: root/platform/i2c_config/controller_0x120b2000
#define root_platform_i2c_config_controller_0x120b2000_exists 1
#define root_platform_i2c_config_controller_0x120b2000_nodeName "controller_0x120b2000"
#define root_platform_i2c_config_controller_0x120b2000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b2000_bus 2

// hcs node macros: root/platform/i2c_config/controller_0x120b3000
#define root_platform_i2c_config_controller_0x120b3000_exists 1
#define root_platform_i2c_config_controller_0x120b3000_nodeName "controller_0x120b3000"
#define root_platform_i2c_config_controller_0x120b3000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b3000_bus 3

// hcs node macros: root/platform/i2c_config/controller_0x120b4000
#define root_platform_i2c_config_controller_0x120b4000_exists 1
#define root_platform_i2c_config_controller_0x120b4000_nodeName "controller_0x120b4000"
#define root_platform_i2c_config_controller_0x120b4000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b4000_bus 4

// hcs node macros: root/platform/i2c_config/controller_0x120b5000
#define root_platform_i2c_config_controller_0x120b5000_exists 1
#define root_platform_i2c_config_controller_0x120b5000_nodeName "controller_0x120b5000"
#define root_platform_i2c_config_controller_0x120b5000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b5000_bus 5

// hcs node macros: root/platform/i2c_config/controller_0x120b6000
#define root_platform_i2c_config_controller_0x120b6000_exists 1
#define root_platform_i2c_config_controller_0x120b6000_nodeName "controller_0x120b6000"
#define root_platform_i2c_config_controller_0x120b6000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b6000_bus 6

// hcs node macros: root/platform/i2c_config/controller_0x120b7000
#define root_platform_i2c_config_controller_0x120b7000_exists 1
#define root_platform_i2c_config_controller_0x120b7000_nodeName "controller_0x120b7000"
#define root_platform_i2c_config_controller_0x120b7000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b7000_bus 7

// hcs node macros: root/platform/watchdog_config
#define root_platform_watchdog_config_exists 1
#define root_platform_watchdog_config_nodeName "watchdog_config"
#define root_platform_watchdog_config_foreach_child(func) \
    func(root_platform_watchdog_config_device_0x12050000);

#define root_platform_watchdog_config_foreach_child_vargs(func, ...) \
    func(root_platform_watchdog_config_device_0x12050000, __VA_ARGS__);


// hcs node macros: root/platform/watchdog_config/device_0x12050000
#define root_platform_watchdog_config_device_0x12050000_exists 1
#define root_platform_watchdog_config_device_0x12050000_nodeName "device_0x12050000"
#define root_platform_watchdog_config_device_0x12050000_id_exists 1
#define root_platform_watchdog_config_device_0x12050000_id 0
#define root_platform_watchdog_config_device_0x12050000_match_attr_exists 1
#define root_platform_watchdog_config_device_0x12050000_match_attr "hisilicon_hi35xx_watchdog_0"
#define root_platform_watchdog_config_device_0x12050000_serviceName_exists 1
#define root_platform_watchdog_config_device_0x12050000_serviceName "HDF_PLATFORM_WATCHDOG_0"

// hcs node macros: root/platform/pwm_config
#define root_platform_pwm_config_exists 1
#define root_platform_pwm_config_nodeName "pwm_config"
#define root_platform_pwm_config_foreach_child(func) \
    func(root_platform_pwm_config_device_0x12070000); \
    func(root_platform_pwm_config_device_0x12070020);

#define root_platform_pwm_config_foreach_child_vargs(func, ...) \
    func(root_platform_pwm_config_device_0x12070000, __VA_ARGS__); \
    func(root_platform_pwm_config_device_0x12070020, __VA_ARGS__);


// hcs node macros: root/platform/pwm_config/device_0x12070000
#define root_platform_pwm_config_device_0x12070000_exists 1
#define root_platform_pwm_config_device_0x12070000_nodeName "device_0x12070000"
#define root_platform_pwm_config_device_0x12070000_match_attr_exists 1
#define root_platform_pwm_config_device_0x12070000_match_attr "hisilicon_hi35xx_pwm_0"
#define root_platform_pwm_config_device_0x12070000_serviceName_exists 1
#define root_platform_pwm_config_device_0x12070000_serviceName ""
#define root_platform_pwm_config_device_0x12070000_num_exists 1
#define root_platform_pwm_config_device_0x12070000_num 0
#define root_platform_pwm_config_device_0x12070000_base_exists 1
#define root_platform_pwm_config_device_0x12070000_base 302448640

// hcs node macros: root/platform/pwm_config/device_0x12070020
#define root_platform_pwm_config_device_0x12070020_exists 1
#define root_platform_pwm_config_device_0x12070020_nodeName "device_0x12070020"
#define root_platform_pwm_config_device_0x12070020_match_attr_exists 1
#define root_platform_pwm_config_device_0x12070020_match_attr "hisilicon_hi35xx_pwm_1"
#define root_platform_pwm_config_device_0x12070020_num_exists 1
#define root_platform_pwm_config_device_0x12070020_num 1
#define root_platform_pwm_config_device_0x12070020_base_exists 1
#define root_platform_pwm_config_device_0x12070020_base 302448672
#define root_platform_pwm_config_device_0x12070020_serviceName_exists 1
#define root_platform_pwm_config_device_0x12070020_serviceName ""

// hcs node macros: root/platform/uart_config
#define root_platform_uart_config_exists 1
#define root_platform_uart_config_nodeName "uart_config"
#define root_platform_uart_config_foreach_child(func) \
    func(root_platform_uart_config_device_uart_0x0000); \
    func(root_platform_uart_config_device_uart_0x0001);

#define root_platform_uart_config_foreach_child_vargs(func, ...) \
    func(root_platform_uart_config_device_uart_0x0000, __VA_ARGS__); \
    func(root_platform_uart_config_device_uart_0x0001, __VA_ARGS__);


// hcs node macros: root/platform/uart_config/device_uart_0x0000
#define root_platform_uart_config_device_uart_0x0000_exists 1
#define root_platform_uart_config_device_uart_0x0000_nodeName "device_uart_0x0000"
#define root_platform_uart_config_device_uart_0x0000_match_attr_exists 1
#define root_platform_uart_config_device_uart_0x0000_match_attr "hisilicon_hi35xx_uart_0"
#define root_platform_uart_config_device_uart_0x0000_serviceName_exists 1
#define root_platform_uart_config_device_uart_0x0000_serviceName ""
#define root_platform_uart_config_device_uart_0x0000_driver_name_exists 1
#define root_platform_uart_config_device_uart_0x0000_driver_name "ttyAMA"
#define root_platform_uart_config_device_uart_0x0000_num_exists 1
#define root_platform_uart_config_device_uart_0x0000_num 0

// hcs node macros: root/platform/uart_config/device_uart_0x0001
#define root_platform_uart_config_device_uart_0x0001_exists 1
#define root_platform_uart_config_device_uart_0x0001_nodeName "device_uart_0x0001"
#define root_platform_uart_config_device_uart_0x0001_num_exists 1
#define root_platform_uart_config_device_uart_0x0001_num 1
#define root_platform_uart_config_device_uart_0x0001_match_attr_exists 1
#define root_platform_uart_config_device_uart_0x0001_match_attr "hisilicon_hi35xx_uart_1"
#define root_platform_uart_config_device_uart_0x0001_serviceName_exists 1
#define root_platform_uart_config_device_uart_0x0001_serviceName ""
#define root_platform_uart_config_device_uart_0x0001_driver_name_exists 1
#define root_platform_uart_config_device_uart_0x0001_driver_name "ttyAMA"

// hcs node macros: root/platform/sdio_config
#define root_platform_sdio_config_exists 1
#define root_platform_sdio_config_nodeName "sdio_config"
#define root_platform_sdio_config_foreach_child(func) \
    func(root_platform_sdio_config_controller_0x2dd1);

#define root_platform_sdio_config_foreach_child_vargs(func, ...) \
    func(root_platform_sdio_config_controller_0x2dd1, __VA_ARGS__);


// hcs node macros: root/platform/sdio_config/controller_0x2dd1
#define root_platform_sdio_config_controller_0x2dd1_exists 1
#define root_platform_sdio_config_controller_0x2dd1_nodeName "controller_0x2dd1"
#define root_platform_sdio_config_controller_0x2dd1_match_attr_exists 1
#define root_platform_sdio_config_controller_0x2dd1_match_attr "hisilicon_hi35xx_sdio_0"
#define root_platform_sdio_config_controller_0x2dd1_hostId_exists 1
#define root_platform_sdio_config_controller_0x2dd1_hostId 2
#define root_platform_sdio_config_controller_0x2dd1_devType_exists 1
#define root_platform_sdio_config_controller_0x2dd1_devType 2

// hcs node macros: root/platform/emmc_config
#define root_platform_emmc_config_exists 1
#define root_platform_emmc_config_nodeName "emmc_config"
#define root_platform_emmc_config_foreach_child(func) \
    func(root_platform_emmc_config_controller_0x00);

#define root_platform_emmc_config_foreach_child_vargs(func, ...) \
    func(root_platform_emmc_config_controller_0x00, __VA_ARGS__);


// hcs node macros: root/platform/emmc_config/controller_0x00
#define root_platform_emmc_config_controller_0x00_exists 1
#define root_platform_emmc_config_controller_0x00_nodeName "controller_0x00"
#define root_platform_emmc_config_controller_0x00_match_attr_exists 1
#define root_platform_emmc_config_controller_0x00_match_attr "hisilicon_hi35xx_emmc_0"
#define root_platform_emmc_config_controller_0x00_hostId_exists 1
#define root_platform_emmc_config_controller_0x00_hostId 0
#define root_platform_emmc_config_controller_0x00_devType_exists 1
#define root_platform_emmc_config_controller_0x00_devType 0

// hcs node macros: root/platform/spi_config
#define root_platform_spi_config_exists 1
#define root_platform_spi_config_nodeName "spi_config"
#define root_platform_spi_config_foreach_child(func) \
    func(root_platform_spi_config_controller_0x120c0000); \
    func(root_platform_spi_config_controller_0x120c1000); \
    func(root_platform_spi_config_controller_0x120c2000);

#define root_platform_spi_config_foreach_child_vargs(func, ...) \
    func(root_platform_spi_config_controller_0x120c0000, __VA_ARGS__); \
    func(root_platform_spi_config_controller_0x120c1000, __VA_ARGS__); \
    func(root_platform_spi_config_controller_0x120c2000, __VA_ARGS__);


// hcs node macros: root/platform/spi_config/controller_0x120c0000
#define root_platform_spi_config_controller_0x120c0000_exists 1
#define root_platform_spi_config_controller_0x120c0000_nodeName "controller_0x120c0000"
#define root_platform_spi_config_controller_0x120c0000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c0000_busNum 0
#define root_platform_spi_config_controller_0x120c0000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c0000_numCs 1
#define root_platform_spi_config_controller_0x120c0000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c0000_match_attr "hisilicon_hi35xx_spi_0"
#define root_platform_spi_config_controller_0x120c0000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c0000_serviceName ""

// hcs node macros: root/platform/spi_config/controller_0x120c1000
#define root_platform_spi_config_controller_0x120c1000_exists 1
#define root_platform_spi_config_controller_0x120c1000_nodeName "controller_0x120c1000"
#define root_platform_spi_config_controller_0x120c1000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c1000_match_attr "hisilicon_hi35xx_spi_1"
#define root_platform_spi_config_controller_0x120c1000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c1000_busNum 1
#define root_platform_spi_config_controller_0x120c1000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c1000_numCs 2
#define root_platform_spi_config_controller_0x120c1000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c1000_serviceName ""

// hcs node macros: root/platform/spi_config/controller_0x120c2000
#define root_platform_spi_config_controller_0x120c2000_exists 1
#define root_platform_spi_config_controller_0x120c2000_nodeName "controller_0x120c2000"
#define root_platform_spi_config_controller_0x120c2000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c2000_match_attr "hisilicon_hi35xx_spi_2"
#define root_platform_spi_config_controller_0x120c2000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c2000_busNum 2
#define root_platform_spi_config_controller_0x120c2000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c2000_numCs 1
#define root_platform_spi_config_controller_0x120c2000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c2000_serviceName ""

// hcs node macros: root/platform/adc_config
#define root_platform_adc_config_exists 1
#define root_platform_adc_config_nodeName "adc_config"
#define root_platform_adc_config_foreach_child(func) \
    func(root_platform_adc_config_device_adc_0x0000);

#define root_platform_adc_config_foreach_child_vargs(func, ...) \
    func(root_platform_adc_config_device_adc_0x0000, __VA_ARGS__);

#define root_platform_adc_config_match_attr_exists 1
#define root_platform_adc_config_match_attr "linux_adc_adapter_0"

// hcs node macros: root/platform/adc_config/device_adc_0x0000
#define root_platform_adc_config_device_adc_0x0000_exists 1
#define root_platform_adc_config_device_adc_0x0000_nodeName "device_adc_0x0000"
#define root_platform_adc_config_device_adc_0x0000_channelNum_exists 1
#define root_platform_adc_config_device_adc_0x0000_channelNum 2
#define root_platform_adc_config_device_adc_0x0000_driver_channel0_name_exists 1
#define root_platform_adc_config_device_adc_0x0000_driver_channel0_name "/sys/bus/iio/devices/iio:device0/in_voltage0-voltage0_raw"
#define root_platform_adc_config_device_adc_0x0000_driver_channel1_name_exists 1
#define root_platform_adc_config_device_adc_0x0000_driver_channel1_name "/sys/bus/iio/devices/iio:device0/in_voltage1-voltage0_raw"
#define root_platform_adc_config_device_adc_0x0000_deviceNum_exists 1
#define root_platform_adc_config_device_adc_0x0000_deviceNum 2
#define root_platform_adc_config_device_adc_0x0000_serviceName_exists 1
#define root_platform_adc_config_device_adc_0x0000_serviceName ""
#define root_platform_adc_config_device_adc_0x0000_scanMode_exists 1
#define root_platform_adc_config_device_adc_0x0000_scanMode 0
#define root_platform_adc_config_device_adc_0x0000_rate_exists 1
#define root_platform_adc_config_device_adc_0x0000_rate 100

// hcs node macros: root/platform/regulator_config
#define root_platform_regulator_config_exists 1
#define root_platform_regulator_config_nodeName "regulator_config"
#define root_platform_regulator_config_foreach_child(func) \
    func(root_platform_regulator_config_controller_0x130d0000); \
    func(root_platform_regulator_config_controller_0x130d0001);

#define root_platform_regulator_config_foreach_child_vargs(func, ...) \
    func(root_platform_regulator_config_controller_0x130d0000, __VA_ARGS__); \
    func(root_platform_regulator_config_controller_0x130d0001, __VA_ARGS__);

#define root_platform_regulator_config_match_attr_exists 1
#define root_platform_regulator_config_match_attr "linux_regulator_adapter"

// hcs node macros: root/platform/regulator_config/controller_0x130d0000
#define root_platform_regulator_config_controller_0x130d0000_exists 1
#define root_platform_regulator_config_controller_0x130d0000_nodeName "controller_0x130d0000"
#define root_platform_regulator_config_controller_0x130d0000_device_num_exists 1
#define root_platform_regulator_config_controller_0x130d0000_device_num 1
#define root_platform_regulator_config_controller_0x130d0000_name_exists 1
#define root_platform_regulator_config_controller_0x130d0000_name "regulator_adapter_1"
#define root_platform_regulator_config_controller_0x130d0000_devName_exists 1
#define root_platform_regulator_config_controller_0x130d0000_devName "regulator_adapter_consumer01"
#define root_platform_regulator_config_controller_0x130d0000_supplyName_exists 1
#define root_platform_regulator_config_controller_0x130d0000_supplyName "vir-voltage-reg-hdf-adp"
#define root_platform_regulator_config_controller_0x130d0000_mode_exists 1
#define root_platform_regulator_config_controller_0x130d0000_mode 1
#define root_platform_regulator_config_controller_0x130d0000_minUv_exists 1
#define root_platform_regulator_config_controller_0x130d0000_minUv 1000
#define root_platform_regulator_config_controller_0x130d0000_maxUv_exists 1
#define root_platform_regulator_config_controller_0x130d0000_maxUv 50000
#define root_platform_regulator_config_controller_0x130d0000_minUa_exists 1
#define root_platform_regulator_config_controller_0x130d0000_minUa 0
#define root_platform_regulator_config_controller_0x130d0000_maxUa_exists 1
#define root_platform_regulator_config_controller_0x130d0000_maxUa 0

// hcs node macros: root/platform/regulator_config/controller_0x130d0001
#define root_platform_regulator_config_controller_0x130d0001_exists 1
#define root_platform_regulator_config_controller_0x130d0001_nodeName "controller_0x130d0001"
#define root_platform_regulator_config_controller_0x130d0001_device_num_exists 1
#define root_platform_regulator_config_controller_0x130d0001_device_num 1
#define root_platform_regulator_config_controller_0x130d0001_name_exists 1
#define root_platform_regulator_config_controller_0x130d0001_name "regulator_adapter_2"
#define root_platform_regulator_config_controller_0x130d0001_devName_exists 1
#define root_platform_regulator_config_controller_0x130d0001_devName "regulator_adapter_consumer01"
#define root_platform_regulator_config_controller_0x130d0001_supplyName_exists 1
#define root_platform_regulator_config_controller_0x130d0001_supplyName "vir-current-reg-hdf-adp"
#define root_platform_regulator_config_controller_0x130d0001_mode_exists 1
#define root_platform_regulator_config_controller_0x130d0001_mode 2
#define root_platform_regulator_config_controller_0x130d0001_minUv_exists 1
#define root_platform_regulator_config_controller_0x130d0001_minUv 0
#define root_platform_regulator_config_controller_0x130d0001_maxUv_exists 1
#define root_platform_regulator_config_controller_0x130d0001_maxUv 0
#define root_platform_regulator_config_controller_0x130d0001_minUa_exists 1
#define root_platform_regulator_config_controller_0x130d0001_minUa 1000
#define root_platform_regulator_config_controller_0x130d0001_maxUa_exists 1
#define root_platform_regulator_config_controller_0x130d0001_maxUa 50000

// hcs node macros: root/device_info
#define root_device_info_exists 1
#define root_device_info_nodeName "device_info"
#define root_device_info_foreach_child(func) \
    func(root_device_info_platform); \
    func(root_device_info_display); \
    func(root_device_info_input); \
    func(root_device_info_network); \
    func(root_device_info_sensor); \
    func(root_device_info_vibrator); \
    func(root_device_info_dsoftbus); \
    func(root_device_info_test_host);

#define root_device_info_foreach_child_vargs(func, ...) \
    func(root_device_info_platform, __VA_ARGS__); \
    func(root_device_info_display, __VA_ARGS__); \
    func(root_device_info_input, __VA_ARGS__); \
    func(root_device_info_network, __VA_ARGS__); \
    func(root_device_info_sensor, __VA_ARGS__); \
    func(root_device_info_vibrator, __VA_ARGS__); \
    func(root_device_info_dsoftbus, __VA_ARGS__); \
    func(root_device_info_test_host, __VA_ARGS__);

#define root_device_info_match_attr_exists 1
#define root_device_info_match_attr "hdf_manager"

// hcs node macros: root/device_info/platform
#define root_device_info_platform_exists 1
#define root_device_info_platform_nodeName "platform"
#define root_device_info_platform_foreach_child(func) \
    func(root_device_info_platform_device_gpio); \
    func(root_device_info_platform_device_watchdog); \
    func(root_device_info_platform_device_rtc); \
    func(root_device_info_platform_device_uart); \
    func(root_device_info_platform_device_i2c); \
    func(root_device_info_platform_device_adc); \
    func(root_device_info_platform_device_spi); \
    func(root_device_info_platform_device_sdio); \
    func(root_device_info_platform_device_emmc); \
    func(root_device_info_platform_device_pwm); \
    func(root_device_info_platform_device_mipi_dsi); \
    func(root_device_info_platform_device_regulator);

#define root_device_info_platform_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_gpio, __VA_ARGS__); \
    func(root_device_info_platform_device_watchdog, __VA_ARGS__); \
    func(root_device_info_platform_device_rtc, __VA_ARGS__); \
    func(root_device_info_platform_device_uart, __VA_ARGS__); \
    func(root_device_info_platform_device_i2c, __VA_ARGS__); \
    func(root_device_info_platform_device_adc, __VA_ARGS__); \
    func(root_device_info_platform_device_spi, __VA_ARGS__); \
    func(root_device_info_platform_device_sdio, __VA_ARGS__); \
    func(root_device_info_platform_device_emmc, __VA_ARGS__); \
    func(root_device_info_platform_device_pwm, __VA_ARGS__); \
    func(root_device_info_platform_device_mipi_dsi, __VA_ARGS__); \
    func(root_device_info_platform_device_regulator, __VA_ARGS__);

#define root_device_info_platform_hostName_exists 1
#define root_device_info_platform_hostName "platform_host"
#define root_device_info_platform_priority_exists 1
#define root_device_info_platform_priority 50

// hcs node macros: root/device_info/platform/device_gpio
#define root_device_info_platform_device_gpio_exists 1
#define root_device_info_platform_device_gpio_nodeName "device_gpio"
#define root_device_info_platform_device_gpio_foreach_child(func) \
    func(root_device_info_platform_device_gpio_device0); \
    func(root_device_info_platform_device_gpio_device1);

#define root_device_info_platform_device_gpio_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_gpio_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_gpio_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_gpio/device0
#define root_device_info_platform_device_gpio_device0_exists 1
#define root_device_info_platform_device_gpio_device0_nodeName "device0"
#define root_device_info_platform_device_gpio_device0_policy_exists 1
#define root_device_info_platform_device_gpio_device0_policy 2
#define root_device_info_platform_device_gpio_device0_priority_exists 1
#define root_device_info_platform_device_gpio_device0_priority 10
#define root_device_info_platform_device_gpio_device0_permission_exists 1
#define root_device_info_platform_device_gpio_device0_permission 420
#define root_device_info_platform_device_gpio_device0_moduleName_exists 1
#define root_device_info_platform_device_gpio_device0_moduleName "HDF_PLATFORM_GPIO_MANAGER"
#define root_device_info_platform_device_gpio_device0_serviceName_exists 1
#define root_device_info_platform_device_gpio_device0_serviceName "HDF_PLATFORM_GPIO_MANAGER"
#define root_device_info_platform_device_gpio_device0_preload_exists 1
#define root_device_info_platform_device_gpio_device0_preload 0
#define root_device_info_platform_device_gpio_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_gpio_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/platform/device_gpio/device1
#define root_device_info_platform_device_gpio_device1_exists 1
#define root_device_info_platform_device_gpio_device1_nodeName "device1"
#define root_device_info_platform_device_gpio_device1_policy_exists 1
#define root_device_info_platform_device_gpio_device1_policy 0
#define root_device_info_platform_device_gpio_device1_priority_exists 1
#define root_device_info_platform_device_gpio_device1_priority 10
#define root_device_info_platform_device_gpio_device1_permission_exists 1
#define root_device_info_platform_device_gpio_device1_permission 420
#define root_device_info_platform_device_gpio_device1_moduleName_exists 1
#define root_device_info_platform_device_gpio_device1_moduleName "linux_gpio_adapter"
#define root_device_info_platform_device_gpio_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_gpio_device1_deviceMatchAttr "linux_gpio_adapter"
#define root_device_info_platform_device_gpio_device1_preload_exists 1
#define root_device_info_platform_device_gpio_device1_preload 0
#define root_device_info_platform_device_gpio_device1_serviceName_exists 1
#define root_device_info_platform_device_gpio_device1_serviceName ""

// hcs node macros: root/device_info/platform/device_watchdog
#define root_device_info_platform_device_watchdog_exists 1
#define root_device_info_platform_device_watchdog_nodeName "device_watchdog"
#define root_device_info_platform_device_watchdog_foreach_child(func) \
    func(root_device_info_platform_device_watchdog_device0);

#define root_device_info_platform_device_watchdog_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_watchdog_device0, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_watchdog/device0
#define root_device_info_platform_device_watchdog_device0_exists 1
#define root_device_info_platform_device_watchdog_device0_nodeName "device0"
#define root_device_info_platform_device_watchdog_device0_policy_exists 1
#define root_device_info_platform_device_watchdog_device0_policy 2
#define root_device_info_platform_device_watchdog_device0_priority_exists 1
#define root_device_info_platform_device_watchdog_device0_priority 20
#define root_device_info_platform_device_watchdog_device0_permission_exists 1
#define root_device_info_platform_device_watchdog_device0_permission 420
#define root_device_info_platform_device_watchdog_device0_moduleName_exists 1
#define root_device_info_platform_device_watchdog_device0_moduleName "HDF_PLATFORM_WATCHDOG"
#define root_device_info_platform_device_watchdog_device0_serviceName_exists 1
#define root_device_info_platform_device_watchdog_device0_serviceName "HDF_PLATFORM_WATCHDOG_0"
#define root_device_info_platform_device_watchdog_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_watchdog_device0_deviceMatchAttr "hisilicon_hi35xx_watchdog_0"
#define root_device_info_platform_device_watchdog_device0_preload_exists 1
#define root_device_info_platform_device_watchdog_device0_preload 0

// hcs node macros: root/device_info/platform/device_rtc
#define root_device_info_platform_device_rtc_exists 1
#define root_device_info_platform_device_rtc_nodeName "device_rtc"
#define root_device_info_platform_device_rtc_foreach_child(func) \
    func(root_device_info_platform_device_rtc_device0);

#define root_device_info_platform_device_rtc_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_rtc_device0, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_rtc/device0
#define root_device_info_platform_device_rtc_device0_exists 1
#define root_device_info_platform_device_rtc_device0_nodeName "device0"
#define root_device_info_platform_device_rtc_device0_policy_exists 1
#define root_device_info_platform_device_rtc_device0_policy 2
#define root_device_info_platform_device_rtc_device0_priority_exists 1
#define root_device_info_platform_device_rtc_device0_priority 30
#define root_device_info_platform_device_rtc_device0_permission_exists 1
#define root_device_info_platform_device_rtc_device0_permission 420
#define root_device_info_platform_device_rtc_device0_moduleName_exists 1
#define root_device_info_platform_device_rtc_device0_moduleName "HDF_PLATFORM_RTC"
#define root_device_info_platform_device_rtc_device0_serviceName_exists 1
#define root_device_info_platform_device_rtc_device0_serviceName "HDF_PLATFORM_RTC"
#define root_device_info_platform_device_rtc_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_rtc_device0_deviceMatchAttr "hisilicon_hi35xx_rtc"
#define root_device_info_platform_device_rtc_device0_preload_exists 1
#define root_device_info_platform_device_rtc_device0_preload 0

// hcs node macros: root/device_info/platform/device_uart
#define root_device_info_platform_device_uart_exists 1
#define root_device_info_platform_device_uart_nodeName "device_uart"
#define root_device_info_platform_device_uart_foreach_child(func) \
    func(root_device_info_platform_device_uart_device0); \
    func(root_device_info_platform_device_uart_device1);

#define root_device_info_platform_device_uart_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_uart_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_uart_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_uart/device0
#define root_device_info_platform_device_uart_device0_exists 1
#define root_device_info_platform_device_uart_device0_nodeName "device0"
#define root_device_info_platform_device_uart_device0_policy_exists 1
#define root_device_info_platform_device_uart_device0_policy 1
#define root_device_info_platform_device_uart_device0_priority_exists 1
#define root_device_info_platform_device_uart_device0_priority 40
#define root_device_info_platform_device_uart_device0_permission_exists 1
#define root_device_info_platform_device_uart_device0_permission 420
#define root_device_info_platform_device_uart_device0_moduleName_exists 1
#define root_device_info_platform_device_uart_device0_moduleName "HDF_PLATFORM_UART"
#define root_device_info_platform_device_uart_device0_serviceName_exists 1
#define root_device_info_platform_device_uart_device0_serviceName "HDF_PLATFORM_UART_0"
#define root_device_info_platform_device_uart_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_uart_device0_deviceMatchAttr "hisilicon_hi35xx_uart_0"
#define root_device_info_platform_device_uart_device0_preload_exists 1
#define root_device_info_platform_device_uart_device0_preload 0

// hcs node macros: root/device_info/platform/device_uart/device1
#define root_device_info_platform_device_uart_device1_exists 1
#define root_device_info_platform_device_uart_device1_nodeName "device1"
#define root_device_info_platform_device_uart_device1_policy_exists 1
#define root_device_info_platform_device_uart_device1_policy 2
#define root_device_info_platform_device_uart_device1_permission_exists 1
#define root_device_info_platform_device_uart_device1_permission 420
#define root_device_info_platform_device_uart_device1_priority_exists 1
#define root_device_info_platform_device_uart_device1_priority 40
#define root_device_info_platform_device_uart_device1_moduleName_exists 1
#define root_device_info_platform_device_uart_device1_moduleName "HDF_PLATFORM_UART"
#define root_device_info_platform_device_uart_device1_serviceName_exists 1
#define root_device_info_platform_device_uart_device1_serviceName "HDF_PLATFORM_UART_1"
#define root_device_info_platform_device_uart_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_uart_device1_deviceMatchAttr "hisilicon_hi35xx_uart_1"
#define root_device_info_platform_device_uart_device1_preload_exists 1
#define root_device_info_platform_device_uart_device1_preload 0

// hcs node macros: root/device_info/platform/device_i2c
#define root_device_info_platform_device_i2c_exists 1
#define root_device_info_platform_device_i2c_nodeName "device_i2c"
#define root_device_info_platform_device_i2c_foreach_child(func) \
    func(root_device_info_platform_device_i2c_device0); \
    func(root_device_info_platform_device_i2c_device1);

#define root_device_info_platform_device_i2c_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_i2c_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_i2c_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_i2c/device0
#define root_device_info_platform_device_i2c_device0_exists 1
#define root_device_info_platform_device_i2c_device0_nodeName "device0"
#define root_device_info_platform_device_i2c_device0_policy_exists 1
#define root_device_info_platform_device_i2c_device0_policy 2
#define root_device_info_platform_device_i2c_device0_priority_exists 1
#define root_device_info_platform_device_i2c_device0_priority 50
#define root_device_info_platform_device_i2c_device0_permission_exists 1
#define root_device_info_platform_device_i2c_device0_permission 420
#define root_device_info_platform_device_i2c_device0_moduleName_exists 1
#define root_device_info_platform_device_i2c_device0_moduleName "HDF_PLATFORM_I2C_MANAGER"
#define root_device_info_platform_device_i2c_device0_serviceName_exists 1
#define root_device_info_platform_device_i2c_device0_serviceName "HDF_PLATFORM_I2C_MANAGER"
#define root_device_info_platform_device_i2c_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_i2c_device0_deviceMatchAttr "hdf_platform_i2c_manager"
#define root_device_info_platform_device_i2c_device0_preload_exists 1
#define root_device_info_platform_device_i2c_device0_preload 0

// hcs node macros: root/device_info/platform/device_i2c/device1
#define root_device_info_platform_device_i2c_device1_exists 1
#define root_device_info_platform_device_i2c_device1_nodeName "device1"
#define root_device_info_platform_device_i2c_device1_policy_exists 1
#define root_device_info_platform_device_i2c_device1_policy 0
#define root_device_info_platform_device_i2c_device1_priority_exists 1
#define root_device_info_platform_device_i2c_device1_priority 55
#define root_device_info_platform_device_i2c_device1_permission_exists 1
#define root_device_info_platform_device_i2c_device1_permission 420
#define root_device_info_platform_device_i2c_device1_moduleName_exists 1
#define root_device_info_platform_device_i2c_device1_moduleName "linux_i2c_adapter"
#define root_device_info_platform_device_i2c_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_i2c_device1_deviceMatchAttr "linux_i2c_adapter"
#define root_device_info_platform_device_i2c_device1_preload_exists 1
#define root_device_info_platform_device_i2c_device1_preload 0
#define root_device_info_platform_device_i2c_device1_serviceName_exists 1
#define root_device_info_platform_device_i2c_device1_serviceName ""

// hcs node macros: root/device_info/platform/device_adc
#define root_device_info_platform_device_adc_exists 1
#define root_device_info_platform_device_adc_nodeName "device_adc"
#define root_device_info_platform_device_adc_foreach_child(func) \
    func(root_device_info_platform_device_adc_device0); \
    func(root_device_info_platform_device_adc_device1);

#define root_device_info_platform_device_adc_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_adc_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_adc_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_adc/device0
#define root_device_info_platform_device_adc_device0_exists 1
#define root_device_info_platform_device_adc_device0_nodeName "device0"
#define root_device_info_platform_device_adc_device0_policy_exists 1
#define root_device_info_platform_device_adc_device0_policy 0
#define root_device_info_platform_device_adc_device0_priority_exists 1
#define root_device_info_platform_device_adc_device0_priority 65
#define root_device_info_platform_device_adc_device0_permission_exists 1
#define root_device_info_platform_device_adc_device0_permission 420
#define root_device_info_platform_device_adc_device0_moduleName_exists 1
#define root_device_info_platform_device_adc_device0_moduleName "linux_adc_adapter"
#define root_device_info_platform_device_adc_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_adc_device0_deviceMatchAttr "linux_adc_adapter_0"
#define root_device_info_platform_device_adc_device0_preload_exists 1
#define root_device_info_platform_device_adc_device0_preload 0
#define root_device_info_platform_device_adc_device0_serviceName_exists 1
#define root_device_info_platform_device_adc_device0_serviceName ""

// hcs node macros: root/device_info/platform/device_adc/device1
#define root_device_info_platform_device_adc_device1_exists 1
#define root_device_info_platform_device_adc_device1_nodeName "device1"
#define root_device_info_platform_device_adc_device1_policy_exists 1
#define root_device_info_platform_device_adc_device1_policy 2
#define root_device_info_platform_device_adc_device1_priority_exists 1
#define root_device_info_platform_device_adc_device1_priority 60
#define root_device_info_platform_device_adc_device1_permission_exists 1
#define root_device_info_platform_device_adc_device1_permission 420
#define root_device_info_platform_device_adc_device1_moduleName_exists 1
#define root_device_info_platform_device_adc_device1_moduleName "HDF_PLATFORM_ADC_MANAGER"
#define root_device_info_platform_device_adc_device1_serviceName_exists 1
#define root_device_info_platform_device_adc_device1_serviceName "HDF_PLATFORM_ADC_MANAGER"
#define root_device_info_platform_device_adc_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_adc_device1_deviceMatchAttr "hdf_platform_adc_manager"
#define root_device_info_platform_device_adc_device1_preload_exists 1
#define root_device_info_platform_device_adc_device1_preload 0

// hcs node macros: root/device_info/platform/device_spi
#define root_device_info_platform_device_spi_exists 1
#define root_device_info_platform_device_spi_nodeName "device_spi"
#define root_device_info_platform_device_spi_foreach_child(func) \
    func(root_device_info_platform_device_spi_device0); \
    func(root_device_info_platform_device_spi_device1); \
    func(root_device_info_platform_device_spi_device2);

#define root_device_info_platform_device_spi_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_spi_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_spi_device1, __VA_ARGS__); \
    func(root_device_info_platform_device_spi_device2, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_spi/device0
#define root_device_info_platform_device_spi_device0_exists 1
#define root_device_info_platform_device_spi_device0_nodeName "device0"
#define root_device_info_platform_device_spi_device0_policy_exists 1
#define root_device_info_platform_device_spi_device0_policy 2
#define root_device_info_platform_device_spi_device0_priority_exists 1
#define root_device_info_platform_device_spi_device0_priority 60
#define root_device_info_platform_device_spi_device0_permission_exists 1
#define root_device_info_platform_device_spi_device0_permission 420
#define root_device_info_platform_device_spi_device0_moduleName_exists 1
#define root_device_info_platform_device_spi_device0_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device0_serviceName_exists 1
#define root_device_info_platform_device_spi_device0_serviceName "HDF_PLATFORM_SPI_0"
#define root_device_info_platform_device_spi_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device0_deviceMatchAttr "hisilicon_hi35xx_spi_0"
#define root_device_info_platform_device_spi_device0_preload_exists 1
#define root_device_info_platform_device_spi_device0_preload 0

// hcs node macros: root/device_info/platform/device_spi/device1
#define root_device_info_platform_device_spi_device1_exists 1
#define root_device_info_platform_device_spi_device1_nodeName "device1"
#define root_device_info_platform_device_spi_device1_policy_exists 1
#define root_device_info_platform_device_spi_device1_policy 2
#define root_device_info_platform_device_spi_device1_priority_exists 1
#define root_device_info_platform_device_spi_device1_priority 60
#define root_device_info_platform_device_spi_device1_permission_exists 1
#define root_device_info_platform_device_spi_device1_permission 420
#define root_device_info_platform_device_spi_device1_moduleName_exists 1
#define root_device_info_platform_device_spi_device1_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device1_serviceName_exists 1
#define root_device_info_platform_device_spi_device1_serviceName "HDF_PLATFORM_SPI_1"
#define root_device_info_platform_device_spi_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device1_deviceMatchAttr "hisilicon_hi35xx_spi_1"
#define root_device_info_platform_device_spi_device1_preload_exists 1
#define root_device_info_platform_device_spi_device1_preload 0

// hcs node macros: root/device_info/platform/device_spi/device2
#define root_device_info_platform_device_spi_device2_exists 1
#define root_device_info_platform_device_spi_device2_nodeName "device2"
#define root_device_info_platform_device_spi_device2_policy_exists 1
#define root_device_info_platform_device_spi_device2_policy 2
#define root_device_info_platform_device_spi_device2_priority_exists 1
#define root_device_info_platform_device_spi_device2_priority 60
#define root_device_info_platform_device_spi_device2_permission_exists 1
#define root_device_info_platform_device_spi_device2_permission 420
#define root_device_info_platform_device_spi_device2_moduleName_exists 1
#define root_device_info_platform_device_spi_device2_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device2_serviceName_exists 1
#define root_device_info_platform_device_spi_device2_serviceName "HDF_PLATFORM_SPI_2"
#define root_device_info_platform_device_spi_device2_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device2_deviceMatchAttr "hisilicon_hi35xx_spi_2"
#define root_device_info_platform_device_spi_device2_preload_exists 1
#define root_device_info_platform_device_spi_device2_preload 0

// hcs node macros: root/device_info/platform/device_sdio
#define root_device_info_platform_device_sdio_exists 1
#define root_device_info_platform_device_sdio_nodeName "device_sdio"
#define root_device_info_platform_device_sdio_foreach_child(func) \
    func(root_device_info_platform_device_sdio_device0);

#define root_device_info_platform_device_sdio_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_sdio_device0, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_sdio/device0
#define root_device_info_platform_device_sdio_device0_exists 1
#define root_device_info_platform_device_sdio_device0_nodeName "device0"
#define root_device_info_platform_device_sdio_device0_policy_exists 1
#define root_device_info_platform_device_sdio_device0_policy 1
#define root_device_info_platform_device_sdio_device0_priority_exists 1
#define root_device_info_platform_device_sdio_device0_priority 20
#define root_device_info_platform_device_sdio_device0_permission_exists 1
#define root_device_info_platform_device_sdio_device0_permission 420
#define root_device_info_platform_device_sdio_device0_moduleName_exists 1
#define root_device_info_platform_device_sdio_device0_moduleName "HDF_PLATFORM_SDIO"
#define root_device_info_platform_device_sdio_device0_serviceName_exists 1
#define root_device_info_platform_device_sdio_device0_serviceName "HDF_PLATFORM_MMC_2"
#define root_device_info_platform_device_sdio_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_sdio_device0_deviceMatchAttr "hisilicon_hi35xx_sdio_0"
#define root_device_info_platform_device_sdio_device0_preload_exists 1
#define root_device_info_platform_device_sdio_device0_preload 0

// hcs node macros: root/device_info/platform/device_emmc
#define root_device_info_platform_device_emmc_exists 1
#define root_device_info_platform_device_emmc_nodeName "device_emmc"
#define root_device_info_platform_device_emmc_foreach_child(func) \
    func(root_device_info_platform_device_emmc_device0);

#define root_device_info_platform_device_emmc_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_emmc_device0, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_emmc/device0
#define root_device_info_platform_device_emmc_device0_exists 1
#define root_device_info_platform_device_emmc_device0_nodeName "device0"
#define root_device_info_platform_device_emmc_device0_policy_exists 1
#define root_device_info_platform_device_emmc_device0_policy 2
#define root_device_info_platform_device_emmc_device0_priority_exists 1
#define root_device_info_platform_device_emmc_device0_priority 20
#define root_device_info_platform_device_emmc_device0_permission_exists 1
#define root_device_info_platform_device_emmc_device0_permission 420
#define root_device_info_platform_device_emmc_device0_moduleName_exists 1
#define root_device_info_platform_device_emmc_device0_moduleName "HDF_PLATFORM_EMMC"
#define root_device_info_platform_device_emmc_device0_serviceName_exists 1
#define root_device_info_platform_device_emmc_device0_serviceName "HDF_PLATFORM_MMC_0"
#define root_device_info_platform_device_emmc_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_emmc_device0_deviceMatchAttr "hisilicon_hi35xx_emmc_0"
#define root_device_info_platform_device_emmc_device0_preload_exists 1
#define root_device_info_platform_device_emmc_device0_preload 0

// hcs node macros: root/device_info/platform/device_pwm
#define root_device_info_platform_device_pwm_exists 1
#define root_device_info_platform_device_pwm_nodeName "device_pwm"
#define root_device_info_platform_device_pwm_foreach_child(func) \
    func(root_device_info_platform_device_pwm_device0); \
    func(root_device_info_platform_device_pwm_device1);

#define root_device_info_platform_device_pwm_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_pwm_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_pwm_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_pwm/device0
#define root_device_info_platform_device_pwm_device0_exists 1
#define root_device_info_platform_device_pwm_device0_nodeName "device0"
#define root_device_info_platform_device_pwm_device0_policy_exists 1
#define root_device_info_platform_device_pwm_device0_policy 2
#define root_device_info_platform_device_pwm_device0_priority_exists 1
#define root_device_info_platform_device_pwm_device0_priority 80
#define root_device_info_platform_device_pwm_device0_permission_exists 1
#define root_device_info_platform_device_pwm_device0_permission 420
#define root_device_info_platform_device_pwm_device0_moduleName_exists 1
#define root_device_info_platform_device_pwm_device0_moduleName "HDF_PLATFORM_PWM"
#define root_device_info_platform_device_pwm_device0_serviceName_exists 1
#define root_device_info_platform_device_pwm_device0_serviceName "HDF_PLATFORM_PWM_0"
#define root_device_info_platform_device_pwm_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_pwm_device0_deviceMatchAttr "hisilicon_hi35xx_pwm_0"
#define root_device_info_platform_device_pwm_device0_preload_exists 1
#define root_device_info_platform_device_pwm_device0_preload 0

// hcs node macros: root/device_info/platform/device_pwm/device1
#define root_device_info_platform_device_pwm_device1_exists 1
#define root_device_info_platform_device_pwm_device1_nodeName "device1"
#define root_device_info_platform_device_pwm_device1_policy_exists 1
#define root_device_info_platform_device_pwm_device1_policy 2
#define root_device_info_platform_device_pwm_device1_priority_exists 1
#define root_device_info_platform_device_pwm_device1_priority 80
#define root_device_info_platform_device_pwm_device1_permission_exists 1
#define root_device_info_platform_device_pwm_device1_permission 420
#define root_device_info_platform_device_pwm_device1_moduleName_exists 1
#define root_device_info_platform_device_pwm_device1_moduleName "HDF_PLATFORM_PWM"
#define root_device_info_platform_device_pwm_device1_serviceName_exists 1
#define root_device_info_platform_device_pwm_device1_serviceName "HDF_PLATFORM_PWM_1"
#define root_device_info_platform_device_pwm_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_pwm_device1_deviceMatchAttr "hisilicon_hi35xx_pwm_1"
#define root_device_info_platform_device_pwm_device1_preload_exists 1
#define root_device_info_platform_device_pwm_device1_preload 0

// hcs node macros: root/device_info/platform/device_mipi_dsi
#define root_device_info_platform_device_mipi_dsi_exists 1
#define root_device_info_platform_device_mipi_dsi_nodeName "device_mipi_dsi"
#define root_device_info_platform_device_mipi_dsi_foreach_child(func) \
    func(root_device_info_platform_device_mipi_dsi_device0);

#define root_device_info_platform_device_mipi_dsi_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_mipi_dsi_device0, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_mipi_dsi/device0
#define root_device_info_platform_device_mipi_dsi_device0_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_nodeName "device0"
#define root_device_info_platform_device_mipi_dsi_device0_policy_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_policy 0
#define root_device_info_platform_device_mipi_dsi_device0_priority_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_priority 150
#define root_device_info_platform_device_mipi_dsi_device0_permission_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_permission 420
#define root_device_info_platform_device_mipi_dsi_device0_moduleName_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_moduleName "HDF_MIPI_TX"
#define root_device_info_platform_device_mipi_dsi_device0_serviceName_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_serviceName "HDF_MIPI_TX"
#define root_device_info_platform_device_mipi_dsi_device0_preload_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_preload 0
#define root_device_info_platform_device_mipi_dsi_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/platform/device_regulator
#define root_device_info_platform_device_regulator_exists 1
#define root_device_info_platform_device_regulator_nodeName "device_regulator"
#define root_device_info_platform_device_regulator_foreach_child(func) \
    func(root_device_info_platform_device_regulator_device0); \
    func(root_device_info_platform_device_regulator_device1);

#define root_device_info_platform_device_regulator_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_regulator_device0, __VA_ARGS__); \
    func(root_device_info_platform_device_regulator_device1, __VA_ARGS__);


// hcs node macros: root/device_info/platform/device_regulator/device0
#define root_device_info_platform_device_regulator_device0_exists 1
#define root_device_info_platform_device_regulator_device0_nodeName "device0"
#define root_device_info_platform_device_regulator_device0_policy_exists 1
#define root_device_info_platform_device_regulator_device0_policy 1
#define root_device_info_platform_device_regulator_device0_priority_exists 1
#define root_device_info_platform_device_regulator_device0_priority 50
#define root_device_info_platform_device_regulator_device0_permission_exists 1
#define root_device_info_platform_device_regulator_device0_permission 420
#define root_device_info_platform_device_regulator_device0_moduleName_exists 1
#define root_device_info_platform_device_regulator_device0_moduleName "HDF_PLATFORM_REGULATOR_MANAGER"
#define root_device_info_platform_device_regulator_device0_serviceName_exists 1
#define root_device_info_platform_device_regulator_device0_serviceName "HDF_PLATFORM_REGULATOR_MANAGER"
#define root_device_info_platform_device_regulator_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_regulator_device0_deviceMatchAttr "hdf_platform_regulator_manager"
#define root_device_info_platform_device_regulator_device0_preload_exists 1
#define root_device_info_platform_device_regulator_device0_preload 0

// hcs node macros: root/device_info/platform/device_regulator/device1
#define root_device_info_platform_device_regulator_device1_exists 1
#define root_device_info_platform_device_regulator_device1_nodeName "device1"
#define root_device_info_platform_device_regulator_device1_policy_exists 1
#define root_device_info_platform_device_regulator_device1_policy 0
#define root_device_info_platform_device_regulator_device1_priority_exists 1
#define root_device_info_platform_device_regulator_device1_priority 55
#define root_device_info_platform_device_regulator_device1_permission_exists 1
#define root_device_info_platform_device_regulator_device1_permission 420
#define root_device_info_platform_device_regulator_device1_moduleName_exists 1
#define root_device_info_platform_device_regulator_device1_moduleName "linux_regulator_adapter"
#define root_device_info_platform_device_regulator_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_regulator_device1_deviceMatchAttr "linux_regulator_adapter"
#define root_device_info_platform_device_regulator_device1_preload_exists 1
#define root_device_info_platform_device_regulator_device1_preload 0
#define root_device_info_platform_device_regulator_device1_serviceName_exists 1
#define root_device_info_platform_device_regulator_device1_serviceName ""

// hcs node macros: root/device_info/display
#define root_device_info_display_exists 1
#define root_device_info_display_nodeName "display"
#define root_device_info_display_foreach_child(func) \
    func(root_device_info_display_device_hdf_disp); \
    func(root_device_info_display_device_hi35xx_disp); \
    func(root_device_info_display_device_lcd); \
    func(root_device_info_display_device_pwm_bl); \
    func(root_device_info_display_device_backlight);

#define root_device_info_display_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hdf_disp, __VA_ARGS__); \
    func(root_device_info_display_device_hi35xx_disp, __VA_ARGS__); \
    func(root_device_info_display_device_lcd, __VA_ARGS__); \
    func(root_device_info_display_device_pwm_bl, __VA_ARGS__); \
    func(root_device_info_display_device_backlight, __VA_ARGS__);

#define root_device_info_display_hostName_exists 1
#define root_device_info_display_hostName "display_host"

// hcs node macros: root/device_info/display/device_hdf_disp
#define root_device_info_display_device_hdf_disp_exists 1
#define root_device_info_display_device_hdf_disp_nodeName "device_hdf_disp"
#define root_device_info_display_device_hdf_disp_foreach_child(func) \
    func(root_device_info_display_device_hdf_disp_device0);

#define root_device_info_display_device_hdf_disp_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hdf_disp_device0, __VA_ARGS__);


// hcs node macros: root/device_info/display/device_hdf_disp/device0
#define root_device_info_display_device_hdf_disp_device0_exists 1
#define root_device_info_display_device_hdf_disp_device0_nodeName "device0"
#define root_device_info_display_device_hdf_disp_device0_policy_exists 1
#define root_device_info_display_device_hdf_disp_device0_policy 2
#define root_device_info_display_device_hdf_disp_device0_priority_exists 1
#define root_device_info_display_device_hdf_disp_device0_priority 140
#define root_device_info_display_device_hdf_disp_device0_permission_exists 1
#define root_device_info_display_device_hdf_disp_device0_permission 432
#define root_device_info_display_device_hdf_disp_device0_moduleName_exists 1
#define root_device_info_display_device_hdf_disp_device0_moduleName "HDF_DISP"
#define root_device_info_display_device_hdf_disp_device0_serviceName_exists 1
#define root_device_info_display_device_hdf_disp_device0_serviceName "hdf_disp"
#define root_device_info_display_device_hdf_disp_device0_preload_exists 1
#define root_device_info_display_device_hdf_disp_device0_preload 0
#define root_device_info_display_device_hdf_disp_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_hdf_disp_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_hi35xx_disp
#define root_device_info_display_device_hi35xx_disp_exists 1
#define root_device_info_display_device_hi35xx_disp_nodeName "device_hi35xx_disp"
#define root_device_info_display_device_hi35xx_disp_foreach_child(func) \
    func(root_device_info_display_device_hi35xx_disp_device0);

#define root_device_info_display_device_hi35xx_disp_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hi35xx_disp_device0, __VA_ARGS__);


// hcs node macros: root/device_info/display/device_hi35xx_disp/device0
#define root_device_info_display_device_hi35xx_disp_device0_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_nodeName "device0"
#define root_device_info_display_device_hi35xx_disp_device0_policy_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_policy 0
#define root_device_info_display_device_hi35xx_disp_device0_priority_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_priority 120
#define root_device_info_display_device_hi35xx_disp_device0_moduleName_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_moduleName "HI351XX_DISP"
#define root_device_info_display_device_hi35xx_disp_device0_preload_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_preload 0
#define root_device_info_display_device_hi35xx_disp_device0_permission_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_permission 436
#define root_device_info_display_device_hi35xx_disp_device0_serviceName_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_serviceName ""
#define root_device_info_display_device_hi35xx_disp_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_lcd
#define root_device_info_display_device_lcd_exists 1
#define root_device_info_display_device_lcd_nodeName "device_lcd"
#define root_device_info_display_device_lcd_foreach_child(func) \
    func(root_device_info_display_device_lcd_device0);

#define root_device_info_display_device_lcd_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_lcd_device0, __VA_ARGS__);


// hcs node macros: root/device_info/display/device_lcd/device0
#define root_device_info_display_device_lcd_device0_exists 1
#define root_device_info_display_device_lcd_device0_nodeName "device0"
#define root_device_info_display_device_lcd_device0_policy_exists 1
#define root_device_info_display_device_lcd_device0_policy 0
#define root_device_info_display_device_lcd_device0_priority_exists 1
#define root_device_info_display_device_lcd_device0_priority 100
#define root_device_info_display_device_lcd_device0_preload_exists 1
#define root_device_info_display_device_lcd_device0_preload 0
#define root_device_info_display_device_lcd_device0_moduleName_exists 1
#define root_device_info_display_device_lcd_device0_moduleName "LCD_ICN9700"
#define root_device_info_display_device_lcd_device0_permission_exists 1
#define root_device_info_display_device_lcd_device0_permission 436
#define root_device_info_display_device_lcd_device0_serviceName_exists 1
#define root_device_info_display_device_lcd_device0_serviceName ""
#define root_device_info_display_device_lcd_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_lcd_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_pwm_bl
#define root_device_info_display_device_pwm_bl_exists 1
#define root_device_info_display_device_pwm_bl_nodeName "device_pwm_bl"
#define root_device_info_display_device_pwm_bl_foreach_child(func) \
    func(root_device_info_display_device_pwm_bl_device0);

#define root_device_info_display_device_pwm_bl_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_pwm_bl_device0, __VA_ARGS__);


// hcs node macros: root/device_info/display/device_pwm_bl/device0
#define root_device_info_display_device_pwm_bl_device0_exists 1
#define root_device_info_display_device_pwm_bl_device0_nodeName "device0"
#define root_device_info_display_device_pwm_bl_device0_policy_exists 1
#define root_device_info_display_device_pwm_bl_device0_policy 0
#define root_device_info_display_device_pwm_bl_device0_priority_exists 1
#define root_device_info_display_device_pwm_bl_device0_priority 95
#define root_device_info_display_device_pwm_bl_device0_preload_exists 1
#define root_device_info_display_device_pwm_bl_device0_preload 0
#define root_device_info_display_device_pwm_bl_device0_moduleName_exists 1
#define root_device_info_display_device_pwm_bl_device0_moduleName "PWM_BL"
#define root_device_info_display_device_pwm_bl_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_pwm_bl_device0_deviceMatchAttr "pwm_bl_dev"
#define root_device_info_display_device_pwm_bl_device0_permission_exists 1
#define root_device_info_display_device_pwm_bl_device0_permission 436
#define root_device_info_display_device_pwm_bl_device0_serviceName_exists 1
#define root_device_info_display_device_pwm_bl_device0_serviceName ""

// hcs node macros: root/device_info/display/device_backlight
#define root_device_info_display_device_backlight_exists 1
#define root_device_info_display_device_backlight_nodeName "device_backlight"
#define root_device_info_display_device_backlight_foreach_child(func) \
    func(root_device_info_display_device_backlight_device0);

#define root_device_info_display_device_backlight_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_backlight_device0, __VA_ARGS__);


// hcs node macros: root/device_info/display/device_backlight/device0
#define root_device_info_display_device_backlight_device0_exists 1
#define root_device_info_display_device_backlight_device0_nodeName "device0"
#define root_device_info_display_device_backlight_device0_policy_exists 1
#define root_device_info_display_device_backlight_device0_policy 2
#define root_device_info_display_device_backlight_device0_priority_exists 1
#define root_device_info_display_device_backlight_device0_priority 90
#define root_device_info_display_device_backlight_device0_preload_exists 1
#define root_device_info_display_device_backlight_device0_preload 0
#define root_device_info_display_device_backlight_device0_permission_exists 1
#define root_device_info_display_device_backlight_device0_permission 432
#define root_device_info_display_device_backlight_device0_moduleName_exists 1
#define root_device_info_display_device_backlight_device0_moduleName "HDF_BL"
#define root_device_info_display_device_backlight_device0_serviceName_exists 1
#define root_device_info_display_device_backlight_device0_serviceName "hdf_bl"
#define root_device_info_display_device_backlight_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_backlight_device0_deviceMatchAttr ""
#define root_device_info_display_priority_exists 1
#define root_device_info_display_priority 100

// hcs node macros: root/device_info/input
#define root_device_info_input_exists 1
#define root_device_info_input_nodeName "input"
#define root_device_info_input_foreach_child(func) \
    func(root_device_info_input_device_input_manager); \
    func(root_device_info_input_device_hdf_touch); \
    func(root_device_info_input_device_hdf_hid);

#define root_device_info_input_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_input_manager, __VA_ARGS__); \
    func(root_device_info_input_device_hdf_touch, __VA_ARGS__); \
    func(root_device_info_input_device_hdf_hid, __VA_ARGS__);

#define root_device_info_input_hostName_exists 1
#define root_device_info_input_hostName "input_host"
#define root_device_info_input_priority_exists 1
#define root_device_info_input_priority 100

// hcs node macros: root/device_info/input/device_input_manager
#define root_device_info_input_device_input_manager_exists 1
#define root_device_info_input_device_input_manager_nodeName "device_input_manager"
#define root_device_info_input_device_input_manager_foreach_child(func) \
    func(root_device_info_input_device_input_manager_device0);

#define root_device_info_input_device_input_manager_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_input_manager_device0, __VA_ARGS__);


// hcs node macros: root/device_info/input/device_input_manager/device0
#define root_device_info_input_device_input_manager_device0_exists 1
#define root_device_info_input_device_input_manager_device0_nodeName "device0"
#define root_device_info_input_device_input_manager_device0_policy_exists 1
#define root_device_info_input_device_input_manager_device0_policy 2
#define root_device_info_input_device_input_manager_device0_priority_exists 1
#define root_device_info_input_device_input_manager_device0_priority 100
#define root_device_info_input_device_input_manager_device0_preload_exists 1
#define root_device_info_input_device_input_manager_device0_preload 0
#define root_device_info_input_device_input_manager_device0_permission_exists 1
#define root_device_info_input_device_input_manager_device0_permission 432
#define root_device_info_input_device_input_manager_device0_moduleName_exists 1
#define root_device_info_input_device_input_manager_device0_moduleName "HDF_INPUT_MANAGER"
#define root_device_info_input_device_input_manager_device0_serviceName_exists 1
#define root_device_info_input_device_input_manager_device0_serviceName "hdf_input_host"
#define root_device_info_input_device_input_manager_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_input_manager_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/input/device_hdf_touch
#define root_device_info_input_device_hdf_touch_exists 1
#define root_device_info_input_device_hdf_touch_nodeName "device_hdf_touch"
#define root_device_info_input_device_hdf_touch_foreach_child(func) \
    func(root_device_info_input_device_hdf_touch_device0); \
    func(root_device_info_input_device_hdf_touch_device1);

#define root_device_info_input_device_hdf_touch_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_hdf_touch_device0, __VA_ARGS__); \
    func(root_device_info_input_device_hdf_touch_device1, __VA_ARGS__);


// hcs node macros: root/device_info/input/device_hdf_touch/device0
#define root_device_info_input_device_hdf_touch_device0_exists 1
#define root_device_info_input_device_hdf_touch_device0_nodeName "device0"
#define root_device_info_input_device_hdf_touch_device0_policy_exists 1
#define root_device_info_input_device_hdf_touch_device0_policy 2
#define root_device_info_input_device_hdf_touch_device0_priority_exists 1
#define root_device_info_input_device_hdf_touch_device0_priority 120
#define root_device_info_input_device_hdf_touch_device0_preload_exists 1
#define root_device_info_input_device_hdf_touch_device0_preload 0
#define root_device_info_input_device_hdf_touch_device0_permission_exists 1
#define root_device_info_input_device_hdf_touch_device0_permission 432
#define root_device_info_input_device_hdf_touch_device0_moduleName_exists 1
#define root_device_info_input_device_hdf_touch_device0_moduleName "HDF_TOUCH"
#define root_device_info_input_device_hdf_touch_device0_serviceName_exists 1
#define root_device_info_input_device_hdf_touch_device0_serviceName "hdf_input_event1"
#define root_device_info_input_device_hdf_touch_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_touch_device0_deviceMatchAttr "touch_device1"

// hcs node macros: root/device_info/input/device_hdf_touch/device1
#define root_device_info_input_device_hdf_touch_device1_exists 1
#define root_device_info_input_device_hdf_touch_device1_nodeName "device1"
#define root_device_info_input_device_hdf_touch_device1_policy_exists 1
#define root_device_info_input_device_hdf_touch_device1_policy 0
#define root_device_info_input_device_hdf_touch_device1_priority_exists 1
#define root_device_info_input_device_hdf_touch_device1_priority 130
#define root_device_info_input_device_hdf_touch_device1_preload_exists 1
#define root_device_info_input_device_hdf_touch_device1_preload 0
#define root_device_info_input_device_hdf_touch_device1_permission_exists 1
#define root_device_info_input_device_hdf_touch_device1_permission 432
#define root_device_info_input_device_hdf_touch_device1_moduleName_exists 1
#define root_device_info_input_device_hdf_touch_device1_moduleName "HDF_TOUCH_GT911"
#define root_device_info_input_device_hdf_touch_device1_serviceName_exists 1
#define root_device_info_input_device_hdf_touch_device1_serviceName "hdf_touch_gt911_service"
#define root_device_info_input_device_hdf_touch_device1_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_touch_device1_deviceMatchAttr "zsj_gt911_5p5"

// hcs node macros: root/device_info/input/device_hdf_hid
#define root_device_info_input_device_hdf_hid_exists 1
#define root_device_info_input_device_hdf_hid_nodeName "device_hdf_hid"
#define root_device_info_input_device_hdf_hid_foreach_child(func) \
    func(root_device_info_input_device_hdf_hid_device0);

#define root_device_info_input_device_hdf_hid_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_hdf_hid_device0, __VA_ARGS__);


// hcs node macros: root/device_info/input/device_hdf_hid/device0
#define root_device_info_input_device_hdf_hid_device0_exists 1
#define root_device_info_input_device_hdf_hid_device0_nodeName "device0"
#define root_device_info_input_device_hdf_hid_device0_policy_exists 1
#define root_device_info_input_device_hdf_hid_device0_policy 2
#define root_device_info_input_device_hdf_hid_device0_priority_exists 1
#define root_device_info_input_device_hdf_hid_device0_priority 200
#define root_device_info_input_device_hdf_hid_device0_preload_exists 1
#define root_device_info_input_device_hdf_hid_device0_preload 0
#define root_device_info_input_device_hdf_hid_device0_permission_exists 1
#define root_device_info_input_device_hdf_hid_device0_permission 432
#define root_device_info_input_device_hdf_hid_device0_moduleName_exists 1
#define root_device_info_input_device_hdf_hid_device0_moduleName "HDF_HID"
#define root_device_info_input_device_hdf_hid_device0_serviceName_exists 1
#define root_device_info_input_device_hdf_hid_device0_serviceName ""
#define root_device_info_input_device_hdf_hid_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_hid_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/network
#define root_device_info_network_exists 1
#define root_device_info_network_nodeName "network"
#define root_device_info_network_foreach_child(func) \
    func(root_device_info_network_device_wifi); \
    func(root_device_info_network_device_wlan_chips);

#define root_device_info_network_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wifi, __VA_ARGS__); \
    func(root_device_info_network_device_wlan_chips, __VA_ARGS__);

#define root_device_info_network_hostName_exists 1
#define root_device_info_network_hostName "network_host"

// hcs node macros: root/device_info/network/device_wifi
#define root_device_info_network_device_wifi_exists 1
#define root_device_info_network_device_wifi_nodeName "device_wifi"
#define root_device_info_network_device_wifi_foreach_child(func) \
    func(root_device_info_network_device_wifi_device0);

#define root_device_info_network_device_wifi_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wifi_device0, __VA_ARGS__);


// hcs node macros: root/device_info/network/device_wifi/device0
#define root_device_info_network_device_wifi_device0_exists 1
#define root_device_info_network_device_wifi_device0_nodeName "device0"
#define root_device_info_network_device_wifi_device0_policy_exists 1
#define root_device_info_network_device_wifi_device0_policy 2
#define root_device_info_network_device_wifi_device0_priority_exists 1
#define root_device_info_network_device_wifi_device0_priority 100
#define root_device_info_network_device_wifi_device0_preload_exists 1
#define root_device_info_network_device_wifi_device0_preload 0
#define root_device_info_network_device_wifi_device0_permission_exists 1
#define root_device_info_network_device_wifi_device0_permission 436
#define root_device_info_network_device_wifi_device0_moduleName_exists 1
#define root_device_info_network_device_wifi_device0_moduleName "HDF_WIFI"
#define root_device_info_network_device_wifi_device0_deviceMatchAttr_exists 1
#define root_device_info_network_device_wifi_device0_deviceMatchAttr "hdf_wlan_driver"
#define root_device_info_network_device_wifi_device0_serviceName_exists 1
#define root_device_info_network_device_wifi_device0_serviceName "hdfwifi"

// hcs node macros: root/device_info/network/device_wlan_chips
#define root_device_info_network_device_wlan_chips_exists 1
#define root_device_info_network_device_wlan_chips_nodeName "device_wlan_chips"
#define root_device_info_network_device_wlan_chips_foreach_child(func) \
    func(root_device_info_network_device_wlan_chips_device0);

#define root_device_info_network_device_wlan_chips_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wlan_chips_device0, __VA_ARGS__);


// hcs node macros: root/device_info/network/device_wlan_chips/device0
#define root_device_info_network_device_wlan_chips_device0_exists 1
#define root_device_info_network_device_wlan_chips_device0_nodeName "device0"
#define root_device_info_network_device_wlan_chips_device0_policy_exists 1
#define root_device_info_network_device_wlan_chips_device0_policy 0
#define root_device_info_network_device_wlan_chips_device0_preload_exists 1
#define root_device_info_network_device_wlan_chips_device0_preload 2
#define root_device_info_network_device_wlan_chips_device0_moduleName_exists 1
#define root_device_info_network_device_wlan_chips_device0_moduleName "HDF_WLAN_CHIPS"
#define root_device_info_network_device_wlan_chips_device0_deviceMatchAttr_exists 1
#define root_device_info_network_device_wlan_chips_device0_deviceMatchAttr "hdf_wlan_chips_hi3881"
#define root_device_info_network_device_wlan_chips_device0_serviceName_exists 1
#define root_device_info_network_device_wlan_chips_device0_serviceName "hisi"
#define root_device_info_network_device_wlan_chips_device0_priority_exists 1
#define root_device_info_network_device_wlan_chips_device0_priority 100
#define root_device_info_network_device_wlan_chips_device0_permission_exists 1
#define root_device_info_network_device_wlan_chips_device0_permission 436
#define root_device_info_network_priority_exists 1
#define root_device_info_network_priority 100

// hcs node macros: root/device_info/sensor
#define root_device_info_sensor_exists 1
#define root_device_info_sensor_nodeName "sensor"
#define root_device_info_sensor_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_manager); \
    func(root_device_info_sensor_device_sensor_accel); \
    func(root_device_info_sensor_device_sensor_bmi160); \
    func(root_device_info_sensor_device_sensor_als); \
    func(root_device_info_sensor_device_sensor_bh1745); \
    func(root_device_info_sensor_device_sensor_gyro); \
    func(root_device_info_sensor_device_sensor_gyro_bmi160); \
    func(root_device_info_sensor_device_sensor_barometer); \
    func(root_device_info_sensor_device_sensor_bmp180); \
    func(root_device_info_sensor_device_sensor_hall); \
    func(root_device_info_sensor_device_sensor_ak8789); \
    func(root_device_info_sensor_device_sensor_magnetic); \
    func(root_device_info_sensor_device_sensor_lsm303); \
    func(root_device_info_sensor_device_sensor_proximity); \
    func(root_device_info_sensor_device_sensor_apds9960);

#define root_device_info_sensor_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_manager, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_accel, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_bmi160, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_als, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_bh1745, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_gyro, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_gyro_bmi160, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_barometer, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_bmp180, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_hall, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_ak8789, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_magnetic, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_lsm303, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_proximity, __VA_ARGS__); \
    func(root_device_info_sensor_device_sensor_apds9960, __VA_ARGS__);

#define root_device_info_sensor_hostName_exists 1
#define root_device_info_sensor_hostName "sensor_host"

// hcs node macros: root/device_info/sensor/device_sensor_manager
#define root_device_info_sensor_device_sensor_manager_exists 1
#define root_device_info_sensor_device_sensor_manager_nodeName "device_sensor_manager"
#define root_device_info_sensor_device_sensor_manager_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_manager_device0);

#define root_device_info_sensor_device_sensor_manager_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_manager_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_manager/device0
#define root_device_info_sensor_device_sensor_manager_device0_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_manager_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_policy 2
#define root_device_info_sensor_device_sensor_manager_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_priority 100
#define root_device_info_sensor_device_sensor_manager_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_preload 0
#define root_device_info_sensor_device_sensor_manager_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_permission 436
#define root_device_info_sensor_device_sensor_manager_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_moduleName "HDF_SENSOR_MGR_AP"
#define root_device_info_sensor_device_sensor_manager_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_serviceName "hdf_sensor_manager_ap"
#define root_device_info_sensor_device_sensor_manager_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/sensor/device_sensor_accel
#define root_device_info_sensor_device_sensor_accel_exists 1
#define root_device_info_sensor_device_sensor_accel_nodeName "device_sensor_accel"
#define root_device_info_sensor_device_sensor_accel_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_accel_device0);

#define root_device_info_sensor_device_sensor_accel_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_accel_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_accel/device0
#define root_device_info_sensor_device_sensor_accel_device0_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_accel_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_policy 1
#define root_device_info_sensor_device_sensor_accel_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_priority 110
#define root_device_info_sensor_device_sensor_accel_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_preload 2
#define root_device_info_sensor_device_sensor_accel_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_permission 436
#define root_device_info_sensor_device_sensor_accel_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_moduleName "HDF_SENSOR_ACCEL"
#define root_device_info_sensor_device_sensor_accel_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_serviceName "hdf_sensor_accel"
#define root_device_info_sensor_device_sensor_accel_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_deviceMatchAttr "hdf_sensor_accel_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bmi160
#define root_device_info_sensor_device_sensor_bmi160_exists 1
#define root_device_info_sensor_device_sensor_bmi160_nodeName "device_sensor_bmi160"
#define root_device_info_sensor_device_sensor_bmi160_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bmi160_device0);

#define root_device_info_sensor_device_sensor_bmi160_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bmi160_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_bmi160/device0
#define root_device_info_sensor_device_sensor_bmi160_device0_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_bmi160_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_policy 1
#define root_device_info_sensor_device_sensor_bmi160_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_priority 120
#define root_device_info_sensor_device_sensor_bmi160_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_preload 2
#define root_device_info_sensor_device_sensor_bmi160_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_permission 436
#define root_device_info_sensor_device_sensor_bmi160_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_moduleName "HDF_SENSOR_ACCEL_BMI160"
#define root_device_info_sensor_device_sensor_bmi160_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_serviceName "hdf_accel_bmi160"
#define root_device_info_sensor_device_sensor_bmi160_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_deviceMatchAttr "hdf_sensor_accel_bmi160_driver"

// hcs node macros: root/device_info/sensor/device_sensor_als
#define root_device_info_sensor_device_sensor_als_exists 1
#define root_device_info_sensor_device_sensor_als_nodeName "device_sensor_als"
#define root_device_info_sensor_device_sensor_als_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_als_device0);

#define root_device_info_sensor_device_sensor_als_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_als_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_als/device0
#define root_device_info_sensor_device_sensor_als_device0_exists 1
#define root_device_info_sensor_device_sensor_als_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_als_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_als_device0_policy 1
#define root_device_info_sensor_device_sensor_als_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_als_device0_priority 110
#define root_device_info_sensor_device_sensor_als_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_als_device0_preload 2
#define root_device_info_sensor_device_sensor_als_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_als_device0_permission 436
#define root_device_info_sensor_device_sensor_als_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_als_device0_moduleName "HDF_SENSOR_ALS"
#define root_device_info_sensor_device_sensor_als_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_als_device0_serviceName "hdf_sensor_als"
#define root_device_info_sensor_device_sensor_als_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_als_device0_deviceMatchAttr "hdf_sensor_als_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bh1745
#define root_device_info_sensor_device_sensor_bh1745_exists 1
#define root_device_info_sensor_device_sensor_bh1745_nodeName "device_sensor_bh1745"
#define root_device_info_sensor_device_sensor_bh1745_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bh1745_device0);

#define root_device_info_sensor_device_sensor_bh1745_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bh1745_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_bh1745/device0
#define root_device_info_sensor_device_sensor_bh1745_device0_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_bh1745_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_policy 1
#define root_device_info_sensor_device_sensor_bh1745_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_priority 120
#define root_device_info_sensor_device_sensor_bh1745_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_preload 2
#define root_device_info_sensor_device_sensor_bh1745_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_permission 436
#define root_device_info_sensor_device_sensor_bh1745_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_moduleName "HDF_SENSOR_ALS_BH1745"
#define root_device_info_sensor_device_sensor_bh1745_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_serviceName "hdf_als_bh1745"
#define root_device_info_sensor_device_sensor_bh1745_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_deviceMatchAttr "hdf_sensor_als_bh1745_driver"

// hcs node macros: root/device_info/sensor/device_sensor_gyro
#define root_device_info_sensor_device_sensor_gyro_exists 1
#define root_device_info_sensor_device_sensor_gyro_nodeName "device_sensor_gyro"
#define root_device_info_sensor_device_sensor_gyro_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_gyro_device0);

#define root_device_info_sensor_device_sensor_gyro_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_gyro_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_gyro/device0
#define root_device_info_sensor_device_sensor_gyro_device0_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_gyro_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_policy 1
#define root_device_info_sensor_device_sensor_gyro_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_priority 110
#define root_device_info_sensor_device_sensor_gyro_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_preload 2
#define root_device_info_sensor_device_sensor_gyro_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_permission 436
#define root_device_info_sensor_device_sensor_gyro_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_moduleName "HDF_SENSOR_GYRO"
#define root_device_info_sensor_device_sensor_gyro_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_serviceName "hdf_sensor_gyro"
#define root_device_info_sensor_device_sensor_gyro_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_deviceMatchAttr "hdf_sensor_gyro_driver"

// hcs node macros: root/device_info/sensor/device_sensor_gyro_bmi160
#define root_device_info_sensor_device_sensor_gyro_bmi160_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_nodeName "device_sensor_gyro_bmi160"
#define root_device_info_sensor_device_sensor_gyro_bmi160_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160_device0);

#define root_device_info_sensor_device_sensor_gyro_bmi160_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_gyro_bmi160/device0
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_policy 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_priority 120
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_preload 2
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_permission 436
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_moduleName "HDF_SENSOR_GYRO_BMI160"
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_serviceName "hdf_gyro_bmi160"
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_deviceMatchAttr "hdf_sensor_gyro_bmi160_driver"

// hcs node macros: root/device_info/sensor/device_sensor_barometer
#define root_device_info_sensor_device_sensor_barometer_exists 1
#define root_device_info_sensor_device_sensor_barometer_nodeName "device_sensor_barometer"
#define root_device_info_sensor_device_sensor_barometer_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_barometer_device0);

#define root_device_info_sensor_device_sensor_barometer_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_barometer_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_barometer/device0
#define root_device_info_sensor_device_sensor_barometer_device0_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_barometer_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_policy 1
#define root_device_info_sensor_device_sensor_barometer_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_priority 110
#define root_device_info_sensor_device_sensor_barometer_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_preload 2
#define root_device_info_sensor_device_sensor_barometer_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_permission 436
#define root_device_info_sensor_device_sensor_barometer_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_moduleName "HDF_SENSOR_BAROMETER"
#define root_device_info_sensor_device_sensor_barometer_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_serviceName "hdf_sensor_barometer"
#define root_device_info_sensor_device_sensor_barometer_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_deviceMatchAttr "hdf_sensor_barometer_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bmp180
#define root_device_info_sensor_device_sensor_bmp180_exists 1
#define root_device_info_sensor_device_sensor_bmp180_nodeName "device_sensor_bmp180"
#define root_device_info_sensor_device_sensor_bmp180_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bmp180_device0);

#define root_device_info_sensor_device_sensor_bmp180_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bmp180_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_bmp180/device0
#define root_device_info_sensor_device_sensor_bmp180_device0_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_bmp180_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_policy 1
#define root_device_info_sensor_device_sensor_bmp180_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_priority 120
#define root_device_info_sensor_device_sensor_bmp180_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_preload 2
#define root_device_info_sensor_device_sensor_bmp180_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_permission 436
#define root_device_info_sensor_device_sensor_bmp180_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_moduleName "HDF_SENSOR_BAROMETER_BMP180"
#define root_device_info_sensor_device_sensor_bmp180_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_serviceName "hdf_barometer_bmp180"
#define root_device_info_sensor_device_sensor_bmp180_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_deviceMatchAttr "hdf_sensor_barometer_bmp180_driver"

// hcs node macros: root/device_info/sensor/device_sensor_hall
#define root_device_info_sensor_device_sensor_hall_exists 1
#define root_device_info_sensor_device_sensor_hall_nodeName "device_sensor_hall"
#define root_device_info_sensor_device_sensor_hall_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_hall_device0);

#define root_device_info_sensor_device_sensor_hall_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_hall_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_hall/device0
#define root_device_info_sensor_device_sensor_hall_device0_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_hall_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_policy 1
#define root_device_info_sensor_device_sensor_hall_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_priority 110
#define root_device_info_sensor_device_sensor_hall_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_preload 2
#define root_device_info_sensor_device_sensor_hall_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_permission 436
#define root_device_info_sensor_device_sensor_hall_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_moduleName "HDF_SENSOR_HALL"
#define root_device_info_sensor_device_sensor_hall_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_serviceName "hdf_sensor_hall"
#define root_device_info_sensor_device_sensor_hall_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_deviceMatchAttr "hdf_sensor_hall_driver"

// hcs node macros: root/device_info/sensor/device_sensor_ak8789
#define root_device_info_sensor_device_sensor_ak8789_exists 1
#define root_device_info_sensor_device_sensor_ak8789_nodeName "device_sensor_ak8789"
#define root_device_info_sensor_device_sensor_ak8789_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_ak8789_device0);

#define root_device_info_sensor_device_sensor_ak8789_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_ak8789_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_ak8789/device0
#define root_device_info_sensor_device_sensor_ak8789_device0_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_ak8789_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_policy 1
#define root_device_info_sensor_device_sensor_ak8789_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_priority 120
#define root_device_info_sensor_device_sensor_ak8789_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_preload 2
#define root_device_info_sensor_device_sensor_ak8789_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_permission 436
#define root_device_info_sensor_device_sensor_ak8789_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_moduleName "HDF_SENSOR_HALL_AK8789"
#define root_device_info_sensor_device_sensor_ak8789_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_serviceName "hdf_hall_ak8789"
#define root_device_info_sensor_device_sensor_ak8789_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_deviceMatchAttr "hdf_sensor_hall_ak8789_driver"

// hcs node macros: root/device_info/sensor/device_sensor_magnetic
#define root_device_info_sensor_device_sensor_magnetic_exists 1
#define root_device_info_sensor_device_sensor_magnetic_nodeName "device_sensor_magnetic"
#define root_device_info_sensor_device_sensor_magnetic_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_magnetic_device0);

#define root_device_info_sensor_device_sensor_magnetic_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_magnetic_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_magnetic/device0
#define root_device_info_sensor_device_sensor_magnetic_device0_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_magnetic_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_policy 1
#define root_device_info_sensor_device_sensor_magnetic_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_priority 110
#define root_device_info_sensor_device_sensor_magnetic_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_preload 2
#define root_device_info_sensor_device_sensor_magnetic_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_permission 436
#define root_device_info_sensor_device_sensor_magnetic_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_moduleName "HDF_SENSOR_MAGNETIC"
#define root_device_info_sensor_device_sensor_magnetic_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_serviceName "hdf_sensor_magnetic"
#define root_device_info_sensor_device_sensor_magnetic_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_deviceMatchAttr "hdf_sensor_magnetic_driver"

// hcs node macros: root/device_info/sensor/device_sensor_lsm303
#define root_device_info_sensor_device_sensor_lsm303_exists 1
#define root_device_info_sensor_device_sensor_lsm303_nodeName "device_sensor_lsm303"
#define root_device_info_sensor_device_sensor_lsm303_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_lsm303_device0);

#define root_device_info_sensor_device_sensor_lsm303_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_lsm303_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_lsm303/device0
#define root_device_info_sensor_device_sensor_lsm303_device0_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_lsm303_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_policy 1
#define root_device_info_sensor_device_sensor_lsm303_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_priority 120
#define root_device_info_sensor_device_sensor_lsm303_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_preload 2
#define root_device_info_sensor_device_sensor_lsm303_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_permission 436
#define root_device_info_sensor_device_sensor_lsm303_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_moduleName "HDF_SENSOR_MAGNETIC_LSM303"
#define root_device_info_sensor_device_sensor_lsm303_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_serviceName "hdf_magnetic_lsm303"
#define root_device_info_sensor_device_sensor_lsm303_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_deviceMatchAttr "hdf_sensor_magnetic_lsm303_driver"

// hcs node macros: root/device_info/sensor/device_sensor_proximity
#define root_device_info_sensor_device_sensor_proximity_exists 1
#define root_device_info_sensor_device_sensor_proximity_nodeName "device_sensor_proximity"
#define root_device_info_sensor_device_sensor_proximity_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_proximity_device0);

#define root_device_info_sensor_device_sensor_proximity_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_proximity_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_proximity/device0
#define root_device_info_sensor_device_sensor_proximity_device0_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_proximity_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_policy 1
#define root_device_info_sensor_device_sensor_proximity_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_priority 110
#define root_device_info_sensor_device_sensor_proximity_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_preload 2
#define root_device_info_sensor_device_sensor_proximity_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_permission 436
#define root_device_info_sensor_device_sensor_proximity_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_moduleName "HDF_SENSOR_PROXIMITY"
#define root_device_info_sensor_device_sensor_proximity_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_serviceName "hdf_sensor_proximity"
#define root_device_info_sensor_device_sensor_proximity_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_deviceMatchAttr "hdf_sensor_proximity_driver"

// hcs node macros: root/device_info/sensor/device_sensor_apds9960
#define root_device_info_sensor_device_sensor_apds9960_exists 1
#define root_device_info_sensor_device_sensor_apds9960_nodeName "device_sensor_apds9960"
#define root_device_info_sensor_device_sensor_apds9960_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_apds9960_device0);

#define root_device_info_sensor_device_sensor_apds9960_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_apds9960_device0, __VA_ARGS__);


// hcs node macros: root/device_info/sensor/device_sensor_apds9960/device0
#define root_device_info_sensor_device_sensor_apds9960_device0_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_nodeName "device0"
#define root_device_info_sensor_device_sensor_apds9960_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_policy 1
#define root_device_info_sensor_device_sensor_apds9960_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_priority 120
#define root_device_info_sensor_device_sensor_apds9960_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_preload 2
#define root_device_info_sensor_device_sensor_apds9960_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_permission 436
#define root_device_info_sensor_device_sensor_apds9960_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_moduleName "HDF_SENSOR_PROXIMITY_APDS9960"
#define root_device_info_sensor_device_sensor_apds9960_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_serviceName "hdf_proximity_apds9960"
#define root_device_info_sensor_device_sensor_apds9960_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_deviceMatchAttr "hdf_sensor_proximity_adps9960_driver"
#define root_device_info_sensor_priority_exists 1
#define root_device_info_sensor_priority 100

// hcs node macros: root/device_info/vibrator
#define root_device_info_vibrator_exists 1
#define root_device_info_vibrator_nodeName "vibrator"
#define root_device_info_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_vibrator); \
    func(root_device_info_vibrator_device_linear_vibrator);

#define root_device_info_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_vibrator, __VA_ARGS__); \
    func(root_device_info_vibrator_device_linear_vibrator, __VA_ARGS__);

#define root_device_info_vibrator_hostName_exists 1
#define root_device_info_vibrator_hostName "vibrator_host"

// hcs node macros: root/device_info/vibrator/device_vibrator
#define root_device_info_vibrator_device_vibrator_exists 1
#define root_device_info_vibrator_device_vibrator_nodeName "device_vibrator"
#define root_device_info_vibrator_device_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_vibrator_device0);

#define root_device_info_vibrator_device_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_vibrator_device0, __VA_ARGS__);


// hcs node macros: root/device_info/vibrator/device_vibrator/device0
#define root_device_info_vibrator_device_vibrator_device0_exists 1
#define root_device_info_vibrator_device_vibrator_device0_nodeName "device0"
#define root_device_info_vibrator_device_vibrator_device0_policy_exists 1
#define root_device_info_vibrator_device_vibrator_device0_policy 2
#define root_device_info_vibrator_device_vibrator_device0_priority_exists 1
#define root_device_info_vibrator_device_vibrator_device0_priority 100
#define root_device_info_vibrator_device_vibrator_device0_preload_exists 1
#define root_device_info_vibrator_device_vibrator_device0_preload 0
#define root_device_info_vibrator_device_vibrator_device0_permission_exists 1
#define root_device_info_vibrator_device_vibrator_device0_permission 436
#define root_device_info_vibrator_device_vibrator_device0_moduleName_exists 1
#define root_device_info_vibrator_device_vibrator_device0_moduleName "HDF_VIBRATOR"
#define root_device_info_vibrator_device_vibrator_device0_serviceName_exists 1
#define root_device_info_vibrator_device_vibrator_device0_serviceName "hdf_misc_vibrator"
#define root_device_info_vibrator_device_vibrator_device0_deviceMatchAttr_exists 1
#define root_device_info_vibrator_device_vibrator_device0_deviceMatchAttr "hdf_vibrator_driver"

// hcs node macros: root/device_info/vibrator/device_linear_vibrator
#define root_device_info_vibrator_device_linear_vibrator_exists 1
#define root_device_info_vibrator_device_linear_vibrator_nodeName "device_linear_vibrator"
#define root_device_info_vibrator_device_linear_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_linear_vibrator_device0);

#define root_device_info_vibrator_device_linear_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_linear_vibrator_device0, __VA_ARGS__);


// hcs node macros: root/device_info/vibrator/device_linear_vibrator/device0
#define root_device_info_vibrator_device_linear_vibrator_device0_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_nodeName "device0"
#define root_device_info_vibrator_device_linear_vibrator_device0_policy_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_policy 1
#define root_device_info_vibrator_device_linear_vibrator_device0_priority_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_priority 105
#define root_device_info_vibrator_device_linear_vibrator_device0_preload_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_preload 0
#define root_device_info_vibrator_device_linear_vibrator_device0_permission_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_permission 436
#define root_device_info_vibrator_device_linear_vibrator_device0_moduleName_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_moduleName "HDF_LINEAR_VIBRATOR"
#define root_device_info_vibrator_device_linear_vibrator_device0_serviceName_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_serviceName "hdf_misc_linear_vibrator"
#define root_device_info_vibrator_device_linear_vibrator_device0_deviceMatchAttr_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_deviceMatchAttr "hdf_linear_vibrator_driver"
#define root_device_info_vibrator_priority_exists 1
#define root_device_info_vibrator_priority 100

// hcs node macros: root/device_info/dsoftbus
#define root_device_info_dsoftbus_exists 1
#define root_device_info_dsoftbus_nodeName "dsoftbus"
#define root_device_info_dsoftbus_foreach_child(func) \
    func(root_device_info_dsoftbus_device_dsoftbus);

#define root_device_info_dsoftbus_foreach_child_vargs(func, ...) \
    func(root_device_info_dsoftbus_device_dsoftbus, __VA_ARGS__);

#define root_device_info_dsoftbus_hostName_exists 1
#define root_device_info_dsoftbus_hostName "dsoftbus_host"

// hcs node macros: root/device_info/dsoftbus/device_dsoftbus
#define root_device_info_dsoftbus_device_dsoftbus_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_nodeName "device_dsoftbus"
#define root_device_info_dsoftbus_device_dsoftbus_foreach_child(func) \
    func(root_device_info_dsoftbus_device_dsoftbus_device0);

#define root_device_info_dsoftbus_device_dsoftbus_foreach_child_vargs(func, ...) \
    func(root_device_info_dsoftbus_device_dsoftbus_device0, __VA_ARGS__);


// hcs node macros: root/device_info/dsoftbus/device_dsoftbus/device0
#define root_device_info_dsoftbus_device_dsoftbus_device0_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_nodeName "device0"
#define root_device_info_dsoftbus_device_dsoftbus_device0_policy_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_policy 2
#define root_device_info_dsoftbus_device_dsoftbus_device0_priority_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_priority 100
#define root_device_info_dsoftbus_device_dsoftbus_device0_preload_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_preload 0
#define root_device_info_dsoftbus_device_dsoftbus_device0_permission_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_permission 436
#define root_device_info_dsoftbus_device_dsoftbus_device0_moduleName_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_moduleName "HDF_DSOFTBUS"
#define root_device_info_dsoftbus_device_dsoftbus_device0_serviceName_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_serviceName "hdf_dsoftbus"
#define root_device_info_dsoftbus_device_dsoftbus_device0_deviceMatchAttr_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_deviceMatchAttr ""
#define root_device_info_dsoftbus_priority_exists 1
#define root_device_info_dsoftbus_priority 100

// hcs node macros: root/device_info/test_host
#define root_device_info_test_host_exists 1
#define root_device_info_test_host_nodeName "test_host"
#define root_device_info_test_host_foreach_child(func) \
    func(root_device_info_test_host_device_hdf_test); \
    func(root_device_info_test_host_device_platform_i2c_test); \
    func(root_device_info_test_host_device_platform_adc_test); \
    func(root_device_info_test_host_device_platform_spi_test); \
    func(root_device_info_test_host_device_platform_gpio_test); \
    func(root_device_info_test_host_device_platform_mipi_dsi_test); \
    func(root_device_info_test_host_device_platform_uart_test); \
    func(root_device_info_test_host_device_platform_sdio_test); \
    func(root_device_info_test_host_device_platform_pwm_test); \
    func(root_device_info_test_host_device_platform_emmc_test); \
    func(root_device_info_test_host_device_platform_watchdog_test); \
    func(root_device_info_test_host_device_platform_mipi_csi_test); \
    func(root_device_info_test_host_device_platform_regulator_test); \
    func(root_device_info_test_host_device_platform_rtc_test); \
    func(root_device_info_test_host_device_sensor_test);

#define root_device_info_test_host_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_hdf_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_i2c_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_adc_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_spi_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_gpio_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_mipi_dsi_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_uart_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_sdio_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_pwm_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_emmc_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_watchdog_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_mipi_csi_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_regulator_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_platform_rtc_test, __VA_ARGS__); \
    func(root_device_info_test_host_device_sensor_test, __VA_ARGS__);

#define root_device_info_test_host_hostName_exists 1
#define root_device_info_test_host_hostName "test_host"
#define root_device_info_test_host_priority_exists 1
#define root_device_info_test_host_priority 180

// hcs node macros: root/device_info/test_host/device_hdf_test
#define root_device_info_test_host_device_hdf_test_exists 1
#define root_device_info_test_host_device_hdf_test_nodeName "device_hdf_test"
#define root_device_info_test_host_device_hdf_test_foreach_child(func) \
    func(root_device_info_test_host_device_hdf_test_device0); \
    func(root_device_info_test_host_device_hdf_test_device1); \
    func(root_device_info_test_host_device_hdf_test_device2);

#define root_device_info_test_host_device_hdf_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_hdf_test_device0, __VA_ARGS__); \
    func(root_device_info_test_host_device_hdf_test_device1, __VA_ARGS__); \
    func(root_device_info_test_host_device_hdf_test_device2, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_hdf_test/device0
#define root_device_info_test_host_device_hdf_test_device0_exists 1
#define root_device_info_test_host_device_hdf_test_device0_nodeName "device0"
#define root_device_info_test_host_device_hdf_test_device0_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device0_policy 2
#define root_device_info_test_host_device_hdf_test_device0_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device0_priority 10
#define root_device_info_test_host_device_hdf_test_device0_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device0_preload 0
#define root_device_info_test_host_device_hdf_test_device0_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device0_permission 420
#define root_device_info_test_host_device_hdf_test_device0_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device0_moduleName "khdf_ut"
#define root_device_info_test_host_device_hdf_test_device0_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device0_serviceName "khdf_ut"
#define root_device_info_test_host_device_hdf_test_device0_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_hdf_test/device1
#define root_device_info_test_host_device_hdf_test_device1_exists 1
#define root_device_info_test_host_device_hdf_test_device1_nodeName "device1"
#define root_device_info_test_host_device_hdf_test_device1_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device1_policy 2
#define root_device_info_test_host_device_hdf_test_device1_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device1_priority 10
#define root_device_info_test_host_device_hdf_test_device1_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device1_preload 2
#define root_device_info_test_host_device_hdf_test_device1_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device1_permission 420
#define root_device_info_test_host_device_hdf_test_device1_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device1_moduleName "sample_driver"
#define root_device_info_test_host_device_hdf_test_device1_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device1_serviceName "sample_service"
#define root_device_info_test_host_device_hdf_test_device1_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device1_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_hdf_test/device2
#define root_device_info_test_host_device_hdf_test_device2_exists 1
#define root_device_info_test_host_device_hdf_test_device2_nodeName "device2"
#define root_device_info_test_host_device_hdf_test_device2_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device2_policy 2
#define root_device_info_test_host_device_hdf_test_device2_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device2_priority 10
#define root_device_info_test_host_device_hdf_test_device2_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device2_preload 0
#define root_device_info_test_host_device_hdf_test_device2_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device2_permission 420
#define root_device_info_test_host_device_hdf_test_device2_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device2_moduleName "pm_test_driver"
#define root_device_info_test_host_device_hdf_test_device2_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device2_serviceName "pm_ut_service"
#define root_device_info_test_host_device_hdf_test_device2_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device2_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_platform_i2c_test
#define root_device_info_test_host_device_platform_i2c_test_exists 1
#define root_device_info_test_host_device_platform_i2c_test_nodeName "device_platform_i2c_test"
#define root_device_info_test_host_device_platform_i2c_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_i2c_test_i2c_tester);

#define root_device_info_test_host_device_platform_i2c_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_i2c_test_i2c_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_i2c_test/i2c_tester
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_nodeName "i2c_tester"
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_policy_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_policy 2
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_priority_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_priority 101
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_preload_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_preload 0
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_permission_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_permission 420
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_moduleName "PLATFORM_I2C_TEST"
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_serviceName "I2C_TEST"
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_i2c_test_i2c_tester_deviceMatchAttr "I2C_TEST"

// hcs node macros: root/device_info/test_host/device_platform_adc_test
#define root_device_info_test_host_device_platform_adc_test_exists 1
#define root_device_info_test_host_device_platform_adc_test_nodeName "device_platform_adc_test"
#define root_device_info_test_host_device_platform_adc_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_adc_test_adc_tester);

#define root_device_info_test_host_device_platform_adc_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_adc_test_adc_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_adc_test/adc_tester
#define root_device_info_test_host_device_platform_adc_test_adc_tester_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_nodeName "adc_tester"
#define root_device_info_test_host_device_platform_adc_test_adc_tester_policy_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_policy 2
#define root_device_info_test_host_device_platform_adc_test_adc_tester_priority_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_priority 110
#define root_device_info_test_host_device_platform_adc_test_adc_tester_preload_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_preload 0
#define root_device_info_test_host_device_platform_adc_test_adc_tester_permission_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_permission 420
#define root_device_info_test_host_device_platform_adc_test_adc_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_moduleName "PLATFORM_ADC_TEST"
#define root_device_info_test_host_device_platform_adc_test_adc_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_serviceName "ADC_TEST"
#define root_device_info_test_host_device_platform_adc_test_adc_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_adc_test_adc_tester_deviceMatchAttr "ADC_TEST"

// hcs node macros: root/device_info/test_host/device_platform_spi_test
#define root_device_info_test_host_device_platform_spi_test_exists 1
#define root_device_info_test_host_device_platform_spi_test_nodeName "device_platform_spi_test"
#define root_device_info_test_host_device_platform_spi_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_spi_test_spi_test);

#define root_device_info_test_host_device_platform_spi_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_spi_test_spi_test, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_spi_test/spi_test
#define root_device_info_test_host_device_platform_spi_test_spi_test_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_nodeName "spi_test"
#define root_device_info_test_host_device_platform_spi_test_spi_test_policy_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_policy 2
#define root_device_info_test_host_device_platform_spi_test_spi_test_priority_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_priority 100
#define root_device_info_test_host_device_platform_spi_test_spi_test_preload_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_preload 0
#define root_device_info_test_host_device_platform_spi_test_spi_test_permission_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_permission 420
#define root_device_info_test_host_device_platform_spi_test_spi_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_moduleName "PLATFORM_SPI_TEST"
#define root_device_info_test_host_device_platform_spi_test_spi_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_serviceName "SPI_TEST"
#define root_device_info_test_host_device_platform_spi_test_spi_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_spi_test_spi_test_deviceMatchAttr "SPI_TEST"

// hcs node macros: root/device_info/test_host/device_platform_gpio_test
#define root_device_info_test_host_device_platform_gpio_test_exists 1
#define root_device_info_test_host_device_platform_gpio_test_nodeName "device_platform_gpio_test"
#define root_device_info_test_host_device_platform_gpio_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_gpio_test_gpio_tester);

#define root_device_info_test_host_device_platform_gpio_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_gpio_test_gpio_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_gpio_test/gpio_tester
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_nodeName "gpio_tester"
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_policy_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_policy 2
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_priority_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_priority 101
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_preload_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_preload 0
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_permission_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_permission 420
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_moduleName "PLATFORM_GPIO_TEST"
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_serviceName "GPIO_TEST"
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_gpio_test_gpio_tester_deviceMatchAttr "GPIO_TEST"

// hcs node macros: root/device_info/test_host/device_platform_mipi_dsi_test
#define root_device_info_test_host_device_platform_mipi_dsi_test_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_nodeName "device_platform_mipi_dsi_test"
#define root_device_info_test_host_device_platform_mipi_dsi_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester);

#define root_device_info_test_host_device_platform_mipi_dsi_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_mipi_dsi_test/mipi_dsi_tester
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_nodeName "mipi_dsi_tester"
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_policy_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_policy 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_priority_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_priority 100
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_preload_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_preload 0
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_permission_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_permission 420
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_moduleName "PLATFORM_MIPI_DSI_TEST"
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_serviceName "PLATFORM_MIPI_DSI_TEST"
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_mipi_dsi_test_mipi_dsi_tester_deviceMatchAttr "PLATFORM_MIPI_DSI_TEST"

// hcs node macros: root/device_info/test_host/device_platform_uart_test
#define root_device_info_test_host_device_platform_uart_test_exists 1
#define root_device_info_test_host_device_platform_uart_test_nodeName "device_platform_uart_test"
#define root_device_info_test_host_device_platform_uart_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_uart_test_uart_tester);

#define root_device_info_test_host_device_platform_uart_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_uart_test_uart_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_uart_test/uart_tester
#define root_device_info_test_host_device_platform_uart_test_uart_tester_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_nodeName "uart_tester"
#define root_device_info_test_host_device_platform_uart_test_uart_tester_policy_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_policy 2
#define root_device_info_test_host_device_platform_uart_test_uart_tester_priority_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_priority 100
#define root_device_info_test_host_device_platform_uart_test_uart_tester_preload_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_preload 0
#define root_device_info_test_host_device_platform_uart_test_uart_tester_permission_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_permission 420
#define root_device_info_test_host_device_platform_uart_test_uart_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_moduleName "PLATFORM_UART_TEST"
#define root_device_info_test_host_device_platform_uart_test_uart_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_serviceName "UART_TEST"
#define root_device_info_test_host_device_platform_uart_test_uart_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_uart_test_uart_tester_deviceMatchAttr "UART_TEST"

// hcs node macros: root/device_info/test_host/device_platform_sdio_test
#define root_device_info_test_host_device_platform_sdio_test_exists 1
#define root_device_info_test_host_device_platform_sdio_test_nodeName "device_platform_sdio_test"
#define root_device_info_test_host_device_platform_sdio_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_sdio_test_sdio_tester);

#define root_device_info_test_host_device_platform_sdio_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_sdio_test_sdio_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_sdio_test/sdio_tester
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_nodeName "sdio_tester"
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_policy_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_policy 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_priority_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_priority 100
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_preload_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_preload 0
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_permission_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_permission 420
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_moduleName "PLATFORM_SDIO_TEST"
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_serviceName "SDIO_TEST"
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_sdio_test_sdio_tester_deviceMatchAttr "SDIO_TEST"

// hcs node macros: root/device_info/test_host/device_platform_pwm_test
#define root_device_info_test_host_device_platform_pwm_test_exists 1
#define root_device_info_test_host_device_platform_pwm_test_nodeName "device_platform_pwm_test"
#define root_device_info_test_host_device_platform_pwm_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_pwm_test_pwm_test);

#define root_device_info_test_host_device_platform_pwm_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_pwm_test_pwm_test, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_pwm_test/pwm_test
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_nodeName "pwm_test"
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_policy_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_policy 2
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_priority_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_priority 110
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_preload_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_preload 0
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_permission_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_permission 420
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_moduleName "PLATFORM_PWM_TEST"
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_serviceName "PWM_TEST"
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_pwm_test_pwm_test_deviceMatchAttr "PWM_TEST"

// hcs node macros: root/device_info/test_host/device_platform_emmc_test
#define root_device_info_test_host_device_platform_emmc_test_exists 1
#define root_device_info_test_host_device_platform_emmc_test_nodeName "device_platform_emmc_test"
#define root_device_info_test_host_device_platform_emmc_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_emmc_test_emmc_tester);

#define root_device_info_test_host_device_platform_emmc_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_emmc_test_emmc_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_emmc_test/emmc_tester
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_nodeName "emmc_tester"
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_policy_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_policy 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_priority_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_priority 100
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_preload_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_preload 0
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_permission_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_permission 420
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_moduleName "PLATFORM_EMMC_TEST"
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_serviceName "EMMC_TEST"
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_emmc_test_emmc_tester_deviceMatchAttr "EMMC_TEST"

// hcs node macros: root/device_info/test_host/device_platform_watchdog_test
#define root_device_info_test_host_device_platform_watchdog_test_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_nodeName "device_platform_watchdog_test"
#define root_device_info_test_host_device_platform_watchdog_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_watchdog_test_watchdog_test);

#define root_device_info_test_host_device_platform_watchdog_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_watchdog_test_watchdog_test, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_watchdog_test/watchdog_test
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_nodeName "watchdog_test"
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_policy_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_policy 2
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_priority_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_priority 110
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_preload_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_preload 0
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_permission_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_permission 432
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_moduleName "PLATFORM_WATCHDOG_TEST"
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_serviceName "WATCHDOG_TEST"
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_watchdog_test_watchdog_test_deviceMatchAttr "WATCHDOG_TEST"

// hcs node macros: root/device_info/test_host/device_platform_mipi_csi_test
#define root_device_info_test_host_device_platform_mipi_csi_test_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_nodeName "device_platform_mipi_csi_test"
#define root_device_info_test_host_device_platform_mipi_csi_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester);

#define root_device_info_test_host_device_platform_mipi_csi_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_mipi_csi_test/mipi_csi_tester
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_nodeName "mipi_csi_tester"
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_policy_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_policy 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_priority_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_priority 100
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_preload_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_preload 0
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_permission_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_permission 420
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_moduleName "PLATFORM_MIPI_CSI_TEST"
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_serviceName "PLATFORM_MIPI_CSI_TEST"
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_mipi_csi_test_mipi_csi_tester_deviceMatchAttr "PLATFORM_MIPI_CSI_TEST"

// hcs node macros: root/device_info/test_host/device_platform_regulator_test
#define root_device_info_test_host_device_platform_regulator_test_exists 1
#define root_device_info_test_host_device_platform_regulator_test_nodeName "device_platform_regulator_test"
#define root_device_info_test_host_device_platform_regulator_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_regulator_test_regulator_tester);

#define root_device_info_test_host_device_platform_regulator_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_regulator_test_regulator_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_regulator_test/regulator_tester
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_nodeName "regulator_tester"
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_policy_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_policy 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_priority_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_priority 100
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_preload_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_preload 0
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_permission_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_permission 420
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_moduleName "PLATFORM_REGULATOR_TEST"
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_serviceName "REGULATOR_TEST"
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_regulator_test_regulator_tester_deviceMatchAttr "REGULATOR_TEST"

// hcs node macros: root/device_info/test_host/device_platform_rtc_test
#define root_device_info_test_host_device_platform_rtc_test_exists 1
#define root_device_info_test_host_device_platform_rtc_test_nodeName "device_platform_rtc_test"
#define root_device_info_test_host_device_platform_rtc_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_rtc_test_rtc_tester);

#define root_device_info_test_host_device_platform_rtc_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_rtc_test_rtc_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_platform_rtc_test/rtc_tester
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_nodeName "rtc_tester"
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_policy_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_policy 2
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_priority_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_priority 101
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_preload_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_preload 0
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_permission_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_permission 420
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_moduleName "PLATFORM_RTC_TEST"
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_serviceName "RTC_TEST"
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_rtc_test_rtc_tester_deviceMatchAttr "RTC_TEST"

// hcs node macros: root/device_info/test_host/device_sensor_test
#define root_device_info_test_host_device_sensor_test_exists 1
#define root_device_info_test_host_device_sensor_test_nodeName "device_sensor_test"
#define root_device_info_test_host_device_sensor_test_foreach_child(func) \
    func(root_device_info_test_host_device_sensor_test_sensor_tester);

#define root_device_info_test_host_device_sensor_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_sensor_test_sensor_tester, __VA_ARGS__);


// hcs node macros: root/device_info/test_host/device_sensor_test/sensor_tester
#define root_device_info_test_host_device_sensor_test_sensor_tester_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_nodeName "sensor_tester"
#define root_device_info_test_host_device_sensor_test_sensor_tester_policy_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_policy 0
#define root_device_info_test_host_device_sensor_test_sensor_tester_priority_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_priority 100
#define root_device_info_test_host_device_sensor_test_sensor_tester_preload_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_preload 0
#define root_device_info_test_host_device_sensor_test_sensor_tester_permission_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_permission 420
#define root_device_info_test_host_device_sensor_test_sensor_tester_moduleName_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_moduleName "HDF_SENSOR_TEST_DRIVER"
#define root_device_info_test_host_device_sensor_test_sensor_tester_serviceName_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_serviceName ""
#define root_device_info_test_host_device_sensor_test_sensor_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_deviceMatchAttr ""
#define root_module_exists 1
#define root_module "hisilicon,hi35xx_chip"

// hcs node macros: root/lcdkit_config
#define root_lcdkit_config_exists 1
#define root_lcdkit_config_nodeName "lcdkit_config"
#define root_lcdkit_config_foreach_child(func) \
    func(root_lcdkit_config_Icn9700Config); \
    func(root_lcdkit_config_Tg07aConfig); \
    func(root_lcdkit_config_St7701Config);

#define root_lcdkit_config_foreach_child_vargs(func, ...) \
    func(root_lcdkit_config_Icn9700Config, __VA_ARGS__); \
    func(root_lcdkit_config_Tg07aConfig, __VA_ARGS__); \
    func(root_lcdkit_config_St7701Config, __VA_ARGS__);

#define root_lcdkit_config_match_attr_exists 1
#define root_lcdkit_config_match_attr "hdf_lcdkit_driver"
#define root_lcdkit_config_active_panel_exists 1
#define root_lcdkit_config_active_panel "Icn9700Config"

// hcs node macros: root/lcdkit_config/Icn9700Config
#define root_lcdkit_config_Icn9700Config_exists 1
#define root_lcdkit_config_Icn9700Config_nodeName "Icn9700Config"
#define root_lcdkit_config_Icn9700Config_width_exists 1
#define root_lcdkit_config_Icn9700Config_width 480
#define root_lcdkit_config_Icn9700Config_height_exists 1
#define root_lcdkit_config_Icn9700Config_height 960
#define root_lcdkit_config_Icn9700Config_hbp_exists 1
#define root_lcdkit_config_Icn9700Config_hbp 20
#define root_lcdkit_config_Icn9700Config_hfp_exists 1
#define root_lcdkit_config_Icn9700Config_hfp 20
#define root_lcdkit_config_Icn9700Config_hsw_exists 1
#define root_lcdkit_config_Icn9700Config_hsw 10
#define root_lcdkit_config_Icn9700Config_vbp_exists 1
#define root_lcdkit_config_Icn9700Config_vbp 14
#define root_lcdkit_config_Icn9700Config_vfp_exists 1
#define root_lcdkit_config_Icn9700Config_vfp 16
#define root_lcdkit_config_Icn9700Config_vsw_exists 1
#define root_lcdkit_config_Icn9700Config_vsw 2
#define root_lcdkit_config_Icn9700Config_frameRate_exists 1
#define root_lcdkit_config_Icn9700Config_frameRate 60
#define root_lcdkit_config_Icn9700Config_intfType_exists 1
#define root_lcdkit_config_Icn9700Config_intfType 0
#define root_lcdkit_config_Icn9700Config_intfSync_exists 1
#define root_lcdkit_config_Icn9700Config_intfSync 0
#define root_lcdkit_config_Icn9700Config_dsiDev_exists 1
#define root_lcdkit_config_Icn9700Config_dsiDev 0
#define root_lcdkit_config_Icn9700Config_dsiLane_exists 1
#define root_lcdkit_config_Icn9700Config_dsiLane 2
#define root_lcdkit_config_Icn9700Config_mode_exists 1
#define root_lcdkit_config_Icn9700Config_mode 1
#define root_lcdkit_config_Icn9700Config_burstMode_exists 1
#define root_lcdkit_config_Icn9700Config_burstMode 0
#define root_lcdkit_config_Icn9700Config_pixelFmt_exists 1
#define root_lcdkit_config_Icn9700Config_pixelFmt 2
#define root_lcdkit_config_Icn9700Config_blkType_exists 1
#define root_lcdkit_config_Icn9700Config_blkType 0
#define root_lcdkit_config_Icn9700Config_minLevel_exists 1
#define root_lcdkit_config_Icn9700Config_minLevel 3
#define root_lcdkit_config_Icn9700Config_maxLevel_exists 1
#define root_lcdkit_config_Icn9700Config_maxLevel 255
#define root_lcdkit_config_Icn9700Config_defLevel_exists 1
#define root_lcdkit_config_Icn9700Config_defLevel 102
#define root_lcdkit_config_Icn9700Config_pwmDev_exists 1
#define root_lcdkit_config_Icn9700Config_pwmDev 1
#define root_lcdkit_config_Icn9700Config_pwmPeriod_exists 1
#define root_lcdkit_config_Icn9700Config_pwmPeriod 100000
#define root_lcdkit_config_Icn9700Config_panelOnCmd_exists 1
#define root_lcdkit_config_Icn9700Config_panelOnCmd_array_size 214
#define root_lcdkit_config_Icn9700Config_panelOnCmd_data { \
    41, 0, 3, 240, 90, 90, 41, 0, \
    3, 241, 165, 165, 41, 0, 12, 179, \
    3, 3, 3, 7, 5, 13, 15, 17, \
    19, 9, 11, 41, 0, 12, 180, 3, \
    3, 3, 6, 4, 12, 14, 16, 18, \
    8, 10, 41, 0, 13, 176, 84, 50, \
    35, 69, 68, 68, 68, 68, 96, 0, \
    96, 28, 41, 0, 9, 177, 50, 132, \
    2, 135, 18, 0, 80, 28, 41, 0, \
    4, 178, 115, 9, 8, 41, 0, 4, \
    182, 92, 92, 5, 41, 0, 6, 184, \
    35, 65, 50, 48, 3, 41, 0, 11, \
    188, 210, 14, 99, 99, 90, 50, 34, \
    20, 34, 3, 21, 0, 2, 183, 65, \
    41, 0, 7, 193, 12, 16, 4, 12, \
    16, 4, 41, 0, 3, 194, 16, 224, \
    41, 0, 3, 195, 34, 17, 41, 0, \
    3, 208, 7, 255, 41, 0, 5, 210, \
    99, 11, 8, 136, 41, 0, 8, 198, \
    8, 21, 255, 16, 22, 128, 96, 21, \
    0, 2, 199, 4, 41, 1, 39, 200, \
    124, 80, 59, 44, 37, 22, 28, 8, \
    39, 43, 47, 82, 67, 76, 64, 61, \
    48, 30, 6, 124, 80, 59, 44, 37, \
    22, 28, 8, 39, 43, 47, 82, 67, \
    76, 64, 61, 48, 30, 6, 5, 120, \
    1, 17, 5, 120, 1, 41 \
}
#define root_lcdkit_config_Icn9700Config_panelOffCmd_exists 1
#define root_lcdkit_config_Icn9700Config_panelOffCmd_array_size 8
#define root_lcdkit_config_Icn9700Config_panelOffCmd_data { \
    5, 20, 1, 40, 5, 120, 1, 16 \
}
#define root_lcdkit_config_Icn9700Config_powerSetting_exists 1
#define root_lcdkit_config_Icn9700Config_powerSetting_array_size 3
#define root_lcdkit_config_Icn9700Config_powerSetting_data { \
    0, 5, 0 \
}
#define root_lcdkit_config_Icn9700Config_powerOnSeq_exists 1
#define root_lcdkit_config_Icn9700Config_powerOnSeq_array_size 4
#define root_lcdkit_config_Icn9700Config_powerOnSeq_data { \
    0, 5, 1, 5 \
}
#define root_lcdkit_config_Icn9700Config_powerOffSeq_exists 1
#define root_lcdkit_config_Icn9700Config_powerOffSeq_array_size 4
#define root_lcdkit_config_Icn9700Config_powerOffSeq_data { \
    0, 5, 0, 5 \
}

// hcs node macros: root/lcdkit_config/Tg07aConfig
#define root_lcdkit_config_Tg07aConfig_exists 1
#define root_lcdkit_config_Tg07aConfig_nodeName "Tg07aConfig"
#define root_lcdkit_config_Tg07aConfig_width_exists 1
#define root_lcdkit_config_Tg07aConfig_width 600
#define root_lcdkit_config_Tg07aConfig_height_exists 1
#define root_lcdkit_config_Tg07aConfig_height 1024
#define root_lcdkit_config_Tg07aConfig_hbp_exists 1
#define root_lcdkit_config_Tg07aConfig_hbp 60
#define root_lcdkit_config_Tg07aConfig_hfp_exists 1
#define root_lcdkit_config_Tg07aConfig_hfp 80
#define root_lcdkit_config_Tg07aConfig_hsw_exists 1
#define root_lcdkit_config_Tg07aConfig_hsw 1
#define root_lcdkit_config_Tg07aConfig_vbp_exists 1
#define root_lcdkit_config_Tg07aConfig_vbp 25
#define root_lcdkit_config_Tg07aConfig_vfp_exists 1
#define root_lcdkit_config_Tg07aConfig_vfp 35
#define root_lcdkit_config_Tg07aConfig_vsw_exists 1
#define root_lcdkit_config_Tg07aConfig_vsw 1
#define root_lcdkit_config_Tg07aConfig_frameRate_exists 1
#define root_lcdkit_config_Tg07aConfig_frameRate 60
#define root_lcdkit_config_Tg07aConfig_intfType_exists 1
#define root_lcdkit_config_Tg07aConfig_intfType 0
#define root_lcdkit_config_Tg07aConfig_intfSync_exists 1
#define root_lcdkit_config_Tg07aConfig_intfSync 0
#define root_lcdkit_config_Tg07aConfig_dsiDev_exists 1
#define root_lcdkit_config_Tg07aConfig_dsiDev 0
#define root_lcdkit_config_Tg07aConfig_dsiLane_exists 1
#define root_lcdkit_config_Tg07aConfig_dsiLane 4
#define root_lcdkit_config_Tg07aConfig_mode_exists 1
#define root_lcdkit_config_Tg07aConfig_mode 1
#define root_lcdkit_config_Tg07aConfig_burstMode_exists 1
#define root_lcdkit_config_Tg07aConfig_burstMode 0
#define root_lcdkit_config_Tg07aConfig_pixelFmt_exists 1
#define root_lcdkit_config_Tg07aConfig_pixelFmt 2
#define root_lcdkit_config_Tg07aConfig_blkType_exists 1
#define root_lcdkit_config_Tg07aConfig_blkType 0
#define root_lcdkit_config_Tg07aConfig_minLevel_exists 1
#define root_lcdkit_config_Tg07aConfig_minLevel 3
#define root_lcdkit_config_Tg07aConfig_maxLevel_exists 1
#define root_lcdkit_config_Tg07aConfig_maxLevel 255
#define root_lcdkit_config_Tg07aConfig_defLevel_exists 1
#define root_lcdkit_config_Tg07aConfig_defLevel 102
#define root_lcdkit_config_Tg07aConfig_pwmDev_exists 1
#define root_lcdkit_config_Tg07aConfig_pwmDev 0
#define root_lcdkit_config_Tg07aConfig_pwmPeriod_exists 1
#define root_lcdkit_config_Tg07aConfig_pwmPeriod 100000
#define root_lcdkit_config_Tg07aConfig_panelOnCmd_exists 1
#define root_lcdkit_config_Tg07aConfig_panelOnCmd_array_size 1303
#define root_lcdkit_config_Tg07aConfig_panelOnCmd_data { \
    21, 0, 2, 176, 90, 21, 0, 2, \
    177, 0, 21, 0, 2, 137, 1, 21, \
    0, 2, 145, 23, 21, 0, 2, 177, \
    3, 21, 0, 2, 44, 40, 21, 0, \
    2, 0, 223, 21, 0, 2, 1, 239, \
    21, 0, 2, 2, 247, 21, 0, 2, \
    3, 251, 21, 0, 2, 4, 253, 21, \
    0, 2, 5, 0, 21, 0, 2, 6, \
    0, 21, 0, 2, 7, 0, 21, 0, \
    2, 8, 0, 21, 0, 2, 9, 0, \
    21, 0, 2, 10, 1, 21, 0, 2, \
    11, 59, 21, 0, 2, 12, 0, 21, \
    0, 2, 13, 0, 21, 0, 2, 14, \
    36, 21, 0, 2, 15, 28, 21, 0, \
    2, 16, 201, 21, 0, 2, 17, 96, \
    21, 0, 2, 18, 112, 21, 0, 2, \
    19, 1, 21, 0, 2, 20, 227, 21, \
    0, 2, 21, 255, 21, 0, 2, 22, \
    61, 21, 0, 2, 23, 14, 21, 0, \
    2, 24, 1, 21, 0, 2, 25, 0, \
    21, 0, 2, 26, 0, 21, 0, 2, \
    27, 252, 21, 0, 2, 28, 11, 21, \
    0, 2, 29, 160, 21, 0, 2, 30, \
    3, 21, 0, 2, 31, 4, 21, 0, \
    2, 32, 12, 21, 0, 2, 33, 0, \
    21, 0, 2, 34, 4, 21, 0, 2, \
    35, 129, 21, 0, 2, 36, 31, 21, \
    0, 2, 37, 16, 21, 0, 2, 38, \
    155, 21, 0, 2, 44, 44, 21, 0, \
    2, 45, 1, 21, 0, 2, 46, 132, \
    21, 0, 2, 47, 0, 21, 0, 2, \
    48, 2, 21, 0, 2, 49, 8, 21, \
    0, 2, 50, 1, 21, 0, 2, 51, \
    28, 21, 0, 2, 52, 112, 21, 0, \
    2, 53, 255, 21, 0, 2, 54, 255, \
    21, 0, 2, 55, 255, 21, 0, 2, \
    56, 255, 21, 0, 2, 57, 255, 21, \
    0, 2, 58, 5, 21, 0, 2, 59, \
    0, 21, 0, 2, 60, 0, 21, 0, \
    2, 61, 0, 21, 0, 2, 62, 15, \
    21, 0, 2, 63, 132, 21, 0, 2, \
    64, 42, 21, 0, 2, 65, 0, 21, \
    0, 2, 66, 1, 21, 0, 2, 67, \
    64, 21, 0, 2, 68, 5, 21, 0, \
    2, 69, 232, 21, 0, 2, 70, 22, \
    21, 0, 2, 71, 0, 21, 0, 2, \
    72, 0, 21, 0, 2, 73, 136, 21, \
    0, 2, 74, 8, 21, 0, 2, 75, \
    5, 21, 0, 2, 76, 3, 21, 0, \
    2, 77, 208, 21, 0, 2, 78, 19, \
    21, 0, 2, 79, 255, 21, 0, 2, \
    80, 10, 21, 0, 2, 81, 83, 21, \
    0, 2, 82, 38, 21, 0, 2, 83, \
    34, 21, 0, 2, 84, 9, 21, 0, \
    2, 85, 34, 21, 0, 2, 86, 0, \
    21, 0, 2, 87, 28, 21, 0, 2, \
    88, 3, 21, 0, 2, 89, 63, 21, \
    0, 2, 90, 40, 21, 0, 2, 91, \
    1, 21, 0, 2, 92, 204, 21, 0, \
    2, 93, 33, 21, 0, 2, 94, 4, \
    21, 0, 2, 95, 19, 21, 0, 2, \
    96, 66, 21, 0, 2, 97, 8, 21, \
    0, 2, 98, 100, 21, 0, 2, 99, \
    235, 21, 0, 2, 100, 16, 21, 0, \
    2, 101, 168, 21, 0, 2, 102, 132, \
    21, 0, 2, 103, 142, 21, 0, 2, \
    104, 41, 21, 0, 2, 105, 17, 21, \
    0, 2, 106, 66, 21, 0, 2, 107, \
    56, 21, 0, 2, 108, 33, 21, 0, \
    2, 109, 132, 21, 0, 2, 110, 80, \
    21, 0, 2, 111, 182, 21, 0, 2, \
    112, 14, 21, 0, 2, 113, 161, 21, \
    0, 2, 114, 206, 21, 0, 2, 115, \
    248, 21, 0, 2, 116, 218, 21, 0, \
    2, 117, 26, 21, 0, 2, 118, 128, \
    21, 0, 2, 119, 0, 21, 0, 2, \
    120, 95, 21, 0, 2, 121, 224, 21, \
    0, 2, 122, 1, 21, 0, 2, 123, \
    255, 21, 0, 2, 124, 255, 21, 0, \
    2, 125, 255, 21, 0, 2, 126, 255, \
    21, 0, 2, 127, 254, 21, 0, 2, \
    177, 2, 21, 0, 2, 0, 255, 21, \
    0, 2, 1, 1, 21, 0, 2, 2, \
    0, 21, 0, 2, 3, 0, 21, 0, \
    2, 4, 0, 21, 0, 2, 5, 0, \
    21, 0, 2, 6, 0, 21, 0, 2, \
    7, 0, 21, 0, 2, 8, 192, 21, \
    0, 2, 9, 0, 21, 0, 2, 10, \
    0, 21, 0, 2, 11, 20, 21, 0, \
    2, 12, 230, 21, 0, 2, 13, 13, \
    21, 0, 2, 15, 0, 21, 0, 2, \
    16, 121, 21, 0, 2, 17, 173, 21, \
    0, 2, 18, 190, 21, 0, 2, 19, \
    238, 21, 0, 2, 20, 114, 21, 0, \
    2, 21, 223, 21, 0, 2, 22, 231, \
    21, 0, 2, 23, 221, 21, 0, 2, \
    24, 207, 21, 0, 2, 25, 241, 21, \
    0, 2, 26, 117, 21, 0, 2, 27, \
    14, 21, 0, 2, 28, 255, 21, 0, \
    2, 29, 255, 21, 0, 2, 30, 255, \
    21, 0, 2, 31, 255, 21, 0, 2, \
    32, 255, 21, 0, 2, 33, 255, 21, \
    0, 2, 34, 255, 21, 0, 2, 35, \
    255, 21, 0, 2, 36, 255, 21, 0, \
    2, 37, 255, 21, 0, 2, 38, 255, \
    21, 0, 2, 39, 31, 21, 0, 2, \
    40, 240, 21, 0, 2, 41, 255, 21, \
    0, 2, 42, 255, 21, 0, 2, 43, \
    255, 21, 0, 2, 44, 255, 21, 0, \
    2, 45, 7, 21, 0, 2, 47, 255, \
    21, 0, 2, 48, 127, 21, 0, 2, \
    49, 145, 21, 0, 2, 50, 0, 21, \
    0, 2, 51, 8, 21, 0, 2, 53, \
    127, 21, 0, 2, 54, 5, 21, 0, \
    2, 56, 127, 21, 0, 2, 58, 128, \
    21, 0, 2, 59, 1, 21, 0, 2, \
    60, 192, 21, 0, 2, 61, 50, 21, \
    0, 2, 62, 0, 21, 0, 2, 63, \
    88, 21, 0, 2, 64, 6, 21, 0, \
    2, 65, 0, 21, 0, 2, 66, 203, \
    21, 0, 2, 67, 0, 21, 0, 2, \
    68, 96, 21, 0, 2, 69, 9, 21, \
    0, 2, 70, 0, 21, 0, 2, 71, \
    0, 21, 0, 2, 72, 139, 21, 0, \
    2, 73, 210, 21, 0, 2, 74, 1, \
    21, 0, 2, 75, 0, 21, 0, 2, \
    76, 16, 21, 0, 2, 77, 64, 21, \
    0, 2, 78, 13, 21, 0, 2, 79, \
    97, 21, 0, 2, 80, 60, 21, 0, \
    2, 81, 122, 21, 0, 2, 82, 52, \
    21, 0, 2, 83, 153, 21, 0, 2, \
    84, 162, 21, 0, 2, 85, 3, 21, \
    0, 2, 86, 108, 21, 0, 2, 87, \
    26, 21, 0, 2, 88, 5, 21, 0, \
    2, 89, 240, 21, 0, 2, 90, 251, \
    21, 0, 2, 91, 253, 21, 0, 2, \
    92, 126, 21, 0, 2, 93, 191, 21, \
    0, 2, 94, 31, 21, 0, 2, 95, \
    0, 21, 0, 2, 96, 240, 21, 0, \
    2, 97, 243, 21, 0, 2, 98, 251, \
    21, 0, 2, 99, 249, 21, 0, 2, \
    100, 253, 21, 0, 2, 101, 126, 21, \
    0, 2, 102, 0, 21, 0, 2, 103, \
    0, 21, 0, 2, 104, 20, 21, 0, \
    2, 105, 137, 21, 0, 2, 106, 112, \
    21, 0, 2, 107, 252, 21, 0, 2, \
    108, 252, 21, 0, 2, 109, 252, 21, \
    0, 2, 110, 252, 21, 0, 2, 111, \
    252, 21, 0, 2, 112, 126, 21, 0, \
    2, 113, 191, 21, 0, 2, 114, 223, \
    21, 0, 2, 115, 207, 21, 0, 2, \
    116, 207, 21, 0, 2, 117, 207, 21, \
    0, 2, 118, 15, 21, 0, 2, 119, \
    0, 21, 0, 2, 120, 0, 21, 0, \
    2, 121, 0, 21, 0, 2, 122, 126, \
    21, 0, 2, 123, 126, 21, 0, 2, \
    124, 126, 21, 0, 2, 125, 126, 21, \
    0, 2, 126, 126, 21, 0, 2, 127, \
    191, 21, 0, 2, 11, 4, 21, 0, \
    2, 177, 3, 21, 0, 2, 44, 44, \
    21, 0, 2, 177, 0, 21, 0, 2, \
    137, 3, 5, 120, 1, 17, 5, 20, \
    1, 41, 21, 5, 2, 55, 1 \
}
#define root_lcdkit_config_Tg07aConfig_panelOffCmd_exists 1
#define root_lcdkit_config_Tg07aConfig_panelOffCmd_array_size 8
#define root_lcdkit_config_Tg07aConfig_panelOffCmd_data { \
    5, 20, 1, 40, 5, 120, 1, 16 \
}
#define root_lcdkit_config_Tg07aConfig_powerSetting_exists 1
#define root_lcdkit_config_Tg07aConfig_powerSetting_array_size 3
#define root_lcdkit_config_Tg07aConfig_powerSetting_data { \
    0, 5, 0 \
}
#define root_lcdkit_config_Tg07aConfig_powerOnSeq_exists 1
#define root_lcdkit_config_Tg07aConfig_powerOnSeq_array_size 4
#define root_lcdkit_config_Tg07aConfig_powerOnSeq_data { \
    0, 5, 1, 5 \
}
#define root_lcdkit_config_Tg07aConfig_powerOffSeq_exists 1
#define root_lcdkit_config_Tg07aConfig_powerOffSeq_array_size 4
#define root_lcdkit_config_Tg07aConfig_powerOffSeq_data { \
    0, 5, 0, 5 \
}

// hcs node macros: root/lcdkit_config/St7701Config
#define root_lcdkit_config_St7701Config_exists 1
#define root_lcdkit_config_St7701Config_nodeName "St7701Config"
#define root_lcdkit_config_St7701Config_width_exists 1
#define root_lcdkit_config_St7701Config_width 480
#define root_lcdkit_config_St7701Config_height_exists 1
#define root_lcdkit_config_St7701Config_height 800
#define root_lcdkit_config_St7701Config_hbp_exists 1
#define root_lcdkit_config_St7701Config_hbp 50
#define root_lcdkit_config_St7701Config_hfp_exists 1
#define root_lcdkit_config_St7701Config_hfp 50
#define root_lcdkit_config_St7701Config_hsw_exists 1
#define root_lcdkit_config_St7701Config_hsw 10
#define root_lcdkit_config_St7701Config_vbp_exists 1
#define root_lcdkit_config_St7701Config_vbp 20
#define root_lcdkit_config_St7701Config_vfp_exists 1
#define root_lcdkit_config_St7701Config_vfp 20
#define root_lcdkit_config_St7701Config_vsw_exists 1
#define root_lcdkit_config_St7701Config_vsw 4
#define root_lcdkit_config_St7701Config_frameRate_exists 1
#define root_lcdkit_config_St7701Config_frameRate 60
#define root_lcdkit_config_St7701Config_intfType_exists 1
#define root_lcdkit_config_St7701Config_intfType 0
#define root_lcdkit_config_St7701Config_intfSync_exists 1
#define root_lcdkit_config_St7701Config_intfSync 0
#define root_lcdkit_config_St7701Config_dsiDev_exists 1
#define root_lcdkit_config_St7701Config_dsiDev 0
#define root_lcdkit_config_St7701Config_dsiLane_exists 1
#define root_lcdkit_config_St7701Config_dsiLane 2
#define root_lcdkit_config_St7701Config_mode_exists 1
#define root_lcdkit_config_St7701Config_mode 1
#define root_lcdkit_config_St7701Config_burstMode_exists 1
#define root_lcdkit_config_St7701Config_burstMode 0
#define root_lcdkit_config_St7701Config_pixelFmt_exists 1
#define root_lcdkit_config_St7701Config_pixelFmt 2
#define root_lcdkit_config_St7701Config_blkType_exists 1
#define root_lcdkit_config_St7701Config_blkType 0
#define root_lcdkit_config_St7701Config_minLevel_exists 1
#define root_lcdkit_config_St7701Config_minLevel 3
#define root_lcdkit_config_St7701Config_maxLevel_exists 1
#define root_lcdkit_config_St7701Config_maxLevel 255
#define root_lcdkit_config_St7701Config_defLevel_exists 1
#define root_lcdkit_config_St7701Config_defLevel 102
#define root_lcdkit_config_St7701Config_pwmDev_exists 1
#define root_lcdkit_config_St7701Config_pwmDev 1
#define root_lcdkit_config_St7701Config_pwmPeriod_exists 1
#define root_lcdkit_config_St7701Config_pwmPeriod 100000
#define root_lcdkit_config_St7701Config_panelOnCmd_exists 1
#define root_lcdkit_config_St7701Config_panelOnCmd_array_size 357
#define root_lcdkit_config_St7701Config_panelOnCmd_data { \
    41, 0, 6, 255, 119, 1, 0, 0, \
    19, 21, 0, 2, 239, 8, 41, 0, \
    6, 255, 119, 1, 0, 0, 16, 41, \
    0, 3, 192, 99, 0, 41, 0, 3, \
    193, 16, 2, 41, 0, 3, 194, 1, \
    8, 21, 0, 2, 204, 24, 41, 0, \
    17, 176, 64, 201, 143, 13, 17, 7, \
    2, 9, 9, 31, 4, 80, 15, 228, \
    41, 223, 41, 0, 17, 177, 64, 203, \
    211, 17, 143, 4, 0, 8, 7, 28, \
    6, 83, 18, 99, 235, 223, 41, 0, \
    6, 255, 119, 1, 0, 0, 17, 21, \
    0, 2, 176, 101, 21, 0, 2, 177, \
    52, 21, 0, 2, 178, 135, 21, 0, \
    2, 179, 128, 21, 0, 2, 181, 73, \
    21, 0, 2, 183, 133, 21, 0, 2, \
    184, 32, 21, 0, 2, 185, 16, 21, \
    0, 2, 193, 120, 21, 0, 2, 194, \
    120, 21, 100, 2, 208, 136, 41, 1, \
    4, 224, 0, 25, 2, 41, 0, 12, \
    225, 5, 160, 7, 160, 4, 160, 6, \
    160, 0, 68, 68, 41, 0, 13, 226, \
    0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 0, 0, 41, 0, 5, 227, \
    0, 0, 51, 51, 41, 0, 3, 228, \
    68, 68, 41, 0, 17, 229, 13, 49, \
    200, 175, 15, 51, 200, 175, 9, 45, \
    200, 175, 11, 47, 200, 175, 41, 0, \
    5, 230, 0, 0, 51, 51, 41, 0, \
    3, 231, 68, 68, 41, 0, 17, 232, \
    12, 48, 200, 175, 14, 50, 200, 175, \
    8, 44, 200, 175, 10, 46, 200, 175, \
    41, 0, 8, 235, 2, 0, 228, 228, \
    68, 0, 64, 41, 0, 3, 236, 60, \
    0, 41, 0, 17, 237, 171, 137, 118, \
    84, 1, 255, 255, 255, 255, 255, 255, \
    16, 69, 103, 152, 186, 41, 0, 7, \
    239, 8, 8, 8, 69, 63, 84, 41, \
    0, 6, 255, 119, 1, 0, 0, 0, \
    41, 0, 6, 255, 119, 1, 0, 0, \
    19, 41, 120, 4, 232, 0, 14, 17, \
    41, 10, 3, 232, 0, 12, 41, 10, \
    3, 232, 0, 0, 41, 10, 6, 255, \
    119, 1, 0, 0, 0, 5, 150, 1, \
    17, 5, 50, 1, 41 \
}
#define root_lcdkit_config_St7701Config_panelOffCmd_exists 1
#define root_lcdkit_config_St7701Config_panelOffCmd_array_size 8
#define root_lcdkit_config_St7701Config_panelOffCmd_data { \
    5, 20, 1, 40, 5, 120, 1, 16 \
}
#define root_lcdkit_config_St7701Config_powerSetting_exists 1
#define root_lcdkit_config_St7701Config_powerSetting_array_size 3
#define root_lcdkit_config_St7701Config_powerSetting_data { \
    0, 5, 0 \
}
#define root_lcdkit_config_St7701Config_powerOnSeq_exists 1
#define root_lcdkit_config_St7701Config_powerOnSeq_array_size 4
#define root_lcdkit_config_St7701Config_powerOnSeq_data { \
    0, 5, 1, 5 \
}
#define root_lcdkit_config_St7701Config_powerOffSeq_exists 1
#define root_lcdkit_config_St7701Config_powerOffSeq_array_size 4
#define root_lcdkit_config_St7701Config_powerOffSeq_data { \
    0, 5, 0, 5 \
}

// hcs node macros: root/backlightConfig
#define root_backlightConfig_exists 1
#define root_backlightConfig_nodeName "backlightConfig"
#define root_backlightConfig_foreach_child(func) \
    func(root_backlightConfig_pwmBacklightConfig);

#define root_backlightConfig_foreach_child_vargs(func, ...) \
    func(root_backlightConfig_pwmBacklightConfig, __VA_ARGS__);


// hcs node macros: root/backlightConfig/pwmBacklightConfig
#define root_backlightConfig_pwmBacklightConfig_exists 1
#define root_backlightConfig_pwmBacklightConfig_nodeName "pwmBacklightConfig"
#define root_backlightConfig_pwmBacklightConfig_match_attr_exists 1
#define root_backlightConfig_pwmBacklightConfig_match_attr "pwm_bl_dev"
#define root_backlightConfig_pwmBacklightConfig_pwmDevNum_exists 1
#define root_backlightConfig_pwmBacklightConfig_pwmDevNum 1
#define root_backlightConfig_pwmBacklightConfig_pwmMaxPeriod_exists 1
#define root_backlightConfig_pwmBacklightConfig_pwmMaxPeriod 100000
#define root_backlightConfig_pwmBacklightConfig_backlightDevName_exists 1
#define root_backlightConfig_pwmBacklightConfig_backlightDevName "hdf_pwm"
#define root_backlightConfig_pwmBacklightConfig_minBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_minBrightness 0
#define root_backlightConfig_pwmBacklightConfig_defBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_defBrightness 127
#define root_backlightConfig_pwmBacklightConfig_maxBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_maxBrightness 255

// hcs node macros: root/input_config
#define root_input_config_exists 1
#define root_input_config_nodeName "input_config"
#define root_input_config_foreach_child(func) \
    func(root_input_config_touchConfig); \
    func(root_input_config_keyConfig);

#define root_input_config_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig, __VA_ARGS__); \
    func(root_input_config_keyConfig, __VA_ARGS__);


// hcs node macros: root/input_config/touchConfig
#define root_input_config_touchConfig_exists 1
#define root_input_config_touchConfig_nodeName "touchConfig"
#define root_input_config_touchConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0);

#define root_input_config_touchConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0, __VA_ARGS__);


// hcs node macros: root/input_config/touchConfig/touch0
#define root_input_config_touchConfig_touch0_exists 1
#define root_input_config_touchConfig_touch0_nodeName "touch0"
#define root_input_config_touchConfig_touch0_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_boardConfig); \
    func(root_input_config_touchConfig_touch0_chipConfig);

#define root_input_config_touchConfig_touch0_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_boardConfig, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_chipConfig, __VA_ARGS__);


// hcs node macros: root/input_config/touchConfig/touch0/boardConfig
#define root_input_config_touchConfig_touch0_boardConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_nodeName "boardConfig"
#define root_input_config_touchConfig_touch0_boardConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_boardConfig_inputAttr); \
    func(root_input_config_touchConfig_touch0_boardConfig_busConfig); \
    func(root_input_config_touchConfig_touch0_boardConfig_pinConfig); \
    func(root_input_config_touchConfig_touch0_boardConfig_powerConfig); \
    func(root_input_config_touchConfig_touch0_boardConfig_featureConfig);

#define root_input_config_touchConfig_touch0_boardConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_boardConfig_inputAttr, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_boardConfig_busConfig, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_boardConfig_pinConfig, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_boardConfig_powerConfig, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_boardConfig_featureConfig, __VA_ARGS__);

#define root_input_config_touchConfig_touch0_boardConfig_match_attr_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_match_attr "touch_device1"

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/inputAttr
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_nodeName "inputAttr"
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_inputType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_inputType 0
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionX_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionX 480
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionY_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionY 960
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_devName_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_devName "main_touch"

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/busConfig
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_nodeName "busConfig"
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busType 0
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busNum 6
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_clkGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_clkGpio 86
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_dataGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_dataGpio 87
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_data { \
    290390088, 1027 \
}
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_data { \
    290390092, 1027 \
}

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/pinConfig
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_nodeName "pinConfig"
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstGpio 3
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intGpio 4
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_data { \
    288293012, 1024 \
}
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_data { \
    288293016, 1024 \
}

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/powerConfig
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_nodeName "powerConfig"
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccType 2
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccNum 20
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccValue_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccValue 1800
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciType 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciNum 12
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciValue_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciValue 3300

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/featureConfig
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_nodeName "featureConfig"
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_capacitanceTest_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_capacitanceTest 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gestureMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gestureMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gloverMOde_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gloverMOde 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_coverMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_coverMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_chargerMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_chargerMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_knuckleMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_knuckleMode 0

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig
#define root_input_config_touchConfig_touch0_chipConfig_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_nodeName "chipConfig"
#define root_input_config_touchConfig_touch0_chipConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3);

#define root_input_config_touchConfig_touch0_chipConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2, __VA_ARGS__); \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3, __VA_ARGS__);


// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_nodeName "chip0"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence);

#define root_input_config_touchConfig_touch0_chipConfig_chip0_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence, __VA_ARGS__);

#define root_input_config_touchConfig_touch0_chipConfig_chip0_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_match_attr "zsj_gt911_5p5"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipInfo "ZIDN45100"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipVersion 0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_vendorName "zsj"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip0_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip0_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip0/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_nodeName "powerSequence"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_nodeName "chip1"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence);

#define root_input_config_touchConfig_touch0_chipConfig_chip1_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence, __VA_ARGS__);

#define root_input_config_touchConfig_touch0_chipConfig_chip1_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_match_attr "zsj_gt911_4p0"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipInfo "ZIDN45101"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipVersion 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_vendorName "zsj"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip1_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip1_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip1_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip1/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_nodeName "powerSequence"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_nodeName "chip2"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence);

#define root_input_config_touchConfig_touch0_chipConfig_chip2_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence, __VA_ARGS__);

#define root_input_config_touchConfig_touch0_chipConfig_chip2_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_match_attr "tg_gt911_7p0"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_vendorName "tg"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipInfo "ZIDN45102"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipVersion 2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip2_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip2_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip2/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_nodeName "powerSequence"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip3
#define root_input_config_touchConfig_touch0_chipConfig_chip3_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_nodeName "chip3"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence);

#define root_input_config_touchConfig_touch0_chipConfig_chip3_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence, __VA_ARGS__);

#define root_input_config_touchConfig_touch0_chipConfig_chip3_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_match_attr "st_ft6336_2p35"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipName "ft6336"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_vendorName "st"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipInfo "ZIDN53200"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_deviceAddr 56
#define root_input_config_touchConfig_touch0_chipConfig_chip3_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_irqFlag 1

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip3/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_nodeName "powerSequence"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_data { \
    4, 2, 0, 0, 3, 1, 1, 2, \
    3, 0, 2, 5, 3, 1, 2, 200 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip3_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_maxSpeed 400
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipVersion 0

// hcs node macros: root/input_config/keyConfig
#define root_input_config_keyConfig_exists 1
#define root_input_config_keyConfig_nodeName "keyConfig"
#define root_input_config_keyConfig_foreach_child(func) \
    func(root_input_config_keyConfig_keyInfoList);

#define root_input_config_keyConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_keyConfig_keyInfoList, __VA_ARGS__);

#define root_input_config_keyConfig_keyList_exists 1
#define root_input_config_keyConfig_keyList_array_size 7
#define root_input_config_keyConfig_keyList_data { \
    "power", "VolUp", "VolDown", "Up", "Down", "Left", "Right" \
}

// hcs node macros: root/input_config/keyConfig/keyInfoList
#define root_input_config_keyConfig_keyInfoList_exists 1
#define root_input_config_keyConfig_keyInfoList_nodeName "keyInfoList"
#define root_input_config_keyConfig_keyInfoList_foreach_child(func) \
    func(root_input_config_keyConfig_keyInfoList_key1); \
    func(root_input_config_keyConfig_keyInfoList_key2); \
    func(root_input_config_keyConfig_keyInfoList_key3);

#define root_input_config_keyConfig_keyInfoList_foreach_child_vargs(func, ...) \
    func(root_input_config_keyConfig_keyInfoList_key1, __VA_ARGS__); \
    func(root_input_config_keyConfig_keyInfoList_key2, __VA_ARGS__); \
    func(root_input_config_keyConfig_keyInfoList_key3, __VA_ARGS__);


// hcs node macros: root/input_config/keyConfig/keyInfoList/key1
#define root_input_config_keyConfig_keyInfoList_key1_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_nodeName "key1"
#define root_input_config_keyConfig_keyInfoList_key1_match_attr_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_match_attr "key_device0"
#define root_input_config_keyConfig_keyInfoList_key1_inputType_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_inputType 1
#define root_input_config_keyConfig_keyInfoList_key1_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_keyName "power"
#define root_input_config_keyConfig_keyInfoList_key1_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_gpioNum 1
#define root_input_config_keyConfig_keyInfoList_key1_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_irqFlag 3
#define root_input_config_keyConfig_keyInfoList_key1_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_debounceTime 80

// hcs node macros: root/input_config/keyConfig/keyInfoList/key2
#define root_input_config_keyConfig_keyInfoList_key2_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_nodeName "key2"
#define root_input_config_keyConfig_keyInfoList_key2_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_keyName "volUp"
#define root_input_config_keyConfig_keyInfoList_key2_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_gpioNum 31
#define root_input_config_keyConfig_keyInfoList_key2_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_irqFlag 1
#define root_input_config_keyConfig_keyInfoList_key2_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_debounceTime 80

// hcs node macros: root/input_config/keyConfig/keyInfoList/key3
#define root_input_config_keyConfig_keyInfoList_key3_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_nodeName "key3"
#define root_input_config_keyConfig_keyInfoList_key3_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_keyName "volDown"
#define root_input_config_keyConfig_keyInfoList_key3_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_gpioNum 32
#define root_input_config_keyConfig_keyInfoList_key3_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_irqFlag 1
#define root_input_config_keyConfig_keyInfoList_key3_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_debounceTime 80

// hcs node macros: root/wlan_config
#define root_wlan_config_exists 1
#define root_wlan_config_nodeName "wlan_config"
#define root_wlan_config_foreach_child(func) \
    func(root_wlan_config_moduleConfig); \
    func(root_wlan_config_deviceList); \
    func(root_wlan_config_chipList);

#define root_wlan_config_foreach_child_vargs(func, ...) \
    func(root_wlan_config_moduleConfig, __VA_ARGS__); \
    func(root_wlan_config_deviceList, __VA_ARGS__); \
    func(root_wlan_config_chipList, __VA_ARGS__);

#define root_wlan_config_match_attr_exists 1
#define root_wlan_config_match_attr "hdf_wlan_driver"
#define root_wlan_config_hostChipName_exists 1
#define root_wlan_config_hostChipName "hi3516dv300"

// hcs node macros: root/wlan_config/moduleConfig
#define root_wlan_config_moduleConfig_exists 1
#define root_wlan_config_moduleConfig_nodeName "moduleConfig"
#define root_wlan_config_moduleConfig_foreach_child(func) \
    func(root_wlan_config_moduleConfig_Station); \
    func(root_wlan_config_moduleConfig_HostAp); \
    func(root_wlan_config_moduleConfig_P2P); \
    func(root_wlan_config_moduleConfig_Mac80211); \
    func(root_wlan_config_moduleConfig_Phy);

#define root_wlan_config_moduleConfig_foreach_child_vargs(func, ...) \
    func(root_wlan_config_moduleConfig_Station, __VA_ARGS__); \
    func(root_wlan_config_moduleConfig_HostAp, __VA_ARGS__); \
    func(root_wlan_config_moduleConfig_P2P, __VA_ARGS__); \
    func(root_wlan_config_moduleConfig_Mac80211, __VA_ARGS__); \
    func(root_wlan_config_moduleConfig_Phy, __VA_ARGS__);

#define root_wlan_config_moduleConfig_featureMap_exists 1
#define root_wlan_config_moduleConfig_featureMap 65535
#define root_wlan_config_moduleConfig_msgName_exists 1
#define root_wlan_config_moduleConfig_msgName "WalService"

// hcs node macros: root/wlan_config/moduleConfig/Station
#define root_wlan_config_moduleConfig_Station_exists 1
#define root_wlan_config_moduleConfig_Station_nodeName "Station"
#define root_wlan_config_moduleConfig_Station_name_exists 1
#define root_wlan_config_moduleConfig_Station_name "sta"
#define root_wlan_config_moduleConfig_Station_mode_exists 1
#define root_wlan_config_moduleConfig_Station_mode 1

// hcs node macros: root/wlan_config/moduleConfig/HostAp
#define root_wlan_config_moduleConfig_HostAp_exists 1
#define root_wlan_config_moduleConfig_HostAp_nodeName "HostAp"
#define root_wlan_config_moduleConfig_HostAp_name_exists 1
#define root_wlan_config_moduleConfig_HostAp_name "ap"
#define root_wlan_config_moduleConfig_HostAp_mode_exists 1
#define root_wlan_config_moduleConfig_HostAp_mode 0
#define root_wlan_config_moduleConfig_HostAp_vapResNum_exists 1
#define root_wlan_config_moduleConfig_HostAp_vapResNum 1
#define root_wlan_config_moduleConfig_HostAp_userResNum_exists 1
#define root_wlan_config_moduleConfig_HostAp_userResNum 8

// hcs node macros: root/wlan_config/moduleConfig/P2P
#define root_wlan_config_moduleConfig_P2P_exists 1
#define root_wlan_config_moduleConfig_P2P_nodeName "P2P"
#define root_wlan_config_moduleConfig_P2P_name_exists 1
#define root_wlan_config_moduleConfig_P2P_name "p2p"
#define root_wlan_config_moduleConfig_P2P_mode_exists 1
#define root_wlan_config_moduleConfig_P2P_mode 0

// hcs node macros: root/wlan_config/moduleConfig/Mac80211
#define root_wlan_config_moduleConfig_Mac80211_exists 1
#define root_wlan_config_moduleConfig_Mac80211_nodeName "Mac80211"
#define root_wlan_config_moduleConfig_Mac80211_mode_exists 1
#define root_wlan_config_moduleConfig_Mac80211_mode 1

// hcs node macros: root/wlan_config/moduleConfig/Phy
#define root_wlan_config_moduleConfig_Phy_exists 1
#define root_wlan_config_moduleConfig_Phy_nodeName "Phy"
#define root_wlan_config_moduleConfig_Phy_mode_exists 1
#define root_wlan_config_moduleConfig_Phy_mode 1

// hcs node macros: root/wlan_config/deviceList
#define root_wlan_config_deviceList_exists 1
#define root_wlan_config_deviceList_nodeName "deviceList"
#define root_wlan_config_deviceList_foreach_child(func) \
    func(root_wlan_config_deviceList_device0);

#define root_wlan_config_deviceList_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0, __VA_ARGS__);


// hcs node macros: root/wlan_config/deviceList/device0
#define root_wlan_config_deviceList_device0_exists 1
#define root_wlan_config_deviceList_device0_nodeName "device0"
#define root_wlan_config_deviceList_device0_foreach_child(func) \
    func(root_wlan_config_deviceList_device0_powers); \
    func(root_wlan_config_deviceList_device0_reset); \
    func(root_wlan_config_deviceList_device0_bus);

#define root_wlan_config_deviceList_device0_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0_powers, __VA_ARGS__); \
    func(root_wlan_config_deviceList_device0_reset, __VA_ARGS__); \
    func(root_wlan_config_deviceList_device0_bus, __VA_ARGS__);

#define root_wlan_config_deviceList_device0_deviceInstId_exists 1
#define root_wlan_config_deviceList_device0_deviceInstId 0

// hcs node macros: root/wlan_config/deviceList/device0/powers
#define root_wlan_config_deviceList_device0_powers_exists 1
#define root_wlan_config_deviceList_device0_powers_nodeName "powers"
#define root_wlan_config_deviceList_device0_powers_foreach_child(func) \
    func(root_wlan_config_deviceList_device0_powers_power0); \
    func(root_wlan_config_deviceList_device0_powers_power1);

#define root_wlan_config_deviceList_device0_powers_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0_powers_power0, __VA_ARGS__); \
    func(root_wlan_config_deviceList_device0_powers_power1, __VA_ARGS__);


// hcs node macros: root/wlan_config/deviceList/device0/powers/power0
#define root_wlan_config_deviceList_device0_powers_power0_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_nodeName "power0"
#define root_wlan_config_deviceList_device0_powers_power0_powerSeqDelay_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_powerSeqDelay 0
#define root_wlan_config_deviceList_device0_powers_power0_powerType_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_powerType 1
#define root_wlan_config_deviceList_device0_powers_power0_gpioId_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_gpioId 91
#define root_wlan_config_deviceList_device0_powers_power0_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_activeLevel 1

// hcs node macros: root/wlan_config/deviceList/device0/powers/power1
#define root_wlan_config_deviceList_device0_powers_power1_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_nodeName "power1"
#define root_wlan_config_deviceList_device0_powers_power1_powerSeqDelay_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_powerSeqDelay 0
#define root_wlan_config_deviceList_device0_powers_power1_powerType_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_powerType 0
#define root_wlan_config_deviceList_device0_powers_power1_gpioId_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_gpioId 1
#define root_wlan_config_deviceList_device0_powers_power1_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_activeLevel 1

// hcs node macros: root/wlan_config/deviceList/device0/reset
#define root_wlan_config_deviceList_device0_reset_exists 1
#define root_wlan_config_deviceList_device0_reset_nodeName "reset"
#define root_wlan_config_deviceList_device0_reset_resetType_exists 1
#define root_wlan_config_deviceList_device0_reset_resetType 1
#define root_wlan_config_deviceList_device0_reset_gpioId_exists 1
#define root_wlan_config_deviceList_device0_reset_gpioId 90
#define root_wlan_config_deviceList_device0_reset_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_reset_activeLevel 0
#define root_wlan_config_deviceList_device0_reset_resetHoldTime_exists 1
#define root_wlan_config_deviceList_device0_reset_resetHoldTime 16
#define root_wlan_config_deviceList_device0_bootUpTimeOut_exists 1
#define root_wlan_config_deviceList_device0_bootUpTimeOut 32

// hcs node macros: root/wlan_config/deviceList/device0/bus
#define root_wlan_config_deviceList_device0_bus_exists 1
#define root_wlan_config_deviceList_device0_bus_nodeName "bus"
#define root_wlan_config_deviceList_device0_bus_busEnable_exists 1
#define root_wlan_config_deviceList_device0_bus_busEnable 1
#define root_wlan_config_deviceList_device0_bus_busType_exists 1
#define root_wlan_config_deviceList_device0_bus_busType 0
#define root_wlan_config_deviceList_device0_bus_busIdx_exists 1
#define root_wlan_config_deviceList_device0_bus_busIdx 2
#define root_wlan_config_deviceList_device0_bus_funcNum_exists 1
#define root_wlan_config_deviceList_device0_bus_funcNum_array_size 1
#define root_wlan_config_deviceList_device0_bus_funcNum_data { \
    1 \
}
#define root_wlan_config_deviceList_device0_bus_timeout_exists 1
#define root_wlan_config_deviceList_device0_bus_timeout 1000
#define root_wlan_config_deviceList_device0_bus_blockSize_exists 1
#define root_wlan_config_deviceList_device0_bus_blockSize 512

// hcs node macros: root/wlan_config/chipList
#define root_wlan_config_chipList_exists 1
#define root_wlan_config_chipList_nodeName "chipList"
#define root_wlan_config_chipList_foreach_child(func) \
    func(root_wlan_config_chipList_chipHi3881);

#define root_wlan_config_chipList_foreach_child_vargs(func, ...) \
    func(root_wlan_config_chipList_chipHi3881, __VA_ARGS__);


// hcs node macros: root/wlan_config/chipList/chipHi3881
#define root_wlan_config_chipList_chipHi3881_exists 1
#define root_wlan_config_chipList_chipHi3881_nodeName "chipHi3881"
#define root_wlan_config_chipList_chipHi3881_foreach_child(func) \
    func(root_wlan_config_chipList_chipHi3881_bus);

#define root_wlan_config_chipList_chipHi3881_foreach_child_vargs(func, ...) \
    func(root_wlan_config_chipList_chipHi3881_bus, __VA_ARGS__);

#define root_wlan_config_chipList_chipHi3881_match_attr_exists 1
#define root_wlan_config_chipList_chipHi3881_match_attr "hdf_wlan_chips_hi3881"
#define root_wlan_config_chipList_chipHi3881_driverName_exists 1
#define root_wlan_config_chipList_chipHi3881_driverName "hisi"

// hcs node macros: root/wlan_config/chipList/chipHi3881/bus
#define root_wlan_config_chipList_chipHi3881_bus_exists 1
#define root_wlan_config_chipList_chipHi3881_bus_nodeName "bus"
#define root_wlan_config_chipList_chipHi3881_bus_vendorId_exists 1
#define root_wlan_config_chipList_chipHi3881_bus_vendorId 662
#define root_wlan_config_chipList_chipHi3881_bus_deviceId_exists 1
#define root_wlan_config_chipList_chipHi3881_bus_deviceId_array_size 1
#define root_wlan_config_chipList_chipHi3881_bus_deviceId_data { \
    21319 \
}

// hcs node macros: root/sensorConfig
#define root_sensorConfig_exists 1
#define root_sensorConfig_nodeName "sensorConfig"

// hcs node macros: root/accel_bmi160_chip_config
#define root_accel_bmi160_chip_config_exists 1
#define root_accel_bmi160_chip_config_nodeName "accel_bmi160_chip_config"
#define root_accel_bmi160_chip_config_foreach_child(func) \
    func(root_accel_bmi160_chip_config_sensorInfo); \
    func(root_accel_bmi160_chip_config_sensorBusConfig); \
    func(root_accel_bmi160_chip_config_sensorIdAttr); \
    func(root_accel_bmi160_chip_config_sensorDirection); \
    func(root_accel_bmi160_chip_config_sensorRegConfig);

#define root_accel_bmi160_chip_config_foreach_child_vargs(func, ...) \
    func(root_accel_bmi160_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_accel_bmi160_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_accel_bmi160_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_accel_bmi160_chip_config_sensorDirection, __VA_ARGS__); \
    func(root_accel_bmi160_chip_config_sensorRegConfig, __VA_ARGS__);

#define root_accel_bmi160_chip_config_match_attr_exists 1
#define root_accel_bmi160_chip_config_match_attr "hdf_sensor_accel_bmi160_driver"

// hcs node macros: root/accel_bmi160_chip_config/sensorInfo
#define root_accel_bmi160_chip_config_sensorInfo_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_accel_bmi160_chip_config_sensorInfo_sensorName_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorName "accelerometer"
#define root_accel_bmi160_chip_config_sensorInfo_vendorName_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_vendorName "borsh_bmi160"
#define root_accel_bmi160_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorTypeId 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorId_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorId 1
#define root_accel_bmi160_chip_config_sensorInfo_power_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_power 850
#define root_accel_bmi160_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_accel_bmi160_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_accel_bmi160_chip_config_sensorInfo_maxRange_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_maxRange 8
#define root_accel_bmi160_chip_config_sensorInfo_accuracy_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/accel_bmi160_chip_config/sensorBusConfig
#define root_accel_bmi160_chip_config_sensorBusConfig_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_accel_bmi160_chip_config_sensorBusConfig_busType_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busType 0
#define root_accel_bmi160_chip_config_sensorBusConfig_busNum_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busNum 6
#define root_accel_bmi160_chip_config_sensorBusConfig_busAddr_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busAddr 104
#define root_accel_bmi160_chip_config_sensorBusConfig_regWidth_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regWidth 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/accel_bmi160_chip_config/sensorIdAttr
#define root_accel_bmi160_chip_config_sensorIdAttr_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_accel_bmi160_chip_config_sensorIdAttr_chipName_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipName "bmi160"
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdRegister 0
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdValue 209

// hcs node macros: root/accel_bmi160_chip_config/sensorDirection
#define root_accel_bmi160_chip_config_sensorDirection_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_nodeName "sensorDirection"
#define root_accel_bmi160_chip_config_sensorDirection_direction_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_direction 5
#define root_accel_bmi160_chip_config_sensorDirection_convert_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_convert_array_size 48
#define root_accel_bmi160_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/accel_bmi160_chip_config/sensorRegConfig
#define root_accel_bmi160_chip_config_sensorRegConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_data { \
    126, 182, 255, 1, 5, 2, 0, 0, \
    0, 0, 126, 16, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_array_size 30
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_data { \
    126, 17, 255, 1, 5, 2, 0, 0, \
    0, 0, 65, 3, 255, 1, 0, 2, \
    0, 0, 0, 0, 64, 8, 255, 1, \
    0, 2, 0, 0, 0, 0 \
}
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_data { \
    126, 16, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/als_bh1745_chip_config
#define root_als_bh1745_chip_config_exists 1
#define root_als_bh1745_chip_config_nodeName "als_bh1745_chip_config"
#define root_als_bh1745_chip_config_foreach_child(func) \
    func(root_als_bh1745_chip_config_sensorInfo); \
    func(root_als_bh1745_chip_config_sensorBusConfig); \
    func(root_als_bh1745_chip_config_sensorIdAttr); \
    func(root_als_bh1745_chip_config_sensorRegConfig); \
    func(root_als_bh1745_chip_config_extendAlsRegConfig);

#define root_als_bh1745_chip_config_foreach_child_vargs(func, ...) \
    func(root_als_bh1745_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_als_bh1745_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_als_bh1745_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_als_bh1745_chip_config_sensorRegConfig, __VA_ARGS__); \
    func(root_als_bh1745_chip_config_extendAlsRegConfig, __VA_ARGS__);

#define root_als_bh1745_chip_config_match_attr_exists 1
#define root_als_bh1745_chip_config_match_attr "hdf_sensor_als_bh1745_driver"

// hcs node macros: root/als_bh1745_chip_config/sensorInfo
#define root_als_bh1745_chip_config_sensorInfo_exists 1
#define root_als_bh1745_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_als_bh1745_chip_config_sensorInfo_sensorName_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorName "als"
#define root_als_bh1745_chip_config_sensorInfo_vendorName_exists 1
#define root_als_bh1745_chip_config_sensorInfo_vendorName "rohm_bh1745"
#define root_als_bh1745_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorTypeId 5
#define root_als_bh1745_chip_config_sensorInfo_sensorId_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorId 5
#define root_als_bh1745_chip_config_sensorInfo_power_exists 1
#define root_als_bh1745_chip_config_sensorInfo_power 230
#define root_als_bh1745_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_als_bh1745_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_als_bh1745_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_als_bh1745_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_als_bh1745_chip_config_sensorInfo_maxRange_exists 1
#define root_als_bh1745_chip_config_sensorInfo_maxRange 8
#define root_als_bh1745_chip_config_sensorInfo_accuracy_exists 1
#define root_als_bh1745_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/als_bh1745_chip_config/sensorBusConfig
#define root_als_bh1745_chip_config_sensorBusConfig_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_als_bh1745_chip_config_sensorBusConfig_busType_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busType 0
#define root_als_bh1745_chip_config_sensorBusConfig_busNum_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busNum 6
#define root_als_bh1745_chip_config_sensorBusConfig_busAddr_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busAddr 56
#define root_als_bh1745_chip_config_sensorBusConfig_regWidth_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_regWidth 1
#define root_als_bh1745_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/als_bh1745_chip_config/sensorIdAttr
#define root_als_bh1745_chip_config_sensorIdAttr_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_als_bh1745_chip_config_sensorIdAttr_chipName_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipName "bh1745"
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdRegister 146
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdValue 224

// hcs node macros: root/als_bh1745_chip_config/sensorRegConfig
#define root_als_bh1745_chip_config_sensorRegConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_array_size 40
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_data { \
    64, 192, 255, 1, 5, 2, 0, 0, \
    0, 0, 66, 144, 255, 1, 5, 2, \
    0, 0, 0, 0, 68, 2, 255, 1, \
    5, 2, 0, 0, 0, 0, 65, 1, \
    255, 1, 5, 2, 0, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_array_size 20
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_data { \
    66, 16, 16, 1, 5, 4, 0, 0, \
    0, 0, 68, 2, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_data { \
    66, 0, 16, 1, 5, 4, 0, 0, \
    0, 0 \
}

// hcs node macros: root/als_bh1745_chip_config/extendAlsRegConfig
#define root_als_bh1745_chip_config_extendAlsRegConfig_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_nodeName "extendAlsRegConfig"
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_array_size 60
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_data { \
    65, 0, 7, 1, 5, 0, 1, 0, \
    0, 0, 65, 1, 7, 1, 5, 0, \
    1, 0, 0, 0, 65, 2, 7, 1, \
    5, 0, 1, 0, 0, 0, 65, 3, \
    7, 1, 5, 0, 1, 0, 0, 0, \
    65, 4, 7, 1, 5, 0, 1, 0, \
    0, 0, 65, 5, 7, 1, 5, 0, \
    1, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_array_size 30
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_data { \
    66, 0, 3, 1, 5, 0, 1, 0, \
    0, 0, 66, 1, 3, 1, 5, 0, \
    1, 0, 0, 0, 66, 2, 3, 1, \
    5, 0, 1, 0, 0, 0 \
}

// hcs node macros: root/gyro_bmi160_chip_config
#define root_gyro_bmi160_chip_config_exists 1
#define root_gyro_bmi160_chip_config_nodeName "gyro_bmi160_chip_config"
#define root_gyro_bmi160_chip_config_foreach_child(func) \
    func(root_gyro_bmi160_chip_config_sensorInfo); \
    func(root_gyro_bmi160_chip_config_sensorBusConfig); \
    func(root_gyro_bmi160_chip_config_sensorIdAttr); \
    func(root_gyro_bmi160_chip_config_sensorDirection); \
    func(root_gyro_bmi160_chip_config_sensorRegConfig);

#define root_gyro_bmi160_chip_config_foreach_child_vargs(func, ...) \
    func(root_gyro_bmi160_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_gyro_bmi160_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_gyro_bmi160_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_gyro_bmi160_chip_config_sensorDirection, __VA_ARGS__); \
    func(root_gyro_bmi160_chip_config_sensorRegConfig, __VA_ARGS__);

#define root_gyro_bmi160_chip_config_match_attr_exists 1
#define root_gyro_bmi160_chip_config_match_attr "hdf_sensor_gyro_bmi160_driver"

// hcs node macros: root/gyro_bmi160_chip_config/sensorInfo
#define root_gyro_bmi160_chip_config_sensorInfo_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_gyro_bmi160_chip_config_sensorInfo_sensorName_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorName "gyroscope"
#define root_gyro_bmi160_chip_config_sensorInfo_vendorName_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_vendorName "borsh_bmi160"
#define root_gyro_bmi160_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorTypeId 2
#define root_gyro_bmi160_chip_config_sensorInfo_sensorId_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorId 2
#define root_gyro_bmi160_chip_config_sensorInfo_power_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_power 180
#define root_gyro_bmi160_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_gyro_bmi160_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_gyro_bmi160_chip_config_sensorInfo_maxRange_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_maxRange 8
#define root_gyro_bmi160_chip_config_sensorInfo_accuracy_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/gyro_bmi160_chip_config/sensorBusConfig
#define root_gyro_bmi160_chip_config_sensorBusConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_gyro_bmi160_chip_config_sensorBusConfig_busType_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busType 0
#define root_gyro_bmi160_chip_config_sensorBusConfig_busNum_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busNum 6
#define root_gyro_bmi160_chip_config_sensorBusConfig_busAddr_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busAddr 104
#define root_gyro_bmi160_chip_config_sensorBusConfig_regWidth_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regWidth 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/gyro_bmi160_chip_config/sensorIdAttr
#define root_gyro_bmi160_chip_config_sensorIdAttr_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipName_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipName "bmi160"
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdRegister 0
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdValue 209

// hcs node macros: root/gyro_bmi160_chip_config/sensorDirection
#define root_gyro_bmi160_chip_config_sensorDirection_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_nodeName "sensorDirection"
#define root_gyro_bmi160_chip_config_sensorDirection_direction_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_direction 5
#define root_gyro_bmi160_chip_config_sensorDirection_convert_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_convert_array_size 48
#define root_gyro_bmi160_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/gyro_bmi160_chip_config/sensorRegConfig
#define root_gyro_bmi160_chip_config_sensorRegConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_data { \
    126, 182, 255, 1, 55, 2, 0, 0, \
    0, 0, 126, 20, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_array_size 30
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_data { \
    126, 21, 255, 1, 55, 2, 0, 0, \
    0, 0, 67, 0, 255, 1, 0, 4, \
    0, 0, 0, 0, 66, 8, 255, 1, \
    0, 2, 0, 0, 0, 0 \
}
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_data { \
    126, 20, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/barometer_bmp180_chip_config
#define root_barometer_bmp180_chip_config_exists 1
#define root_barometer_bmp180_chip_config_nodeName "barometer_bmp180_chip_config"
#define root_barometer_bmp180_chip_config_foreach_child(func) \
    func(root_barometer_bmp180_chip_config_sensorInfo); \
    func(root_barometer_bmp180_chip_config_sensorBusConfig); \
    func(root_barometer_bmp180_chip_config_sensorIdAttr); \
    func(root_barometer_bmp180_chip_config_sensorRegConfig);

#define root_barometer_bmp180_chip_config_foreach_child_vargs(func, ...) \
    func(root_barometer_bmp180_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_barometer_bmp180_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_barometer_bmp180_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_barometer_bmp180_chip_config_sensorRegConfig, __VA_ARGS__);

#define root_barometer_bmp180_chip_config_match_attr_exists 1
#define root_barometer_bmp180_chip_config_match_attr "hdf_sensor_barometer_bmp180_driver"

// hcs node macros: root/barometer_bmp180_chip_config/sensorInfo
#define root_barometer_bmp180_chip_config_sensorInfo_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_barometer_bmp180_chip_config_sensorInfo_sensorName_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorName "barometer"
#define root_barometer_bmp180_chip_config_sensorInfo_vendorName_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_vendorName "borsh_bmp180"
#define root_barometer_bmp180_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorTypeId 8
#define root_barometer_bmp180_chip_config_sensorInfo_sensorId_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorId 8
#define root_barometer_bmp180_chip_config_sensorInfo_power_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_power 650
#define root_barometer_bmp180_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_barometer_bmp180_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_barometer_bmp180_chip_config_sensorInfo_maxRange_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_maxRange 8
#define root_barometer_bmp180_chip_config_sensorInfo_accuracy_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/barometer_bmp180_chip_config/sensorBusConfig
#define root_barometer_bmp180_chip_config_sensorBusConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_barometer_bmp180_chip_config_sensorBusConfig_busType_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busType 0
#define root_barometer_bmp180_chip_config_sensorBusConfig_busNum_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busNum 6
#define root_barometer_bmp180_chip_config_sensorBusConfig_busAddr_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busAddr 119
#define root_barometer_bmp180_chip_config_sensorBusConfig_regWidth_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regWidth 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/barometer_bmp180_chip_config/sensorIdAttr
#define root_barometer_bmp180_chip_config_sensorIdAttr_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipName_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipName "bmp180"
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdRegister 208
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdValue 85

// hcs node macros: root/barometer_bmp180_chip_config/sensorRegConfig
#define root_barometer_bmp180_chip_config_sensorRegConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_array_size 10
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_data { \
    224, 182, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_barometer_bmp180_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_enableSeqConfig_array_size 0
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_data { \
    224, 182, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/hall_ak8789_chip_config
#define root_hall_ak8789_chip_config_exists 1
#define root_hall_ak8789_chip_config_nodeName "hall_ak8789_chip_config"
#define root_hall_ak8789_chip_config_foreach_child(func) \
    func(root_hall_ak8789_chip_config_sensorInfo); \
    func(root_hall_ak8789_chip_config_sensorBusConfig); \
    func(root_hall_ak8789_chip_config_sensorIdAttr); \
    func(root_hall_ak8789_chip_config_hallDelayedConfig);

#define root_hall_ak8789_chip_config_foreach_child_vargs(func, ...) \
    func(root_hall_ak8789_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_hall_ak8789_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_hall_ak8789_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_hall_ak8789_chip_config_hallDelayedConfig, __VA_ARGS__);

#define root_hall_ak8789_chip_config_match_attr_exists 1
#define root_hall_ak8789_chip_config_match_attr "hdf_sensor_hall_ak8789_driver"

// hcs node macros: root/hall_ak8789_chip_config/sensorInfo
#define root_hall_ak8789_chip_config_sensorInfo_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_hall_ak8789_chip_config_sensorInfo_sensorName_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorName "hallrometer"
#define root_hall_ak8789_chip_config_sensorInfo_vendorName_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_vendorName "akm_ak8789"
#define root_hall_ak8789_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorTypeId 10
#define root_hall_ak8789_chip_config_sensorInfo_sensorId_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorId 10
#define root_hall_ak8789_chip_config_sensorInfo_power_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_power 9
#define root_hall_ak8789_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_hall_ak8789_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_hall_ak8789_chip_config_sensorInfo_maxRange_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_maxRange 8
#define root_hall_ak8789_chip_config_sensorInfo_accuracy_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/hall_ak8789_chip_config/sensorBusConfig
#define root_hall_ak8789_chip_config_sensorBusConfig_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_hall_ak8789_chip_config_sensorBusConfig_busType_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busType 2
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq1_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq1 24
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq2_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq2 25
#define root_hall_ak8789_chip_config_sensorBusConfig_busNum_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busNum 6
#define root_hall_ak8789_chip_config_sensorBusConfig_busAddr_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busAddr 0
#define root_hall_ak8789_chip_config_sensorBusConfig_regWidth_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regWidth 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/hall_ak8789_chip_config/sensorIdAttr
#define root_hall_ak8789_chip_config_sensorIdAttr_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_hall_ak8789_chip_config_sensorIdAttr_chipName_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipName "ak8789"
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdRegister 0
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdValue 0

// hcs node macros: root/hall_ak8789_chip_config/hallDelayedConfig
#define root_hall_ak8789_chip_config_hallDelayedConfig_exists 1
#define root_hall_ak8789_chip_config_hallDelayedConfig_nodeName "hallDelayedConfig"
#define root_hall_ak8789_chip_config_hallDelayedConfig_delayedTime_exists 1
#define root_hall_ak8789_chip_config_hallDelayedConfig_delayedTime 100

// hcs node macros: root/magnetic_lsm303_chip_config
#define root_magnetic_lsm303_chip_config_exists 1
#define root_magnetic_lsm303_chip_config_nodeName "magnetic_lsm303_chip_config"
#define root_magnetic_lsm303_chip_config_foreach_child(func) \
    func(root_magnetic_lsm303_chip_config_sensorInfo); \
    func(root_magnetic_lsm303_chip_config_sensorBusConfig); \
    func(root_magnetic_lsm303_chip_config_sensorIdAttr); \
    func(root_magnetic_lsm303_chip_config_sensorDirection); \
    func(root_magnetic_lsm303_chip_config_sensorRegConfig);

#define root_magnetic_lsm303_chip_config_foreach_child_vargs(func, ...) \
    func(root_magnetic_lsm303_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_magnetic_lsm303_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_magnetic_lsm303_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_magnetic_lsm303_chip_config_sensorDirection, __VA_ARGS__); \
    func(root_magnetic_lsm303_chip_config_sensorRegConfig, __VA_ARGS__);

#define root_magnetic_lsm303_chip_config_match_attr_exists 1
#define root_magnetic_lsm303_chip_config_match_attr "hdf_sensor_magnetic_lsm303_driver"

// hcs node macros: root/magnetic_lsm303_chip_config/sensorInfo
#define root_magnetic_lsm303_chip_config_sensorInfo_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorName_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorName "magnetometer"
#define root_magnetic_lsm303_chip_config_sensorInfo_vendorName_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_vendorName "st_lsm303"
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorTypeId 6
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorId_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorId 6
#define root_magnetic_lsm303_chip_config_sensorInfo_power_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_power 110
#define root_magnetic_lsm303_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_magnetic_lsm303_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_magnetic_lsm303_chip_config_sensorInfo_maxRange_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_maxRange 8
#define root_magnetic_lsm303_chip_config_sensorInfo_accuracy_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/magnetic_lsm303_chip_config/sensorBusConfig
#define root_magnetic_lsm303_chip_config_sensorBusConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busType_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busType 0
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busNum_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busNum 6
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busAddr_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busAddr 30
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regWidth_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regWidth 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/magnetic_lsm303_chip_config/sensorIdAttr
#define root_magnetic_lsm303_chip_config_sensorIdAttr_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipName_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipName "lsm303"
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdRegister 10
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdValue 72

// hcs node macros: root/magnetic_lsm303_chip_config/sensorDirection
#define root_magnetic_lsm303_chip_config_sensorDirection_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_nodeName "sensorDirection"
#define root_magnetic_lsm303_chip_config_sensorDirection_direction_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_direction 0
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_array_size 48
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/magnetic_lsm303_chip_config/sensorRegConfig
#define root_magnetic_lsm303_chip_config_sensorRegConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_array_size 30
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_data { \
    0, 20, 255, 1, 5, 2, 0, 0, \
    0, 0, 1, 160, 255, 1, 5, 2, \
    0, 0, 0, 0, 2, 0, 255, 1, \
    5, 2, 0, 0, 0, 0 \
}
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_array_size 10
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_data { \
    2, 0, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_data { \
    2, 3, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/proximity_apds9960_chip_config
#define root_proximity_apds9960_chip_config_exists 1
#define root_proximity_apds9960_chip_config_nodeName "proximity_apds9960_chip_config"
#define root_proximity_apds9960_chip_config_foreach_child(func) \
    func(root_proximity_apds9960_chip_config_sensorInfo); \
    func(root_proximity_apds9960_chip_config_sensorBusConfig); \
    func(root_proximity_apds9960_chip_config_sensorIdAttr); \
    func(root_proximity_apds9960_chip_config_sensorRegConfig);

#define root_proximity_apds9960_chip_config_foreach_child_vargs(func, ...) \
    func(root_proximity_apds9960_chip_config_sensorInfo, __VA_ARGS__); \
    func(root_proximity_apds9960_chip_config_sensorBusConfig, __VA_ARGS__); \
    func(root_proximity_apds9960_chip_config_sensorIdAttr, __VA_ARGS__); \
    func(root_proximity_apds9960_chip_config_sensorRegConfig, __VA_ARGS__);

#define root_proximity_apds9960_chip_config_match_attr_exists 1
#define root_proximity_apds9960_chip_config_match_attr "hdf_sensor_proximity_adps9960_driver"

// hcs node macros: root/proximity_apds9960_chip_config/sensorInfo
#define root_proximity_apds9960_chip_config_sensorInfo_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_nodeName "sensorInfo"
#define root_proximity_apds9960_chip_config_sensorInfo_sensorName_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorName "proximity"
#define root_proximity_apds9960_chip_config_sensorInfo_vendorName_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_vendorName "adps9960"
#define root_proximity_apds9960_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorTypeId 12
#define root_proximity_apds9960_chip_config_sensorInfo_sensorId_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorId 12
#define root_proximity_apds9960_chip_config_sensorInfo_power_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_power 230
#define root_proximity_apds9960_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_proximity_apds9960_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_proximity_apds9960_chip_config_sensorInfo_maxRange_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_maxRange 8
#define root_proximity_apds9960_chip_config_sensorInfo_accuracy_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/proximity_apds9960_chip_config/sensorBusConfig
#define root_proximity_apds9960_chip_config_sensorBusConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_nodeName "sensorBusConfig"
#define root_proximity_apds9960_chip_config_sensorBusConfig_busType_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busType 0
#define root_proximity_apds9960_chip_config_sensorBusConfig_busNum_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busNum 6
#define root_proximity_apds9960_chip_config_sensorBusConfig_busAddr_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busAddr 57
#define root_proximity_apds9960_chip_config_sensorBusConfig_regWidth_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regWidth 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/proximity_apds9960_chip_config/sensorIdAttr
#define root_proximity_apds9960_chip_config_sensorIdAttr_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_nodeName "sensorIdAttr"
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipName_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipName "adps9960"
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdRegister 146
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdValue 171

// hcs node macros: root/proximity_apds9960_chip_config/sensorRegConfig
#define root_proximity_apds9960_chip_config_sensorRegConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_nodeName "sensorRegConfig"
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_data { \
    128, 5, 255, 1, 5, 2, 0, 0, \
    0, 0, 143, 0, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_array_size 10
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_data { \
    128, 5, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_data { \
    128, 0, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/vibratorConfig
#define root_vibratorConfig_exists 1
#define root_vibratorConfig_nodeName "vibratorConfig"
#define root_vibratorConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig);

#define root_vibratorConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig, __VA_ARGS__);


// hcs node macros: root/vibratorConfig/boardConfig
#define root_vibratorConfig_boardConfig_exists 1
#define root_vibratorConfig_boardConfig_nodeName "boardConfig"
#define root_vibratorConfig_boardConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig_vibratorAttr); \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig);

#define root_vibratorConfig_boardConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig_vibratorAttr, __VA_ARGS__); \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig, __VA_ARGS__);

#define root_vibratorConfig_boardConfig_match_attr_exists 1
#define root_vibratorConfig_boardConfig_match_attr "hdf_vibrator_driver"

// hcs node macros: root/vibratorConfig/boardConfig/vibratorAttr
#define root_vibratorConfig_boardConfig_vibratorAttr_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_nodeName "vibratorAttr"
#define root_vibratorConfig_boardConfig_vibratorAttr_deviceType_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_deviceType 1
#define root_vibratorConfig_boardConfig_vibratorAttr_supportPreset_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_supportPreset 1

// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_nodeName "vibratorHapticConfig"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer); \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect);

#define root_vibratorConfig_boardConfig_vibratorHapticConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer, __VA_ARGS__); \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect, __VA_ARGS__);


// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig/haptic_clock_timer
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_nodeName "haptic_clock_timer"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_effectName_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_effectName "haptic.clock.timer"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_type_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_type 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_array_size 4
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_data { \
    600, 600, 200, 600 \
}

// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig/haptic_default_effect
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_nodeName "haptic_default_effect"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_effectName_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_effectName "haptic.default.effect"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_type_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_type 0
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_array_size 4
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_data { \
    0, 3, 800, 1 \
}

// hcs node macros: root/linearVibratorConfig
#define root_linearVibratorConfig_exists 1
#define root_linearVibratorConfig_nodeName "linearVibratorConfig"
#define root_linearVibratorConfig_foreach_child(func) \
    func(root_linearVibratorConfig_boardConfig);

#define root_linearVibratorConfig_foreach_child_vargs(func, ...) \
    func(root_linearVibratorConfig_boardConfig, __VA_ARGS__);


// hcs node macros: root/linearVibratorConfig/boardConfig
#define root_linearVibratorConfig_boardConfig_exists 1
#define root_linearVibratorConfig_boardConfig_nodeName "boardConfig"
#define root_linearVibratorConfig_boardConfig_foreach_child(func) \
    func(root_linearVibratorConfig_boardConfig_VibratorBusConfig); \
    func(root_linearVibratorConfig_boardConfig_VibratorChipConfig);

#define root_linearVibratorConfig_boardConfig_foreach_child_vargs(func, ...) \
    func(root_linearVibratorConfig_boardConfig_VibratorBusConfig, __VA_ARGS__); \
    func(root_linearVibratorConfig_boardConfig_VibratorChipConfig, __VA_ARGS__);

#define root_linearVibratorConfig_boardConfig_match_attr_exists 1
#define root_linearVibratorConfig_boardConfig_match_attr "hdf_linear_vibrator_driver"

// hcs node macros: root/linearVibratorConfig/boardConfig/VibratorBusConfig
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_nodeName "VibratorBusConfig"
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_busType_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_busType 1
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_busNum_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorBusConfig_busNum 31

// hcs node macros: root/linearVibratorConfig/boardConfig/VibratorChipConfig
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_nodeName "VibratorChipConfig"
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_isSupportIntensity_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_isSupportIntensity 0
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_isSupportFrequency_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_isSupportFrequency 0
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_intensityMaxValue_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_intensityMaxValue 0
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_intensityMinValue_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_intensityMinValue 0
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_frequencyMaxValue_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_frequencyMaxValue 0
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_frequencyMinValue_exists 1
#define root_linearVibratorConfig_boardConfig_VibratorChipConfig_frequencyMinValue 0
#define root_match_attr_exists 1
#define root_match_attr "test,config"

// hcs node macros: root/audio_info
#define root_audio_info_exists 1
#define root_audio_info_nodeName "audio_info"
#define root_audio_info_match_attr_exists 1
#define root_audio_info_match_attr "hw,hw_audio_info"
#define root_audio_info_pa_identifier_exists 1
#define root_audio_info_pa_identifier "smartpakit"
#define root_audio_info_smartpa_addr_exists 1
#define root_audio_info_smartpa_addr 65536
#define root_audio_info_smartpa_num_exists 1
#define root_audio_info_smartpa_num 1
#define root_audio_info_builtin_primary_mic_exist_exists 1
#define root_audio_info_builtin_primary_mic_exist 1
#define root_audio_info_builtin_second_mic_exist_exists 1
#define root_audio_info_builtin_second_mic_exist 1
#define root_audio_info_voice_vol_level_exists 1
#define root_audio_info_voice_vol_level 256
#define root_audio_info_cust_name_exists 1
#define root_audio_info_cust_name "audio_custom_v2"
#define root_audio_info_dual_smartpa_delay_exists 1
#define root_audio_info_dual_smartpa_delay "true"
#define root_audio_info_read_u64data_exists 1
#define root_audio_info_read_u64data 4294967296
#define root_audio_info_status_exists 1
#define root_audio_info_status "ok"

// hcs node macros: root/fingerprint_info
#define root_fingerprint_info_exists 1
#define root_fingerprint_info_nodeName "fingerprint_info"
#define root_fingerprint_info_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_one); \
    func(root_fingerprint_info_fingerprint_two);

#define root_fingerprint_info_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_one, __VA_ARGS__); \
    func(root_fingerprint_info_fingerprint_two, __VA_ARGS__);

#define root_fingerprint_info_match_attr_exists 1
#define root_fingerprint_info_match_attr "hw,hw_fingerprint_info"
#define root_fingerprint_info_read_u32_index_exists 1
#define root_fingerprint_info_read_u32_index_array_size 3
#define root_fingerprint_info_read_u32_index_data { \
    1, 256, 65536 \
}
#define root_fingerprint_info_finger_info_exists 1
#define root_fingerprint_info_finger_info root_fingerprint_info_fingerprint_one
#define root_fingerprint_info_audio_info_exists 1
#define root_fingerprint_info_audio_info root_audio_info

// hcs node macros: root/fingerprint_info/fingerprint_one
#define root_fingerprint_info_fingerprint_one_exists 1
#define root_fingerprint_info_fingerprint_one_nodeName "fingerprint_one"
#define root_fingerprint_info_fingerprint_one_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_one_one_child);

#define root_fingerprint_info_fingerprint_one_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_one_one_child, __VA_ARGS__);

#define root_fingerprint_info_fingerprint_one_product_exists 1
#define root_fingerprint_info_fingerprint_one_product "test"
#define root_fingerprint_info_fingerprint_one_chip_exists 1
#define root_fingerprint_info_fingerprint_one_chip "one"

// hcs node macros: root/fingerprint_info/fingerprint_one/one_child
#define root_fingerprint_info_fingerprint_one_one_child_exists 1
#define root_fingerprint_info_fingerprint_one_one_child_nodeName "one_child"
#define root_fingerprint_info_fingerprint_one_one_child_status_exists 1
#define root_fingerprint_info_fingerprint_one_one_child_status "ok"
#define root_fingerprint_info_fingerprint_one_status_exists 1
#define root_fingerprint_info_fingerprint_one_status "ok"
#define root_fingerprint_info_dual_fingerprint_exists 1
#define root_fingerprint_info_dual_fingerprint "false"

// hcs node macros: root/fingerprint_info/fingerprint_two
#define root_fingerprint_info_fingerprint_two_exists 1
#define root_fingerprint_info_fingerprint_two_nodeName "fingerprint_two"
#define root_fingerprint_info_fingerprint_two_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_two_one_child);

#define root_fingerprint_info_fingerprint_two_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_two_one_child, __VA_ARGS__);

#define root_fingerprint_info_fingerprint_two_chip_exists 1
#define root_fingerprint_info_fingerprint_two_chip "two"
#define root_fingerprint_info_fingerprint_two_status_exists 1
#define root_fingerprint_info_fingerprint_two_status "disable"
#define root_fingerprint_info_fingerprint_two_product_exists 1
#define root_fingerprint_info_fingerprint_two_product "test"

// hcs node macros: root/fingerprint_info/fingerprint_two/one_child
#define root_fingerprint_info_fingerprint_two_one_child_exists 1
#define root_fingerprint_info_fingerprint_two_one_child_nodeName "one_child"
#define root_fingerprint_info_fingerprint_two_one_child_status_exists 1
#define root_fingerprint_info_fingerprint_two_one_child_status "ok"
#define root_fingerprint_info_string_list_names_exists 1
#define root_fingerprint_info_string_list_names_array_size 3
#define root_fingerprint_info_string_list_names_data { \
    "first", "second", "third" \
}
#define root_fingerprint_info_status_exists 1
#define root_fingerprint_info_status "ok"

// hcs node macros: root/data_type_test
#define root_data_type_test_exists 1
#define root_data_type_test_nodeName "data_type_test"
#define root_data_type_test_match_attr_exists 1
#define root_data_type_test_match_attr "hw,data_type_test"
#define root_data_type_test_read_four_data_type_exists 1
#define root_data_type_test_read_four_data_type_array_size 4
#define root_data_type_test_read_four_data_type_data { \
    1, 256, 65536, 4294967296 \
}
#define root_data_type_test_test_u8_elem_data_exists 1
#define root_data_type_test_test_u8_elem_data_array_size 8
#define root_data_type_test_test_u8_elem_data_data { \
    0, 1, 2, 3, 4, 5, 6, 7 \
}
#define root_data_type_test_test_u16_elem_data_exists 1
#define root_data_type_test_test_u16_elem_data_array_size 8
#define root_data_type_test_test_u16_elem_data_data { \
    0, 1, 2, 3, 4, 5, 256, 257 \
}
#define root_board_id_exists 1
#define root_board_id_array_size 2
#define root_board_id_data { \
    65536, 256 \
}
#define root_support_Device_exists 1
#define root_support_Device "TP LCD Sensor"

// hcs node macros: root/fingerprint_three
#define root_fingerprint_three_exists 1
#define root_fingerprint_three_nodeName "fingerprint_three"
#define root_fingerprint_three_foreach_child(func) \
    func(root_fingerprint_three_one_child);

#define root_fingerprint_three_foreach_child_vargs(func, ...) \
    func(root_fingerprint_three_one_child, __VA_ARGS__);

#define root_fingerprint_three_chip_exists 1
#define root_fingerprint_three_chip "three"
#define root_fingerprint_three_status_exists 1
#define root_fingerprint_three_status "disable"
#define root_fingerprint_three_product_exists 1
#define root_fingerprint_three_product "test"

// hcs node macros: root/fingerprint_three/one_child
#define root_fingerprint_three_one_child_exists 1
#define root_fingerprint_three_one_child_nodeName "one_child"
#define root_fingerprint_three_one_child_status_exists 1
#define root_fingerprint_three_one_child_status "ok"
#define root_modem_id_exists 1
#define root_modem_id_array_size 2
#define root_modem_id_data { \
    4138991926, 0 \
}
#define root_boardId_exists 1
#define root_boardId 8000

// hcs node macros: root/gpio_config
#define root_gpio_config_exists 1
#define root_gpio_config_nodeName "gpio_config"
#define root_gpio_config_foreach_child(func) \
    func(root_gpio_config_gpio_controller_tester);

#define root_gpio_config_foreach_child_vargs(func, ...) \
    func(root_gpio_config_gpio_controller_tester, __VA_ARGS__);


// hcs node macros: root/gpio_config/gpio_controller_tester
#define root_gpio_config_gpio_controller_tester_exists 1
#define root_gpio_config_gpio_controller_tester_nodeName "gpio_controller_tester"
#define root_gpio_config_gpio_controller_tester_match_attr_exists 1
#define root_gpio_config_gpio_controller_tester_match_attr "GPIO_TEST"
#define root_gpio_config_gpio_controller_tester_testNameOne_exists 1
#define root_gpio_config_gpio_controller_tester_testNameOne "GPIO10_3"
#define root_gpio_config_gpio_controller_tester_testNameTwo_exists 1
#define root_gpio_config_gpio_controller_tester_testNameTwo "GPIO10_4"
#define root_gpio_config_gpio_controller_tester_gpio_exists 1
#define root_gpio_config_gpio_controller_tester_gpio 83
#define root_gpio_config_gpio_controller_tester_gpioTestTwo_exists 1
#define root_gpio_config_gpio_controller_tester_gpioTestTwo 84
#define root_gpio_config_gpio_controller_tester_gpioIrq_exists 1
#define root_gpio_config_gpio_controller_tester_gpioIrq 83
#define root_gpio_config_gpio_controller_tester_testUserApi_exists 1
#define root_gpio_config_gpio_controller_tester_testUserApi 1

// hcs node macros: root/i2c_config
#define root_i2c_config_exists 1
#define root_i2c_config_nodeName "i2c_config"
#define root_i2c_config_foreach_child(func) \
    func(root_i2c_config_i2c_controller_tester);

#define root_i2c_config_foreach_child_vargs(func, ...) \
    func(root_i2c_config_i2c_controller_tester, __VA_ARGS__);


// hcs node macros: root/i2c_config/i2c_controller_tester
#define root_i2c_config_i2c_controller_tester_exists 1
#define root_i2c_config_i2c_controller_tester_nodeName "i2c_controller_tester"
#define root_i2c_config_i2c_controller_tester_match_attr_exists 1
#define root_i2c_config_i2c_controller_tester_match_attr "I2C_TEST"
#define root_i2c_config_i2c_controller_tester_bus_num_exists 1
#define root_i2c_config_i2c_controller_tester_bus_num 6
#define root_i2c_config_i2c_controller_tester_dev_addr_exists 1
#define root_i2c_config_i2c_controller_tester_dev_addr 93
#define root_i2c_config_i2c_controller_tester_reg_addr_exists 1
#define root_i2c_config_i2c_controller_tester_reg_addr 208
#define root_i2c_config_i2c_controller_tester_reg_len_exists 1
#define root_i2c_config_i2c_controller_tester_reg_len 2
#define root_i2c_config_i2c_controller_tester_buf_size_exists 1
#define root_i2c_config_i2c_controller_tester_buf_size 7

// hcs node macros: root/pwm_test
#define root_pwm_test_exists 1
#define root_pwm_test_nodeName "pwm_test"
#define root_pwm_test_module_exists 1
#define root_pwm_test_module "pwm_test"
#define root_pwm_test_match_attr_exists 1
#define root_pwm_test_match_attr "PWM_TEST"
#define root_pwm_test_num_exists 1
#define root_pwm_test_num 0
#define root_pwm_test_period_exists 1
#define root_pwm_test_period 999
#define root_pwm_test_duty_exists 1
#define root_pwm_test_duty 333
#define root_pwm_test_polarity_exists 1
#define root_pwm_test_polarity 0
#define root_pwm_test_status_exists 1
#define root_pwm_test_status 1

// hcs node macros: root/spi_test
#define root_spi_test_exists 1
#define root_spi_test_nodeName "spi_test"
#define root_spi_test_module_exists 1
#define root_spi_test_module "spi_test"
#define root_spi_test_match_attr_exists 1
#define root_spi_test_match_attr "SPI_TEST"
#define root_spi_test_bus_exists 1
#define root_spi_test_bus 0
#define root_spi_test_cs_exists 1
#define root_spi_test_cs 0
#define root_spi_test_len_exists 1
#define root_spi_test_len 8
#define root_spi_test_wbuf_exists 1
#define root_spi_test_wbuf_array_size 8
#define root_spi_test_wbuf_data { \
    161, 178, 195, 212, 229, 246, 120, 144 \
}

// hcs node macros: root/sdio_config
#define root_sdio_config_exists 1
#define root_sdio_config_nodeName "sdio_config"
#define root_sdio_config_foreach_child(func) \
    func(root_sdio_config_sdio_controller_tester);

#define root_sdio_config_foreach_child_vargs(func, ...) \
    func(root_sdio_config_sdio_controller_tester, __VA_ARGS__);


// hcs node macros: root/sdio_config/sdio_controller_tester
#define root_sdio_config_sdio_controller_tester_exists 1
#define root_sdio_config_sdio_controller_tester_nodeName "sdio_controller_tester"
#define root_sdio_config_sdio_controller_tester_match_attr_exists 1
#define root_sdio_config_sdio_controller_tester_match_attr "SDIO_TEST"
#define root_sdio_config_sdio_controller_tester_busNum_exists 1
#define root_sdio_config_sdio_controller_tester_busNum 2
#define root_sdio_config_sdio_controller_tester_funcNum_exists 1
#define root_sdio_config_sdio_controller_tester_funcNum 1
#define root_sdio_config_sdio_controller_tester_vendorId_exists 1
#define root_sdio_config_sdio_controller_tester_vendorId 662
#define root_sdio_config_sdio_controller_tester_deviceId_exists 1
#define root_sdio_config_sdio_controller_tester_deviceId 21319

// hcs node macros: root/emmc_config
#define root_emmc_config_exists 1
#define root_emmc_config_nodeName "emmc_config"
#define root_emmc_config_foreach_child(func) \
    func(root_emmc_config_emmc_controller_tester);

#define root_emmc_config_foreach_child_vargs(func, ...) \
    func(root_emmc_config_emmc_controller_tester, __VA_ARGS__);


// hcs node macros: root/emmc_config/emmc_controller_tester
#define root_emmc_config_emmc_controller_tester_exists 1
#define root_emmc_config_emmc_controller_tester_nodeName "emmc_controller_tester"
#define root_emmc_config_emmc_controller_tester_match_attr_exists 1
#define root_emmc_config_emmc_controller_tester_match_attr "EMMC_TEST"
#define root_emmc_config_emmc_controller_tester_busNum_exists 1
#define root_emmc_config_emmc_controller_tester_busNum 0
#define root_emmc_config_emmc_controller_tester_hostId_exists 1
#define root_emmc_config_emmc_controller_tester_hostId 0

// hcs node macros: root/uart_test
#define root_uart_test_exists 1
#define root_uart_test_nodeName "uart_test"
#define root_uart_test_module_exists 1
#define root_uart_test_module "uart_test"
#define root_uart_test_match_attr_exists 1
#define root_uart_test_match_attr "UART_TEST"
#define root_uart_test_port_exists 1
#define root_uart_test_port 1
#define root_uart_test_len_exists 1
#define root_uart_test_len 8
#define root_uart_test_wbuf_exists 1
#define root_uart_test_wbuf_array_size 8
#define root_uart_test_wbuf_data { \
    161, 178, 195, 212, 229, 246, 120, 144 \
}

// hcs node macros: root/regulator_test_config
#define root_regulator_test_config_exists 1
#define root_regulator_test_config_nodeName "regulator_test_config"
#define root_regulator_test_config_foreach_child(func) \
    func(root_regulator_test_config_regulator_controller_tester);

#define root_regulator_test_config_foreach_child_vargs(func, ...) \
    func(root_regulator_test_config_regulator_controller_tester, __VA_ARGS__);


// hcs node macros: root/regulator_test_config/regulator_controller_tester
#define root_regulator_test_config_regulator_controller_tester_exists 1
#define root_regulator_test_config_regulator_controller_tester_nodeName "regulator_controller_tester"
#define root_regulator_test_config_regulator_controller_tester_match_attr_exists 1
#define root_regulator_test_config_regulator_controller_tester_match_attr "REGULATOR_TEST"
#define root_regulator_test_config_regulator_controller_tester_name_exists 1
#define root_regulator_test_config_regulator_controller_tester_name "regulator_adapter_1"
#define root_regulator_test_config_regulator_controller_tester_devName_exists 1
#define root_regulator_test_config_regulator_controller_tester_devName "regulator_adapter_consumer01"
#define root_regulator_test_config_regulator_controller_tester_supplyName_exists 1
#define root_regulator_test_config_regulator_controller_tester_supplyName "vir-voltage-reg-hdf-adp"
#define root_regulator_test_config_regulator_controller_tester_mode_exists 1
#define root_regulator_test_config_regulator_controller_tester_mode 1
#define root_regulator_test_config_regulator_controller_tester_minUv_exists 1
#define root_regulator_test_config_regulator_controller_tester_minUv 2000
#define root_regulator_test_config_regulator_controller_tester_maxUv_exists 1
#define root_regulator_test_config_regulator_controller_tester_maxUv 3000
#define root_regulator_test_config_regulator_controller_tester_minUa_exists 1
#define root_regulator_test_config_regulator_controller_tester_minUa 2000
#define root_regulator_test_config_regulator_controller_tester_maxUa_exists 1
#define root_regulator_test_config_regulator_controller_tester_maxUa 3000

// hcs node macros: root/rtc_config
#define root_rtc_config_exists 1
#define root_rtc_config_nodeName "rtc_config"
#define root_rtc_config_match_attr_exists 1
#define root_rtc_config_match_attr "RTC_TEST"
#define root_rtc_config_time_exists 1
#define root_rtc_config_time 60
#define root_rtc_config_maxYear_exists 1
#define root_rtc_config_maxYear 2222
#define root_rtc_config_year_exists 1
#define root_rtc_config_year 2020
#define root_rtc_config_month_exists 1
#define root_rtc_config_month 8
#define root_rtc_config_day_exists 1
#define root_rtc_config_day 8
#define root_rtc_config_hour_exists 1
#define root_rtc_config_hour 8
#define root_rtc_config_minute_exists 1
#define root_rtc_config_minute 8
#define root_rtc_config_second_exists 1
#define root_rtc_config_second 8
#define root_rtc_config_frequency_exists 1
#define root_rtc_config_frequency 32768
#define root_rtc_config_userValue_exists 1
#define root_rtc_config_userValue 8
#define root_rtc_config_userMaxIndex_exists 1
#define root_rtc_config_userMaxIndex 8
#define root_rtc_config_waitTimeSecond_exists 1
#define root_rtc_config_waitTimeSecond 3
#define root_rtc_config_writeWaitMillisecond_exists 1
#define root_rtc_config_writeWaitMillisecond 30

// hcs node macros: root/adc_config
#define root_adc_config_exists 1
#define root_adc_config_nodeName "adc_config"
#define root_adc_config_foreach_child(func) \
    func(root_adc_config_adc_device_tester);

#define root_adc_config_foreach_child_vargs(func, ...) \
    func(root_adc_config_adc_device_tester, __VA_ARGS__);


// hcs node macros: root/adc_config/adc_device_tester
#define root_adc_config_adc_device_tester_exists 1
#define root_adc_config_adc_device_tester_nodeName "adc_device_tester"
#define root_adc_config_adc_device_tester_match_attr_exists 1
#define root_adc_config_adc_device_tester_match_attr "ADC_TEST"
#define root_adc_config_adc_device_tester_devNum_exists 1
#define root_adc_config_adc_device_tester_devNum 2
#define root_adc_config_adc_device_tester_channel_exists 1
#define root_adc_config_adc_device_tester_channel 1
#define root_adc_config_adc_device_tester_maxChannel_exists 1
#define root_adc_config_adc_device_tester_maxChannel 1
#define root_adc_config_adc_device_tester_dataWidth_exists 1
#define root_adc_config_adc_device_tester_dataWidth 10
#define root_adc_config_adc_device_tester_rate_exists 1
#define root_adc_config_adc_device_tester_rate 20000

// hcs node macros: root/watchdog_config
#define root_watchdog_config_exists 1
#define root_watchdog_config_nodeName "watchdog_config"
#define root_watchdog_config_foreach_child(func) \
    func(root_watchdog_config_watchdog_device_tester);

#define root_watchdog_config_foreach_child_vargs(func, ...) \
    func(root_watchdog_config_watchdog_device_tester, __VA_ARGS__);


// hcs node macros: root/watchdog_config/watchdog_device_tester
#define root_watchdog_config_watchdog_device_tester_exists 1
#define root_watchdog_config_watchdog_device_tester_nodeName "watchdog_device_tester"
#define root_watchdog_config_watchdog_device_tester_match_attr_exists 1
#define root_watchdog_config_watchdog_device_tester_match_attr "WATCHDOG_TEST"
#define root_watchdog_config_watchdog_device_tester_id_exists 1
#define root_watchdog_config_watchdog_device_tester_id 0
#define root_watchdog_config_watchdog_device_tester_timeoutSet_exists 1
#define root_watchdog_config_watchdog_device_tester_timeoutSet 2
#define root_watchdog_config_watchdog_device_tester_statusStop_exists 1
#define root_watchdog_config_watchdog_device_tester_statusStop 0
#define root_watchdog_config_watchdog_device_tester_statusStart_exists 1
#define root_watchdog_config_watchdog_device_tester_statusStart 0
#define root_watchdog_config_watchdog_device_tester_feedTime_exists 1
#define root_watchdog_config_watchdog_device_tester_feedTime 6

#endif // HCS_CONFIG_HDF_MACRO_TEST_HEADER_H
