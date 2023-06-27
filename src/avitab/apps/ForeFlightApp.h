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
#ifndef SRC_AVITAB_APPS_FOREFLIGHTAPP_H_
#define SRC_AVITAB_APPS_FOREFLIGHTAPP_H_

#include "App.h"

#include "src/gui_toolkit/widgets/PixMap.h"
#include "src/gui_toolkit/widgets/Window.h"
#include "src/gui_toolkit/Timer.h"
#include "src/libimg/Image.h"
#include <rfb/rfbclient.h>
#include <rfb/rfb.h>

namespace avitab {

class ForeFlightApp: public App {
public:
    ForeFlightApp(FuncsPtr appFuncs);
    void resume() override;
    void suspend() override;
private:
    std::shared_ptr<Window> window;
    std::shared_ptr<PixMap> mapWidget;
    std::shared_ptr<img::Image> mapImage;
    Timer updateTimer;
    bool onTimer();

    static void frameBufferUpdateWrapper(rfbClient* client, int x, int y, int w, int h);
    void frameBufferUpdate(int x, int y, int w, int h);

    rfbClient* client;
    bool suspended = true;
};

} /* namespace avitab */

#endif /* SRC_AVITAB_APPS_FOREFLIGHTAPP_H_ */
