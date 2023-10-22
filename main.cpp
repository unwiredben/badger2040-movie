/* Copyright 2023, Ben Combee.  All rights reserved.

   This project is licensed under the BSD 3-clause license.  See LICENSE for
   details.. */

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/platform.h"

#define PL_MPEG_IMPLEMENTATION
#include "plmpeg.h"

#include "MPEG1Video.h"

#include "common/pimoroni_common.hpp"
#include "libraries/badger2040/badger2040.hpp"

pimoroni::Badger2040 badger;

constexpr int WIDTH = 296;
constexpr int HEIGHT = 128;

void init() {

}

void loop() {

}

int main() {
  init();
  while (true) {
    loop();
  }
}
