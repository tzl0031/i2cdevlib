/*
I2Cdev library collection - ADXL345 RPi example
Based on the example in Arduino/ADXL345/

==============================================
I2Cdev device library code is placed under the MIT license

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================

To compile on a Raspberry Pi (1 or 2)
  1. install the bcm2835 library, see
http://www.airspayce.com/mikem/bcm2835/index.html
  2. enable i2c on your RPi , see
https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c
  3. connect your i2c devices
  4. then from bash
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o ADXL345_example_1
"ADXL345_example_1.cpp" 163L, 5859C 2,1           Top
      $ PATH_I2CDEVLIB=~/i2cdevlib/
      $ gcc -o ADXL345_example_1
${PATH_I2CDEVLIB}RaspberryPi_bcm2835/ADXL345/examples/ADXL345_example_1.cpp \
         -I ${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev
${PATH_I2CDEVLIB}RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
         -I ${PATH_I2CDEVLIB}/Arduino/ADXL345/
${PATH_I2CDEVLIB}/Arduino/ADXL345/ADXL345.cpp -l bcm2835 -l m
      $ sudo ./ADXL345_example_1

*/

#include "ADXL345.h"
#include "Communicator.h"
#include "FileUtil.h"
#include "I2Cdev.h"
#include <bcm2835.h>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>


using namespace std;
struct timeval start_t, end_t_1, end_t_2, stop_t;
long long diff_1, diff_2, diff_3;
int msg_index = 1;
int16_t ax, ay, az, bx, by, bz;
int port;
class Communicator *comm = NULL;
FileUtil fileUtil;
Json::FastWriter fw;
Json::Value root_1, root_2;
Json::Value root_1, root_2;

int main(int argc, char **argv) {
  I2Cdev::initialize();
  ADXL345 a;
  ADXL345 b(ADXL345_ADDRESS_ALT_HIGH);
  if (a.testConnection()&& b.testConnection())
    printf("Both sensors' connection test successful\n");
  else {
    fprintf(stderr, "ADXL345 connection test failed! exiting ...\n");
    return 1;
  }
  a.initialize();
  b.initialize();
   cout << "current data rate of sensor_1 is " << int(a.getRate())<< endl;
   cout << "current data rate of sensor_2 is " << int(b.getRate()) << endl;
   a.setRate(15);
   b.setRate(10);
   cout << "data rate of sensor_1 after change " <<int(a.getRate()) << endl;
   cout << "data rate of sensor_2 after change " << int(b.getRate()) << endl;

   cout << "current data range of sensor_1 is " << int(a.getRange()) << endl;
   cout << "current data range of sensor_2 is " << int(b.getRange()) << endl;
   a.setRange(0);
   b.setRange(1);
   cout << "data range of sensor_1 after change" << int(a.getRange()) << endl;
   cout << "data range of sensor_2 after change" << int(b.getRange()) << endl;

  gettimeofday(&start_t, NULL);
  printf("start time : %lld\n", start_t.tv_sec * (uint64_t)1000000+ start_t.tv_usec);
  const char *rpi_id = fileUtil.getRpiID().c_str();
  const char *host = fileUtil.getHost().c_str();
  comm = new Communicator(rpi_id, host, port);

  while(1){
    a.getAcceleration(&ax, &ay, &az);
    fflush(stdout);
    gettimeofday(&end_t_1, NULL);
    b.getAcceleration(&bx, &by, &bz);
    fflush(stdout);
    gettimeofday(&end_t_2, NULL);
    diff_1 = (end_t_1.tv_sec - start_t.tv_sec) * (uint64_t)1000000 +
           (end_t_1.tv_usec - start_t.tv_usec);
    diff_2 = (end_t_2.tv_sec - start_t.tv_sec) * (uint64_t)1000000 +
            (end_t_2.tv_usec - start_t.tv_usec);
     root_1["rPi_id"] = rpi_id;
     root_1["sensor_id"] = 1;
     root_1["x_axis"] = ax;
     root_1["y_axis"] = ay;
     root_1["z_aixs"] = az;
     root_1["elapsed_time"] = diff_1;
     root_1["msg_index"] = msg_index;
//     cout << fw.write(root_1);
     root_2["rPi_id"] = rpi_id;
     root_2["sensor_id"] = 2;
     root_2["x_axis"] = bx;
     root_2["y_axis"] = by;
     root_2["z_aixs"] = bz;
    //     cout << fw.write(root_2);
     string json_1 = fw.write(root_1);
     string json_2 = fw.write(root_2);
     const char *j_1 = json_1.c_str();
     const char *j_2 = json_2.c_str();
//     //       publish to broker
     comm->send_message(j_1);
     comm->send_message(j_2);
//     // pthread_mutex_lock(&qlock);
     msg_index++;
     //printf("1, %lld\n", diff_1 );
     //printf("2, %lld\n", diff_2 );

  }
return 1
}