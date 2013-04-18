#pragma once

#include "..\Leap\Leap\Leap.h"

using namespace Leap;

class LeapListener : public Listener{

    public:
        virtual void onInit(const Controller&);
        virtual void onConnect(const Controller&);
        virtual void onDisconnect(const Controller&);
        virtual void onExit(const Controller&);
        virtual void onFrame(const Controller&);

};


