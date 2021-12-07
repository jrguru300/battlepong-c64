#ifndef _battlepong_convenience_rutins_h_
#define _battlepong_convenience_rutins_h_

int constrain_int(int input, int min, int max){ // no overload in c :(
  int output;
  output = input;
  if (input<min) output=min;
  if (input>max) output=max;
  return output;
};

unsigned char constrain_char(unsigned char input, unsigned char min, unsigned char max){
  unsigned char output;
  output = input;
  if (input<=min) output=min;
  if (input>=max) output=max;
  return output;
};

#endif
