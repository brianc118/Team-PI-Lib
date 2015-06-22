/*
 * sin.cpp
 *
 * by Brian Chen
 * (C) Team Pi 20145
 */

#include <iostream>
#include <string>
#include <math.h>

using namespace std;

double table[90] = {0};
int sin_table[90] = {0};

inline int sind(int deg){
  int sign;
  if (deg >= 0){
    sign = 1;
  }
  else{
    deg = -deg;
    sign = -1;
  }
  while (deg >= 360){
    deg -= 360;
  }
  if((deg >= 0) && (deg <= 90)){
    return sign * sin_table[deg];
  } 
  else if((deg > 90) && (deg <= 180)){
    return sign * sin_table[180 - deg];
  } 
  else if((deg > 180) && (deg <= 270)){
    return sign * -sin_table[deg - 180];
  } 
  else if((deg > 270) && (deg <= 360)){
    return sign * -sin_table[360 - deg];
  }
}
inline int cosd(int deg){
  //if (deg < 0){ deg = -deg; }   
  deg = (deg > 0 ? deg : -deg);
  while(deg >= 360){            
    deg -= 360;                 
  } 
  if ((deg >= 0) && (deg <= 90)){
    return sin_table[90 - deg];
  }
  else if ((deg > 90) && (deg <= 180)){
    return -sin_table[deg - 90];
  }
  else if ((deg > 180) && (deg <= 270)){
    return -sin_table[270 - deg];
  }
  else{
    return sin_table[deg - 270];
  }
}

int main ()
{
  
  for (int i = 0; i < 90; i++){
    table[i] = sin(i * 3.141592653589793238462643383279502884 / 180);
    table[i] *= 32767.0f;
    sin_table[i] = round(table[i]);
    cout << sin_table[i] << ',' << endl;
  }
  // cout << cosd(45) << endl;
  // cout << sind(-80) << endl;
  
  return 0;
}