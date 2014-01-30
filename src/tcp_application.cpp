/**
 ******************************************************************************
 * @file    application.cpp
 * @authors  Satish Nair, Zachary Crockett and Mohit Bhoite
 * @version V1.0.0
 * @date    05-November-2013
 * @brief   Tinker application
 ******************************************************************************
  Copyright (c) 2013 Spark Labs, Inc.  All rights reserved.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/  
#include "application.h"
extern "C" void debug(const char *p);
void debug(int v);
void debug_time(const char *p);


#define RENEW_INTERVAL      5*1000      // 30 secs
#define RETRY_INTERVAL      5*1000      // 10 secs
#define RESPONSE_INTERVAL   1*1000       // 1 sec
#define LET_IT_FILL_INTERVAL   3*1000       // 1 sec

TCPClient client;
char server[] = "nscdg.com";

// IO
int led = D2;

// Globals
volatile int state = 0;
volatile int wait = 0;
volatile int loopwait = 10;

volatile int tries = 0;
volatile int store = 0;
volatile int hash = 0;

volatile char command[32];
volatile int command_i=0;

UDP Udp;
void setup1()
{

    debug("Thermometer sketch");

    pinMode(led, OUTPUT);
    // Button resistorless
    state = 0;
    wait = RETRY_INTERVAL;
    Udp.beginPacket(8738);
    // Connecting
}
uint8_t buffer[TCPCLIENT_BUF_MAX_SIZE+1]; // for EOT
int loops = 0;
int total = 0;
void loop1()
{
      delay(1);
      if (++loops == 100) {
          loops = 0;
        debug_time("parsePacket");
        int packetSize = Udp.parsePacket();
        debug(packetSize);
        debug(" Received packet of size ");
        if(packetSize > 0)
        {
          debug_time("read");
          int count = Udp.read(buffer,sizeof(buffer));
          buffer[count] = '\0';
          total += count;
          debug(count);
          debug (" Bytes Read");
          debug ((const char*)buffer);
        }
      }
}



/* Function prototypes -------------------------------------------------------*/
int tinkerDigitalRead(String pin);
int tinkerDigitalWrite(String command);
int tinkerAnalogRead(String pin);
int tinkerAnalogWrite(String command);

/* This function is called once at start up ----------------------------------*/
void setup()
{
	//Setup the Tinker application here
//        Serial1.begin(115200);
	//Register all the Tinker functions

        setup1();
	Spark.function("digitalread", tinkerDigitalRead);
	Spark.function("digitalwrite", tinkerDigitalWrite);

	Spark.function("analogread", tinkerAnalogRead);
	Spark.function("analogwrite", tinkerAnalogWrite);

}

/* This function loops forever --------------------------------------------*/

void debug_time(const char *p)
{
  Serial1.print(millis());
  Serial1.print(" ");
  Serial1.println(p);
}
void debug(const char *p)
{
 static boolean once = false;
 if (!once)
   {
     once = true;
     Serial1.begin(115200);
   }

 //prints time since program started
// Serial1.print(millis());
// Serial1.print(" ");
 Serial1.println(p);
}

void debug(int v)
{
 Serial1.print(v);
}

void loop()
{
  static int count  = 0;
  if (count) {
  count+= 5000;
  Serial1.print(count);
  delay(count);
  debug("end");
  }
  loop1();
	//This will run in a loop
}

/*******************************************************************************
 * Function Name  : tinkerDigitalRead
 * Description    : Reads the digital value of a given pin
 * Input          : Pin 
 * Output         : None.
 * Return         : Value of the pin (0 or 1) in INT type
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerDigitalRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT_PULLDOWN);
		return digitalRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT_PULLDOWN);
		return digitalRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerDigitalWrite
 * Description    : Sets the specified pin HIGH or LOW
 * Input          : Pin and value
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerDigitalWrite(String command)
{
	bool value = 0;
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(command.substring(3,7) == "HIGH") value = 1;
	else if(command.substring(3,6) == "LOW") value = 0;
	else return -2;

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		digitalWrite(pinNumber, value);
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		digitalWrite(pinNumber+10, value);
		return 1;
	}
	else return -3;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogRead
 * Description    : Reads the analog value of a pin
 * Input          : Pin 
 * Output         : None.
 * Return         : Returns the analog value in INT type (0 to 4095)
                    Returns a negative number on failure
 *******************************************************************************/
int tinkerAnalogRead(String pin)
{
	//convert ascii to integer
	int pinNumber = pin.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	if(pin.startsWith("D"))
	{
		pinMode(pinNumber, INPUT);
		return analogRead(pinNumber);
	}
	else if (pin.startsWith("A"))
	{
		pinMode(pinNumber+10, INPUT);
		return analogRead(pinNumber+10);
	}
	return -2;
}

/*******************************************************************************
 * Function Name  : tinkerAnalogWrite
 * Description    : Writes an analog value (PWM) to the specified pin
 * Input          : Pin and Value (0 to 255)
 * Output         : None.
 * Return         : 1 on success and a negative number on failure
 *******************************************************************************/
int tinkerAnalogWrite(String command)
{
	//convert ascii to integer
	int pinNumber = command.charAt(1) - '0';
	//Sanity check to see if the pin numbers are within limits
	if (pinNumber< 0 || pinNumber >7) return -1;

	String value = command.substring(3);

	if(command.startsWith("D"))
	{
		pinMode(pinNumber, OUTPUT);
		analogWrite(pinNumber, value.toInt());
		return 1;
	}
	else if(command.startsWith("A"))
	{
		pinMode(pinNumber+10, OUTPUT);
		analogWrite(pinNumber+10, value.toInt());
		return 1;
	}
	else return -2;
}
