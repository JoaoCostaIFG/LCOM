#include <lcom/lcf.h>

#include "include/err_utils.h"
#include "include/kbc_utils.h"
#include "include/utils.h"
#include "include/mouse.h"


/* GLOBAL VARIABLES */
static uint8_t mout_buf = 0;    /**< @brief	The latest packet fragment read from the mouse. */
static bool mbyte_valid = false;  /**< @brief	Indicates whether the latest packet fragment read from the mouse is valid. */

/* INTERRUPT HANDLER */
void
mouse_ih(void)
{
	/* get current byte on the output buffer */
	if (!check_status_read(true)) {  // check for errors (is mouse data)
		mbyte_valid = true;
		util_sys_inb(KBC_IO_BUF, &mout_buf);
	}
	else
		mbyte_valid = false;
}


/* GETTERS */
uint8_t
get_mout_buf(void)
{
	return mout_buf;
}

bool
get_mbyte_valid(void)
{
	return mbyte_valid;
}


int
mouse_sync_packet(struct packet *pp)
{
	static uint8_t curr_byte = 0;

	if (!pp)
		return 1;

	if (curr_byte > 0) {
		pp->bytes[curr_byte] = mout_buf;

		if (curr_byte == 2) {
			curr_byte = 0;

			/* parse packet info to 'struct packet' data type and print its info */
			mouse_parse_packet(pp);
			// mouse_print_packet(pp);
			return 0;
		}
		else
			++curr_byte;
	}
	else if (mout_buf & MOU_1ST_PACKET_BIT) {
		/* check if byte is a valid first byte (curr_byte == 0) */
		pp->bytes[curr_byte] = mout_buf;
		++curr_byte;
	}

	return -1;
}

void
mouse_parse_packet(struct packet *pp)
{
	if (!pp)
		return;

	/* mouse buttons */
	pp->lb = pp->bytes[0] & MOU_LB;
	pp->rb = pp->bytes[0] & MOU_RB;
	pp->mb = pp->bytes[0] & MOU_MB;

	/* getting the 16 bit cast of the '9 bit' signed int */
	pp->delta_x = ninebit2sixteen((pp->bytes[0] & MOU_MSB_X_DELTA) >> 4, pp->bytes[1]);
	pp->delta_y = ninebit2sixteen((pp->bytes[0] & MOU_MSB_Y_DELTA) >> 4, pp->bytes[2]);

	/* mouse overflows */
	pp->x_ov = pp->bytes[0] & MOU_X_OV;
	pp->y_ov = pp->bytes[0] & MOU_Y_OV;
}

int
mouse_set_stream_mode(void)
{
	/* set stream mode */
	if (mouse_write_cmd(MOU_SET_STREAM_MODE_CMD))
		return 1;

	/* in stream mode (disable data report) should be the first command sent (double checking) */
	if (mouse_write_cmd(MOU_DISABLE_DATA_REPORT_CMD))
		return 1;

	return 0;
}

int
mouse_en_data_report(void)
{
	/* set stream mode (mouse needs to be in stream mode in order to have data reporting enabled) */
	if (mouse_set_stream_mode())
		return 1;

	/* enable data reporting (stream mode only) */
	if (mouse_write_cmd(MOU_ENABLE_DATA_REPORT_CMD))
		return 1;

	return 0;
}
