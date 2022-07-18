#define I2C_BUS_AVAILABLE   (0)
#define SLAVE_DEVICE_NAME("gyros")
#define ADXL345_SLAVE_ADDR(0x53)

static struct i2c_adapter *gyro_i2c_adapter = NULL;
static struct i2c_client *gyro_i2c_client_gyro = NULL;

static struct i2c_board_info gyro_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, ADXL345_SLAVE_ADDR)
};

static int __init gyro_driver_init(void){
    gyro_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    gyro_i2c_client_gyro = i2c_new_device(gyro_i2c_adapter, &gyro_i2c_board_info);
    return 0;
}