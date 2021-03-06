/*
The OpenTRV project licenses this file to you
under the Apache Licence, Version 2.0 (the "Licence");
you may not use this file except in compliance
with the Licence. You may obtain a copy of the Licence at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the Licence is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied. See the Licence for the
specific language governing permissions and limitations
under the Licence.

Author(s) / Copyright (s): Damon Hart-Davis 2013
*/

/*
 Ambient light sensor module.
 */

#include "Ambient_Light_Sensor.h"
#include "V0p2_Board_IO_Config.h" // I/O pin allocation: include ahead of I/O module headers.
#include "Serial_Debug.h"
#include "Power_Management.h"

// This implementation expects LDR (1M dark resistance) from IO_POWER_UP to LDR_SENSOR_AIN and 100k to ground.
// Values below from PICAXE V0.09 impl approx multiplied by 4 to allow for scale change.
#ifdef LDR_EXTRA_SENSITIVE // Define if LDR not exposed to much light.
static const int LDR_THR_LOW = 20; // 5
static const int LDR_THR_HIGH = 32; // 8
#else // Normal settings.
static const int LDR_THR_LOW = 160; // Was 30.
static const int LDR_THR_HIGH = 200; // Was 35.
#endif

static bool isRoomLitFlag;

// Returns true if room/environs well enough lit for normal activity.
// Based on results of last call to readAmbientLight().
bool isRoomLit() { return(isRoomLitFlag); }

static int ambientLightLevel;

// Poll/update the current room ambient light level.
// This may consume significant power and time.
// Probably no need to do this more than (say) once per minute.
// This implementation expects LDR (1M dark resistance) from IO_POWER_UP to LDR_SENSOR_AIN and 100k to ground.
// (Not intended to be called from ISR.)
// Measure/store/return the current room ambient light levels.
int readAmbientLight()
  {
  power_intermittent_peripherals_enable(true);

  //analogReference(DEFAULT); // Force use of Vcc as reference.

  const int al = analogueNoiseReducedRead(LDR_SENSOR_AIN, DEFAULT);
  ambientLightLevel = al;
#if 1 && defined(DEBUG)
  DEBUG_SERIAL_PRINT_FLASHSTRING("Ambient light: ");
  DEBUG_SERIAL_PRINT(ambientLightLevel);
  DEBUG_SERIAL_PRINTLN();
#endif

  // Adjust room-lit flag, with hysteresis.
  if(al < LDR_THR_LOW) { isRoomLitFlag = false; }
  else if(al > LDR_THR_HIGH) { isRoomLitFlag = true; }
#if 0 && defined(DEBUG)
  DEBUG_SERIAL_PRINT_FLASHSTRING("isRoomLit: ");
  DEBUG_SERIAL_PRINT(isRoomLitFlag);
  DEBUG_SERIAL_PRINTLN();
#endif

  power_intermittent_peripherals_disable();

  return(al);
  }


