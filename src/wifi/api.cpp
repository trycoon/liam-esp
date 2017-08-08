#include "api.h"

/*
void processCommand(char* msgBuffer) {
    StaticJsonBuffer<BUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(msgBuffer);

    if (!root.success()) {
        Serial.println("parseObject() failed");
        return;
    }

    yield();
    const char* cmd = root["cmd"];
    char msg[1024];
    if (strcmp(cmd, "heartBeat")  == 0) {
        root.printTo(msg, sizeof(msg));
        safeSend(msg);
    } else if (strcmp(cmd, "identify")  == 0) {
        StaticJsonBuffer<1024> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();

        //SIZE FOR BASE INFO WITHOUT SERVICES MAY BE ~512 so there 512 jsonBuffer for services array: https://github.com/bblanchon/ArduinoJson/wiki/Memory-model
        json["cmd"] = "identify";
        json["chipId"] = ESP.getChipId();
        json["freeSketchSpace"] = ESP.getFreeSketchSpace();

        JsonObject& version = json.createNestedObject("version");
        version["sdk"] = system_get_sdk_version();
        version["major"] = MAJOR_VERSION;
        version["minor"] = MINOR_VERSION;

        rst_info* resetInfo = system_get_rst_info();
        JsonObject& reset = json.createNestedObject("reset");
        reset["reason"] = resetInfo->reason;
        reset["exccause"] = resetInfo->exccause;
        reset["epc1"] = resetInfo->epc1;
        reset["epc2"] = resetInfo->epc2;
        reset["epc3"] = resetInfo->epc3;
        reset["excvaddr"] = resetInfo->excvaddr;
        reset["depc"] = resetInfo->depc;

        JsonObject& flash = json.createNestedObject("flash");
        flash["id"] = ESP.getFlashChipId();
        flash["size"] = ESP.getFlashChipSize();
        flash["realSize"] = ESP.getFlashChipRealSize();
        flash["speed"] = ESP.getFlashChipSpeed();
        FlashMode_t ideMode = ESP.getFlashChipMode();
        flash["mode"] = (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN");

        const char* phyModes[] = { "", "B", "G", "N" };
        JsonObject& wifi = json.createNestedObject("wifi");
        wifi["channel"] = wifi_get_channel();
        wifi["reconnect"] = wifi_station_get_auto_connect();
        wifi["phyMode"] = phyModes[wifi_get_phy_mode()];
        wifi["ssid"] = WiFi.SSID();
        wifi["bssid"] = WiFi.BSSIDstr();
        wifi["mac"] = WiFi.macAddress();
        wifi["ip"] = IP2Str(WiFi.localIP());
        wifi["mask"] = IP2Str(WiFi.subnetMask());
        wifi["gateway"] = IP2Str(WiFi.gatewayIP());
        wifi["dns"] = IP2Str(WiFi.dnsIP());
        wifi["hostname"] = WiFi.hostname();

        JsonArray& services = json.createNestedArray("services");
        for (unsigned int i = 0; i < 10; i++) {
            JsonObject& service = services.createNestedObject();
            service["keyA"] = "AAA";
            service["keyB"] = "BBB";
            service["keyC"] = 111;
            yield();
        }

        json.printTo(msg, sizeof(msg));
        safeSend(msg);
    } else if(strcmp(cmd, "restart")  == 0) {
        ESP.restart();
    } else if(strcmp(cmd, "perform")  == 0) {

    }
}
*/
