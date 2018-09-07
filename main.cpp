/* SDT-example-wifi
 * 
 * Copyright (c) 2018 Sigma Delta Technologies Inc.
 * 
 * MIT License
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mbed.h"
#include "easy-connect.h"

/* Serial */
#define BAUDRATE 9600
Serial g_Serial_pc(USBTX, USBRX, BAUDRATE);

/* DigitalOut */
#define LED_ON      0
#define LED_OFF     1
DigitalOut g_DO_LedRed(LED_RED, LED_OFF);
DigitalOut g_DO_LedGreen(LED_GREEN, LED_OFF);
DigitalOut g_DO_LedBlue(LED_BLUE, LED_OFF);

/* Network */
NetworkInterface* g_pNetwork;

int main(void) {
    g_Serial_pc.printf("< Sigma Delta Technologies Inc. >\n\r");

    g_pNetwork = easy_connect(true);    // 1 argument, enable_logging (pass in true to log to serial port)
    if (!g_pNetwork) {
        g_Serial_pc.printf("Connecting to the network failed\n");
        return 1;
    }

    while(true) {
        g_Serial_pc.printf("LED Toggle\n");
        g_DO_LedBlue = !g_DO_LedBlue;
        wait(1);                    // 1sec
    }

    return 0;
}
