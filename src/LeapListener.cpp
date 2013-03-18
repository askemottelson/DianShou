#include "LeapListener.h"

void LeapListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
  controller.enableGesture( Leap::Gesture::TYPE_SWIPE );
}

void LeapListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
}

void LeapListener::onDisconnect(const Controller& controller) {
  std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {

}