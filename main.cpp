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
#include "TCPSocket.h"

/* Serial */
#define BAUDRATE 9600
Serial serial_pc(USBTX, USBRX, BAUDRATE);

/* DigitalOut */
#define LED_ON      0
#define LED_OFF     1
DigitalOut do_ledRed(LED_RED, LED_OFF);
DigitalOut do_ledGreen(LED_GREEN, LED_OFF);
DigitalOut do_ledBlue(LED_BLUE, LED_OFF);

/* Network */
WiFiInterface *wifi;



const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;

    serial_pc.printf("Scan:\n");

    int count = wifi->scan(NULL,0);

    if (count <= 0) {
        serial_pc.printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    /* Limit number of network arbitrary to 15 */
    count = count < 15 ? count : 15;

    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);

    if (count <= 0) {
        serial_pc.printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        serial_pc.printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    serial_pc.printf("%d networks available.\n", count);

    delete[] ap;
    return count;
}

void http_demo(NetworkInterface *net)
{
    TCPSocket socket;
    nsapi_error_t response;

    serial_pc.printf("Sending HTTP request to www.arm.com...\n");

    // Open a socket on the network interface, and create a TCP connection to www.arm.com
    response = socket.open(net);
    if(0 != response) {
        serial_pc.printf("socket.open() failed: %d\n", response);
        return;
    }

    response = socket.connect("api.ipify.org", 80);
    if(0 != response) {
        serial_pc.printf("Error connecting: %d\n", response);
        socket.close();
        return;
    }

    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    nsapi_size_t size = strlen(sbuffer);

    // Loop until whole request send
    while(size) {
        response = socket.send(sbuffer+response, size);
        if (response < 0) {
            serial_pc.printf("Error sending data: %d\n", response);
            socket.close();
            return;
        }
        size -= response;
        serial_pc.printf("sent %d [%.*s]\n", response, strstr(sbuffer, "\r\n")-sbuffer, sbuffer);
    }

    // Receieve a simple http response and print out the response line
    char rbuffer[64];
    response = socket.recv(rbuffer, sizeof rbuffer);
    if (response < 0) {
        serial_pc.printf("Error receiving data: %d\n", response);
    } else {
        serial_pc.printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
    }

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}

int main(void) {
    serial_pc.printf("< Sigma Delta Technologies Inc. >\n\r");

    serial_pc.printf("WiFi example\n");

#ifdef MBED_MAJOR_VERSION
    serial_pc.printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        serial_pc.printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    int count = scan_demo(wifi);
    if (count == 0) {
        serial_pc.printf("No WIFI APNs found - can't continue further.\n");
        return -1;
    }

    serial_pc.printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    serial_pc.printf("Success\n\n");
    serial_pc.printf("MAC: %s\n", wifi->get_mac_address());
    serial_pc.printf("IP: %s\n", wifi->get_ip_address());
    serial_pc.printf("Netmask: %s\n", wifi->get_netmask());
    serial_pc.printf("Gateway: %s\n", wifi->get_gateway());
    serial_pc.printf("RSSI: %d\n\n", wifi->get_rssi());

    http_demo(wifi);

    wifi->disconnect();

    serial_pc.printf("\nDone\n");

    while(true) {
        serial_pc.printf("LED Toggle\n");
        do_ledBlue = !do_ledBlue;
        wait(1);                    // 1sec
    }
}
