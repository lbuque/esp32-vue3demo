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
#include <cJSON.h>
#include <Update.h>

#define FILESYSTEM LittleFS
// You only need to format the filesystem once
#define FORMAT_FILESYSTEM false
#define DBG_OUTPUT_PORT Serial

#define APP_VERSION "0.1.1"

String ssid = "";
String password = "";

String apSSID;

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

void onServeStaticSubDir(File &dir, String topDir)
{
    File file = dir.openNextFile();
    while (file)
    {
        String path = String(file.path());
        String uri = path.substring(path.indexOf(topDir) + topDir.length(), path.indexOf(".gz"));
        server.serveStatic(uri.c_str(), FILESYSTEM, path.c_str());
        DBG_OUTPUT_PORT.print("uri: ");
        DBG_OUTPUT_PORT.println(uri);
        DBG_OUTPUT_PORT.print("path: ");
        DBG_OUTPUT_PORT.println(path);
        file = dir.openNextFile();
    }
}

void onServeStatic(String dir)
{
    File wwwDir = FILESYSTEM.open(dir);

    if (wwwDir.isDirectory())
    {
        File file = wwwDir.openNextFile();
        while (file)
        {
            if (file.isDirectory())
            {
                file = FILESYSTEM.open(file.path());
                onServeStaticSubDir(file, dir);
            }
            else
            {
                String path = String(file.path());
                String uri = path.substring(path.indexOf(dir) + dir.length(), path.indexOf(".gz"));
                server.serveStatic(uri.c_str(), FILESYSTEM, path.c_str());
                DBG_OUTPUT_PORT.print("uri: ");
                DBG_OUTPUT_PORT.println(uri);
                DBG_OUTPUT_PORT.print("path: ");
                DBG_OUTPUT_PORT.println(path);
            }
            file = wwwDir.openNextFile();
        }
    }
}

