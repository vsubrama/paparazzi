#include <iostream>
#include <Ivy/ivy.h>
#include <Ivy/ivyglibloop.h>
#include <string>
#include "messages.h"
#include "common.h"

using namespace std;
#ifdef __APPLE__
string ivyBus = "224.255.255.255";
#else
string ivyBus = "127.255.255.255";
#endif
string fgAddress = "127.0.0.1";


void ivy_transport_init(void) {
  IvyInit ("Paparazzi jsbsim " + AC_ID, "READY", NULL, NULL, NULL, NULL);
  IvyStart(ivyBus.c_str());

}

