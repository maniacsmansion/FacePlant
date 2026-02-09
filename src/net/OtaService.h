#pragma once
#include <WebServer.h>
#include "../ui/Display.h"

class OtaService {
public:
  void begin(WebServer& server, Display* display = nullptr);

private:
  Display* _display = nullptr;
};
