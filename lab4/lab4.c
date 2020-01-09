// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include "mouse.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
   /*lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");*/

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
   /*lcf_log_output("/home/lcom/labs/lab4/output.txt");*/

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
	if (mouse_packet(&cnt))
		return 1;

	return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
	if (mouse_remote(&period, &cnt))
		return 1;

	return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
	if (mouse_async(&idle_time))
		return 1;

	return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
	if (mouse_gesture(&x_len, &tolerance))
		return 1;

	return 0;
}
