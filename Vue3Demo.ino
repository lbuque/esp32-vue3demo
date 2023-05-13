/*
  FSWebServer - Example WebServer with FS backend for esp8266/esp32
  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the WebServer library for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  upload the contents of the data folder with MkSPIFFS Tool ("ESP32 Sketch Data Upload" in Tools menu in Arduino IDE)
  or you can upload the contents of a folder if you CD in that folder and run the following command:
  for file in `ls -A1`; do curl -F "file=@$PWD/$file" esp32fs.local/edit; done

  access the sample web page at http://esp32fs.local
  edit the page by going to http://esp32fs.local/edit
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

#define FILESYSTEM LittleFS
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM false
#define DBG_OUTPUT_PORT Serial

const char *ssid = "lewin";
const char *password = "88888888";
const char *host = "esp32s3";
WebServer server(80);
// holds the current upload
File fsUploadFile;

// format bytes
String formatBytes(size_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + "B";
    }
    else if (bytes < (1024 * 1024))
    {
        return String(bytes / 1024.0) + "KB";
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    }
    else
    {
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
    }
}

void setup(void)
{
    DBG_OUTPUT_PORT.begin(115200);
    DBG_OUTPUT_PORT.print("\n");
    DBG_OUTPUT_PORT.setDebugOutput(true);
    if (FORMAT_FILESYSTEM)
        FILESYSTEM.format();
    FILESYSTEM.begin();
    {
        File root = FILESYSTEM.open("/");
        File file = root.openNextFile();
        while (file)
        {
            String fileName = file.name();
            size_t fileSize = file.size();
            DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
            file = root.openNextFile();
        }
        DBG_OUTPUT_PORT.printf("\n");
    }

    // WIFI INIT
    DBG_OUTPUT_PORT.printf("Connecting to %s\n", ssid);
    if (String(WiFi.SSID()) != String(ssid))
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DBG_OUTPUT_PORT.print(".");
    }
    DBG_OUTPUT_PORT.println("");
    DBG_OUTPUT_PORT.print("Connected! IP address: ");
    DBG_OUTPUT_PORT.println(WiFi.localIP());

    MDNS.begin(host);
    DBG_OUTPUT_PORT.print("Open http://");
    DBG_OUTPUT_PORT.print(host);
    DBG_OUTPUT_PORT.println(".local/edit to see the file browser");

    // SERVER INIT
    server.serveStatic("/", FILESYSTEM, "/www/index.html");
    server.serveStatic("/index.html", FILESYSTEM, "/www/index.html");
    server.serveStatic("/favicon.ico", FILESYSTEM, "/www/favicon.ico");
    server.serveStatic("/assets/index-2f827784.js", FILESYSTEM, "/www/assets/index-2f827784.js");
    server.serveStatic("/assets/index-9c1d568a.css", FILESYSTEM, "/www/assets/index-9c1d568a.css");
    server.serveStatic("/assets/logo-277e0e97.svg", FILESYSTEM, "/www/assets/logo-277e0e97.svg");

    // called when the url is not defined here
    // use it to load content from FILESYSTEM
    server.onNotFound([]()
                      { server.send(404, "text/plain", "FileNotFound"); });

    server.begin();
    DBG_OUTPUT_PORT.println("HTTP server started");
}

void loop(void)
{
    server.handleClient();
    delay(2); // allow the cpu to switch to other tasks
}
