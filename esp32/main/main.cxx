// #include <MockHandler.hxx>
// #include <MockHandlerDisplay.hxx>
// #include <CanSniffer.hxx>
// #include <Comparator.hxx>
// #include <CanHandler.hxx>
// #include <CanHandlerDisplay.hxx>
// #include <SerialHandlerBLE.hxx>
// #include <SerialHandlerDisplay.hxx>
#include <Obd2CanHandlerDisplay.hxx>

extern "C" void app_main() {
  setup();
  while (true) {
    loop();
  }
}
