#include <iostream>
#include <stdio.h>
#include <unistd.h>  
#include "Uart.h"

using namespace std; 

u_int8_t data[4] = {0, 0, 0, 0};

int main(int argc, char *argv[])
{
  Uart uartESP32;

  string startToken = "<";
  string endToken = ">";
  string seperator = ":";

  while (1)
  {

    for (int i = 0; i < 4; i++)
    {
      data[i]++;
    }

    string dataString = startToken + to_string(data[0]) + seperator + to_string(data[1]) + seperator + to_string(data[2]) + seperator + to_string(data[3]) + endToken + "\n";
    auto chrs = dataString.c_str();
    auto uchrs = reinterpret_cast<unsigned char *>(const_cast<char *>(chrs));

    uartESP32.sendUart(uchrs);
    printf(chrs);
  }
  return 0;
}