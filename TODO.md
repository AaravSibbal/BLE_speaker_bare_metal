1. change gpio_t to not include pin:
    - this is because gpio needs to statically allocate gpio_t's memory
    - with only ports gpio's memory is 9 gpio
    - with pins and ports it becomes 9*15 which is too much
2. now architure descision no peripheral will own gpio in its struct
this is because it shouldn't. Now after the change of what gpio is i.e. a port, no thing can own the whole port thus it should have the port and pin and call the gpio function according to that but not the other way around. 
