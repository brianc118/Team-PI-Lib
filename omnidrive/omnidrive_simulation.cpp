/*
 * omnidrive_simulation.cpp - demonstration of omnidrive library usage
 *
 * by Brian Chen
 * (C) Team Pi 2014
 */

#include <iostream>
#include <fstream>
#include <string>
#include "omnidrive.h"

#define RGB(r,g,b) \
	(((31*(r+4))/255)<<11) |   \
	(((63*(g+2))/255)<<5)  |   \
	((31*(b+4))/255)


using namespace std;

int main ()
{
  ofstream file;	  // csv file object
  omnidrive omni;   // omnidrive object
  int v_x = 100, v_y = 0;  // x and y components of velocity

  // the following two functions should output the same wheel speeds
  omni.move(90 - atan2(v_y, v_x) * 180 / PI, sqrt(pow(v_x,2) + pow(v_y,2)), 0);  
  omni.moveCartesian(v_x, v_y, 0);

  int v = 100, alpha = 0;
  omni.move(alpha, v, 0);
  alpha = 90 - alpha;	 // convert bearing to angle
  omni.moveCartesian(v*cos(alpha * PI/180), v*sin(alpha * PI/180), 0);

  // generate motor outputs for angles [0, 360) in 1 degree intervals
  // cout << "generating motor outputs to csv";

  // file.open ("omnidrive_simulation.csv");
  // file << "A,B,C,D\n";  // write titles

  // for (int i = 0; i < 359; i++){
  //   omni.move(i, omni.getMaxV(i, 0), 0);
  // }
  uint16_t color = RGB(50,50,50);
  cout << color << endl;
  return 0;
}