/*
  Audio Debug Script: When loaded onto aurora, this script will publish the 
  loudest frequency detected on both of the audio channels. The gain switch
  on the microphone should be switched fully right before use. 

  Previously: 
	Example of use of the FFT libray to compute FFT for several signals over a range of frequencies.
        The exponent is calculated once before the excecution since it is a constant.
        This saves resources during the excecution of the sketch and reduces the compiled size.
        The sketch shows the time that the computing is taking.
        Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/

const uint16_t samples = 4096;
uint8_t exponent;

#define PIN_JACK 16
#define PIN_MIC 19
#define TIME_DELAY 100 // in milliseconds.

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

unsigned long time;

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void setup()
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Ready");
  exponent = FFT.Exponent(samples);
  Serial.println("Jack Frequency\tMic Frequency");
  Serial.println("=======================================\n");
}

void loop()
{
  /* Build raw data for the aux-jack*/
  double tic = micros();
  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] = int8_t(analogRead(PIN_JACK));
    vImag[i] = 0; //Reset the imaginary values vector for each new frequency
    delayMicroseconds(TIME_DELAY);
  }
  double toc = micros();
  int sampling = int(samples / ((toc - tic) / 1000000));
  

  time=millis();
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);	/* Weigh data */

  FFT.Compute(vReal, vImag, samples, exponent, FFT_FORWARD); /* Compute FFT */

  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

  double x_jack = FFT.MajorPeak(vReal, samples, sampling);

  //++++++++++++++++++++++++++++++++++++//

  /* Collect data for speaker */
  tic = micros();
  for (uint16_t i = 0; i < samples; i++)
  {
    vReal[i] = int8_t(analogRead(PIN_MIC));
    vImag[i] = 0; //Reset the imaginary values vector for each new frequency
    delayMicroseconds(TIME_DELAY);
  }
  toc = micros();
  sampling = int(samples / ((toc - tic) / 1000000));
  /*Serial.println("Data:");
  PrintVector(vReal, samples, SCL_TIME);*/
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */

  FFT.Compute(vReal, vImag, samples, exponent, FFT_FORWARD); /* Compute FFT */

  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

  double x_speaker = FFT.MajorPeak(vReal, samples, sampling);
  
  Serial.print(x_jack, 4);
  Serial.print("\t\t");
  Serial.print(x_speaker, 4);
  Serial.print("\t\t");
  Serial.println(" ");
}
