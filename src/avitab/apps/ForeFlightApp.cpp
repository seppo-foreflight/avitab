/*
 *   AviTab - Aviator's Virtual Tablet
 *   Copyright (C) 2018 Folke Will <folko@solhost.org>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "ForeFlightApp.h"

#include <iostream>

namespace avitab {

ForeFlightApp::ForeFlightApp(FuncsPtr appFuncs):
    App(appFuncs),
    window(std::make_shared<Window>(getUIContainer(), "")),
    updateTimer(std::bind(&ForeFlightApp::onTimer, this), 20)
{
    printf("Welcome to AviTab ForeFlight!\n");

    window->setOnClose([this] () { exit(); });
    mapImage = std::make_shared<img::Image>(642, 438, img::COLOR_TRANSPARENT);
    static int last_x, last_y;
    static bool last_start, last_end; 
    static std::chrono::time_point last_time = std::chrono::system_clock::now();

    mapWidget = std::make_shared<PixMap>(window);
    mapWidget->setClickable(true);
    mapWidget->setClickHandler([&] (int x, int y, bool start, bool end) {

        if (!start && !end) {
            int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - last_time).count();
            if (elapsed_ms < 200) {
                return;
            }
        }
        last_time = std::chrono::system_clock::now();

        if (x != last_x || y != last_y || start != last_start || end != last_end) {
            printf("%d %d %d %d\n", x, y, start, end);
            if(start){
                SendPointerEvent(client, x, y, 1);
            } else if(end){
                SendPointerEvent(client, x, y, 3);
            } else {
                SendPointerEvent(client, x, y, 2);
            }
            last_x = x;
            last_y = y;
            last_start = start;
            last_end = end;
        }

    });

    // mapWidget->setClickable(true);
    // mapWidget->setClickHandler([this] (int x, int y, bool pr, bool rel) { onMapPan(x, y, pr, rel); });
}

void ForeFlightApp::resume() {
    printf("resume\n");

    suspended = false;

    if (client) {
        return;
    }
    
    const char *serverIP = "10.81.1.187";//"192.168.1.63";//"172.16.97.70";//"192.168.0.29";
    int port = 5900;

    client = rfbGetClient(8,3,4);
    client->GotFrameBufferUpdate = &frameBufferUpdateWrapper;
    client->serverHost = (char*)serverIP;
    client->serverPort = port;
    client->clientData = new rfbClientData();
    client->clientData->data = this;

    char *name = (char*)"avitab";
    char encodings[] = {"-encodings"};
    char encodingLevel[] = {"ultra"};
    char compress[] = {"-compress"};
    char compressLevel = 0;

    char* initClient[5];
    int numberOfArguments = 5;
    initClient[0] = name;
    initClient[1] = encodings;
    initClient[2] = encodingLevel;
    initClient[3] = compress;
    initClient[4] = &compressLevel;

    if (!rfbInitClient(client, &numberOfArguments,initClient))
    // if (!rfbInitClient(client, NULL, NULL))
    {
        std::cerr << "Failed to initialize VNC client." << std::endl;
        return;
    }

    onTimer();
}

void ForeFlightApp::suspend() {
    printf("suspend\n");   
    suspended = true;
}

void ForeFlightApp::frameBufferUpdateWrapper(rfbClient* client, int x, int y, int w, int h) {
    auto foreFlightApp = static_cast<ForeFlightApp*>(client->clientData->data);
    foreFlightApp->frameBufferUpdate(x, y, w, h);
}

void ForeFlightApp::frameBufferUpdate(int x, int y, int w, int h) {

    auto dest_w = mapImage->getWidth();
    auto dest_h = mapImage->getHeight();

    // printf("Update %d %d %d %d, dest: %d %d\n", x, y, w, h, dest_w, dest_h);

    uint32_t *srcPtr = (uint32_t*)client->frameBuffer;
    uint32_t *destPtr = mapImage->getPixels();
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            if (i < dest_w && j < dest_h) {
                int c = srcPtr[j * w + i] | 0xFF000000;
                c = (c & 0xFF000000) | ((c & 0xFF) << 16) | (c & 0xFF00) | ((c & 0xFF0000) >> 16);
                destPtr[j * dest_w + i] = c;
            }
        }
    }
    this->mapWidget->draw(*this->mapImage);
}

bool ForeFlightApp::onTimer() {
    if (suspended) {
        return true;
    }    
    int n = WaitForMessage(client,50);
	if (n > 0) {
        // printf("Handle\n");
        HandleRFBServerMessage(client);
    }
    return true;
}

} /* namespace avitab */
