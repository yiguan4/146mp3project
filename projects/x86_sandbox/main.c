#include <stdio.h>

typedef struct {
  float f1; // 4 bytes
  char c1;  // 1 byte
  float f2;
  char c2;
} /*__attribute__((packed))*/ my_s;

// pass data structure as a copy
// values used come from struct block above
void struct_as_param(my_s s) {
  s.f1 = 4;
  s.f2 = 4;
  s.c1 = 1;
  s.c2 = 1;
}

int main() {
  // TODO: Instantiate a struct of type my_s with the name of "s"
  my_s s;
  printf("Size : %d bytes\n"
         "floats 0x%p 0x%p\n"
         "chars  0x%p 0x%p\n",
         sizeof(s), &s.f1, &s.f2, &s.c1, &s.c2);
}

/** HOMEWORK OBSERVATIONS
  NOTE: The assignment was completed without the SJTwo board

  When the code was compiled for both cases I received a warning
  stating - "warning: format ‘%d’ expects argument of type ‘int’,
            but argument 2 has type ‘long unsigned int’"
  %d is for functions and variables with an int type declaration, but
  where we are using it has a different type declaration which we created
  ourselves known as my_s using struct.

  When the packed attribute is commented, the compiler prints out
  the following statement:

      Size : 16 bytes
      floats 0x0x7fff537f4010 0x0x7fff537f4018
      chars  0x0x7fff537f4014 0x0x7fff537f401c
  In this case, there is padding between each variable to help control the
  number of CPU cycles that are accessing the variables.
  [f1_ _ _f2_ _ _c1 x x xc2 x x x] = 16 bytes used to store the variables (consider
  variable name as a space and 'x' represents an "empty" space)
  The float and char pointers will be different during each run depending where they 
  are being placed in memory. 

  When the packed attribute is uncommented, the compiler prints out
  the following statement:

      Size : 10 bytes
      floats 0x0x7fff7be11dde 0x0x7fff7be11de3
      chars  0x0x7fff7be11de2 0x0x7fff7be11de7
  In this case, there is no padding between the variables so the variables
  are placed as close together as possible.
  [f1_ _ _f2_ _ _c1c2] = 10 bytes used to store the variables (consider variable
  name as a space)
  The float and char pointers will be different during each run depending where they 
  are being placed in memory. 

  Reference used to better understand padding - https://youtu.be/aROgtACPjjg
**/