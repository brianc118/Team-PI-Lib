/*
DebugUtils.h - Simple debugging utilities.

Only compatible with Teensyduino, as printf function is implemented.
Variable number of arguments implemented as well.

Based on Fabio Varesano's original library
http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1271517197

(c) Brian Chen 2014

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#ifdef DEBUG_V 
#include <WProgram.h>
  #define DEBUG_PRINT(str)    \
    Serial.print(millis());     \
    Serial.print(": ");    \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print(' ');      \
    Serial.print(__FILE__);     \
    Serial.print(':');      \
    Serial.print(__LINE__);     \
    Serial.print(' ');      \
    Serial.print(str);
  #define DEBUG_PRINTLN(str)    \
    Serial.print(millis());     \
    Serial.print(":\t");    \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print('\t');      \
    Serial.print(__FILE__);     \
    Serial.print(':');      \
    Serial.print(__LINE__);     \
    Serial.print(' ');      \
    Serial.println(str);
  #define DEBUG_PRINTF(str, ...)    \
    Serial.print(millis());     \
    Serial.print(":\t");    \
    Serial.print(__PRETTY_FUNCTION__); \
    Serial.print('\t');      \
    Serial.print(__FILE__);     \
    Serial.print(':');      \
    Serial.print(__LINE__);     \
    Serial.print('\t');      \
    Serial.printf(str, ##__VA_ARGS__);
        //Serial.printf(strcat("%d: %s %s: %s\t", str), millis(), __PRETTY_FUNCTION__, __FILE__, __LINE__); 	//Teensy only
#endif
 
#ifdef DEBUG
	#define DEBUG_PRINT(str) 	Serial.print(str);
	#define DEBUG_PRINTLN(str) 	Serial.println(str);
	#define DEBUG_PRINTF(str, ...)	Serial.printf(str, ##__VA_ARGS__);		//Teensy only
#endif

#ifndef DEBUG_PRINT
	#define DEBUG_PRINT(str)
#endif

#ifndef DEBUG_PRINTLN
    #define DEBUG_PRINTLN(str)
#endif

#ifndef DEBUG_PRINTF
    #define DEBUG_PRINTF(str, ...) //Teensy only
#endif

#define PRINTARRAY(a){                                \
  Serial.print('{');                                \
  for (int i = 0; i < sizeof(a)/sizeof(a[0]); i++){ \
    Serial.print(a[i]);                           \
    Serial.print('\t'); }                         \
  Serial.println('}'); }                            \

#define CLEARSERIAL(){                          \
  while(Serial.available()){ Serial.read(); }    \
}  \
  
/*
int availableMemory() {
  int size = 2048; // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}
*/


#endif //DEBUGUTILS_H
