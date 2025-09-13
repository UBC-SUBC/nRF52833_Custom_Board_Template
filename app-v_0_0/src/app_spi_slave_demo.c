#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <ram_pwrdn.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>


LOG_MODULE_REGISTER(app_spi_slave_demo, LOG_LEVEL_INF);

#define MY_SPI_SLAVE  DT_NODELABEL(my_spi_slave)

const struct device *spi_slave_dev;
static struct k_poll_signal spi_slave_done_sig = K_POLL_SIGNAL_INITIALIZER(spi_slave_done_sig);

static const struct spi_config spi_slave_cfg = {
	.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_SLAVE,
	.frequency = 1000000,
	.slave = 0,
};

static void spi_slave_init(void)
{
	spi_slave_dev = DEVICE_DT_GET(MY_SPI_SLAVE);
	if(!device_is_ready(spi_slave_dev)) {
		printk("SPI slave device not ready!\n");
	}
}

static uint8_t slave_tx_buffer[2] = {0, 0};
static uint8_t slave_rx_buffer[2] = {0, 0};

const struct spi_buf s_tx_buf = {
	.buf = slave_tx_buffer,
	.len = sizeof(slave_tx_buffer)
};
const struct spi_buf_set s_tx = {
	.buffers = &s_tx_buf,
	.count = 1
};

struct spi_buf s_rx_buf = {
	.buf = slave_rx_buffer,
	.len = sizeof(slave_rx_buffer),
};
const struct spi_buf_set s_rx = {
	.buffers = &s_rx_buf,
	.count = 1
};

static int spi_slave_write_msg(void)
{
	k_poll_signal_reset(&spi_slave_done_sig);
	
	int error = spi_transceive_signal(spi_slave_dev, &spi_slave_cfg, &s_tx, &s_rx, &spi_slave_done_sig);
	if(error != 0){
		return error;
	}
	
	return 0;
}

static int spi_slave_check_for_message(void)
{
	int signaled, result;
	k_poll_signal_check(&spi_slave_done_sig, &signaled, &result);
	if(signaled != 0){
		return 0;
	}
	else return -1;
}

void get_spi_rx_buffer(uint8_t *rx_buffer) {
	LOG_INF("SPI SLAVE RX): 0x%.2x, 0x%.2x", slave_rx_buffer[0], slave_rx_buffer[1]);
	rx_buffer[0] = slave_rx_buffer[0];
	rx_buffer[1] = slave_rx_buffer[1];
    
}

void get_spi_tx_buffer(uint8_t *tx_buffer) {
	LOG_INF("SPI SLAVE TX): 0x%.2x, 0x%.2x", slave_rx_buffer[0], slave_rx_buffer[1]);
	tx_buffer[0] = slave_tx_buffer[0];
	tx_buffer[1] = slave_tx_buffer[1];
    
}

int main(void) {
	// uint16_t tx_buffer_16t = 0;
	// uint8_t *buffer_ptr = slave_tx_buffer;
    LOG_INF("Start APP");
    spi_slave_init();
	spi_slave_write_msg();
	LOG_INF("SPI SETUP COMPLETED");
	while (1) {
		if(spi_slave_check_for_message() == 0){
			spi_slave_write_msg();
		}
		k_msleep(10);
	}	

    return 0;
}