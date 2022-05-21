#include "button.h"

void button_init(void) {
  select_button =
      gpio__construct_with_function(GPIO__PORT_1, 30, GPIO__FUNCITON_0_IO_PIN);
  down_button =
      gpio__construct_with_function(GPIO__PORT_1, 15, GPIO__FUNCITON_0_IO_PIN);
  right_button =
      gpio__construct_with_function(GPIO__PORT_1, 20, GPIO__FUNCITON_0_IO_PIN);
  up_button =
      gpio__construct_with_function(GPIO__PORT_1, 20, GPIO__FUNCITON_0_IO_PIN);
  left_button =
      gpio__construct_with_function(GPIO__PORT_1, 19, GPIO__FUNCITON_0_IO_PIN);
  playpause_button =
      gpio__construct_with_function(GPIO__PORT_0, 6, GPIO__FUNCITON_0_IO_PIN);
}
void button_set_as_input(void) {

  // gpio__set_as_input(extra_button);
  gpio__set_as_input(select_button);
  gpio__set_as_input(down_button);
  gpio__set_as_input(right_button);
  gpio__set_as_input(up_button);
  gpio__set_as_input(left_button);
  gpio__set_as_input(playpause_button);
}