void getStatus()
{
    cJSON *rspObject = NULL;
    cJSON *sysObject = NULL;
    cJSON *archObject = NULL;
    cJSON *memObject = NULL;
    cJSON *fsObject = NULL;
    cJSON *apObject = NULL;
    cJSON *staObject = NULL;

    rspObject = cJSON_CreateObject();
    if (rspObject == NULL)
    {
        goto OUT1;
    }

    sysObject = cJSON_CreateObject();
    if (sysObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "sys", sysObject);
    cJSON_AddStringToObject(sysObject, "model", "ESP32S3 Dev Module");
    cJSON_AddStringToObject(sysObject, "fw", APP_VERSION);
    cJSON_AddStringToObject(sysObject, "sdk", ESP.getSdkVersion());
    archObject = cJSON_CreateObject();
    if (archObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(sysObject, "arch", archObject);
    cJSON_AddStringToObject(archObject, "mfr", "Espressif");
    cJSON_AddStringToObject(archObject, "model", ESP.getChipModel());
    cJSON_AddNumberToObject(archObject, "revision", ESP.getChipRevision());
    if (!strncmp(ESP.getChipModel(), "ESP32-S3", strlen("ESP32-S3")))
    {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX7");
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32-S2", strlen("ESP32-S2")))
    {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® single-core LX7");
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32-C3", strlen("ESP32-C3")))
    {
        cJSON_AddStringToObject(archObject, "cpu", "RISC-V");
    }
    else if (!strncmp(ESP.getChipModel(), "ESP32", strlen("ESP32")))
    {
        cJSON_AddStringToObject(archObject, "cpu", "XTensa® dual-core LX6");
    }
    cJSON_AddNumberToObject(archObject, "freq", ESP.getCpuFreqMHz());

    memObject = cJSON_CreateObject();
    if (memObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "mem", memObject);
    cJSON_AddNumberToObject(memObject, "total", ESP.getHeapSize());
    cJSON_AddNumberToObject(memObject, "free", ESP.getFreeHeap());

    fsObject = cJSON_CreateObject();
    if (fsObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "fs", fsObject);
    cJSON_AddNumberToObject(fsObject, "total", FILESYSTEM.totalBytes());
    cJSON_AddNumberToObject(fsObject, "used", FILESYSTEM.usedBytes());
    cJSON_AddNumberToObject(fsObject, "free", FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes());

    apObject = cJSON_CreateObject();
    if (apObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "ap", apObject);
    cJSON_AddStringToObject(apObject, "ssid", WiFi.softAPSSID().c_str());
    cJSON_AddNumberToObject(apObject, "num", WiFi.softAPgetStationNum());

    staObject = cJSON_CreateObject();
    if (staObject == NULL)
    {
        goto OUT;
    }
    cJSON_AddItemToObject(rspObject, "sta", staObject);
    cJSON_AddStringToObject(staObject, "ssid", ssid.c_str());
    cJSON_AddStringToObject(staObject, "status", WiFi.isConnected() ? "connected" : "disconnect");

    server.send(200, "application/json", cJSON_Print(rspObject));
OUT:
    cJSON_Delete(rspObject);
OUT1:
    return;
}

void getConfig()
{
    File file = FILESYSTEM.open("/db.json", "r");
    size_t sent = server.streamFile(file, "application/json");
    file.close();
    return;
}

void postConfig()
{
    cJSON *reqObject = NULL;
    cJSON *wifiObject = NULL;
    bool flag = false;

    String content = server.arg("plain");
    reqObject = cJSON_Parse(content.c_str());
    if (reqObject == NULL)
    {
        Serial.println("JSON parse error");
        Serial.print("payload: ");
        Serial.println(server.arg("plain"));
        return;
    }

    wifiObject = cJSON_GetObjectItem(reqObject, "wifi");
    if (wifiObject)
    {
        cJSON *ssidObject = cJSON_GetObjectItem(wifiObject, "ssid");
        if (String(ssidObject->valuestring) != ssid)
        {
            ssid = ssidObject->valuestring;
            flag = true;
        }
        cJSON *psdObject = cJSON_GetObjectItem(wifiObject, "password");
        if (String(psdObject->valuestring) != password)
        {
            password = psdObject->valuestring;
            flag = true;
        }
    }

    File configfile = FILESYSTEM.open("/db.json", FILE_WRITE);
    configfile.write((const uint8_t *)content.c_str(), content.length());
    configfile.close();

    server.send(201, "application/json", server.arg("plain"));

    if (flag)
    {
        WiFi.disconnect();
        delay(1000);
        WiFi.begin(ssid.c_str(), password.c_str());
    }

    return;
}

void appLoadDateBase(void)
{

    DBG_OUTPUT_PORT.println("appLoadDateBase");

    File dbfile = FILESYSTEM.open("/db.json", "r");
    if (!dbfile)
    {
        DBG_OUTPUT_PORT.println("Error opening file.");
        return;
    }

    char *buffer = (char *)malloc(dbfile.size());

    while (dbfile.available())
    {
        dbfile.readBytes(buffer, dbfile.size());
    }

    DBG_OUTPUT_PORT.println(buffer);

    cJSON *rootObject = cJSON_ParseWithLength(buffer, dbfile.size());
    if (rootObject == NULL)
    {
        dbfile.close();
        return;
    }

    cJSON *wifiObject = cJSON_GetObjectItem(rootObject, "wifi");
    cJSON *ssidObject = cJSON_GetObjectItem(wifiObject, "ssid");
    cJSON *psdObject = cJSON_GetObjectItem(wifiObject, "password");
    ssid = String(ssidObject->valuestring);
    password = String(psdObject->valuestring);

    DBG_OUTPUT_PORT.print("ssid: ");
    DBG_OUTPUT_PORT.println(ssid);
    DBG_OUTPUT_PORT.print("password: ");
    DBG_OUTPUT_PORT.println(password);

    free(buffer);
    cJSON_Delete(rootObject);
    dbfile.close();

    DBG_OUTPUT_PORT.println("appLoadDateBase");
}

void wifiSetup()
{
    DBG_OUTPUT_PORT.println("wifiSetup");
    uint8_t mac[6] = {0};
    char ssidStr[32] = {0};

    WiFi.disconnect(true);
    delay(1000);

    Serial.println("WiFi: Set mode to WIFI_AP_STA");
    WiFi.mode(WIFI_AP_STA);
    // WiFi.onEvent(onWiFiEvent);

    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for WiFi to connect to AP
    Serial.println("Waiting for WiFi");

    WiFi.softAPmacAddress(mac);
    sprintf(ssidStr, "ESP-%02X%02X%02X", mac[3], mac[4], mac[5]);
    if (WiFi.softAP(ssidStr) != true)
    {
        Serial.println("WiFi: failed to create softAP");
        return;
    }

    apSSID = String(ssidStr);
    Serial.println("WiFi: softAP has been established");
    Serial.printf("WiFi: please connect to the %s\r\n", apSSID);
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

    appLoadDateBase();
    wifiSetup();

    server.serveStatic("/", FILESYSTEM, "/www/index.html");
    onServeStatic("/www");

    server.on("/api/v1/status", HTTP_GET, getStatus);
    server.on("/api/v1/config", HTTP_GET, getConfig);
    server.on("/api/v1/config", HTTP_POST, postConfig);

    server.on(
        "/api/v1/update", HTTP_POST, []()
        {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart(); },
        []()
        {
            HTTPUpload &upload = server.upload();
            if (upload.status == UPLOAD_FILE_START)
            {
                Serial.setDebugOutput(true);
                Serial.printf("Update: %s\n", upload.filename.c_str());
                if (!Update.begin())
                { // start with max available size
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_WRITE)
            {
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                {
                    Update.printError(Serial);
                }
            }
            else if (upload.status == UPLOAD_FILE_END)
            {
                if (Update.end(true))
                { // true to set the size to the current progress
                    Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                }
                else
                {
                    Update.printError(Serial);
                }
                Serial.setDebugOutput(false);
            }
            else
            {
                Serial.printf("Update Failed Unexpectedly (likely broken connection): status=%d\n", upload.status);
            }
        });

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
