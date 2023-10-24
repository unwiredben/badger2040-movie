/* Copyright 2023, Ben Combee.  All rights reserved.

   This project is licensed under the BSD 3-clause license.  See LICENSE for
   details.
 */

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/platform.h"

/* define wrappers for malloc/realloc to determine memory use */
void* my_malloc(const char* what, std::size_t size);
void* my_realloc(const char* what, void* ptr, std::size_t new_size);

#define PLM_BUFFER_DEFAULT_SIZE (16 * 1024)
#define PLM_MALLOC(what, sz) my_malloc(what, sz)
#define PLM_FREE(p) free(p)
#define PLM_REALLOC(what, p, sz) my_realloc(what, p, sz)
#define PL_MPEG_IMPLEMENTATION
#include "pl_mpeg.h"

#include "MPEG1Video.h"
#define VIDEO_DATA BadgeDemoSupercon2022_mpg
#define VIDEO_LEN  BadgeDemoSupercon2022_mpg_len

#include "common/pimoroni_common.hpp"
#include "libraries/badger2040/badger2040.hpp"

pimoroni::Badger2040 badger;

constexpr int WIDTH = 296;
constexpr int HEIGHT = 128;

constexpr int MOVIE_UPDATE_SPEED = 1;

void clear_screen() {
    badger.update_speed(0);

    badger.pen(15);
    badger.clear();
    badger.update(true);
    badger.update(true);
    badger.update(true);
    badger.update(true);

    badger.pen(0);
    badger.clear();
    badger.update(true);

    badger.update_speed(MOVIE_UPDATE_SPEED);
}

void show_frame(plm_frame_t *frame) {
    auto width = frame->width;
    auto height = frame->height;
    auto pixels = frame->y.data;
    int frameNumber = frame->time * 30;
    int x_offset = (WIDTH - width) / 2;

    // do a clear before frame 0 to clean up screen
    if (frameNumber == 0) {
        clear_screen();
    }

    /* do 4x4 Bayer dithering on 8-bit luma pixel data */
    constexpr uint8_t bayerThresholdMap[4][4] = {
        {  15, 135,  45, 165  },
        { 195,  75, 225, 105  },
        {  60, 180,  30, 150  },
        { 240, 120, 210,  90 }
    };

    for (int y = 0; y < frame->height; ++y) {
        for (int x = 0; x < frame->width; ++x) {
            int currentPixelOffset = x + y * width;
            uint8_t luma = pixels[currentPixelOffset];
            uint8_t threshhold = bayerThresholdMap[x % 4][y % 4];
            badger.pen(luma < threshhold ? 0 :15);
            badger.pixel(x + x_offset, y);
        }
    }
    // badger.partial_update(x_offset, 0, width, HEIGHT, true);
    badger.update(true);
}

void play_video() {
    plm_t *plm =
        plm_create_with_memory(
                const_cast<uint8_t*>(VIDEO_DATA),
                VIDEO_LEN,
                false);

    plm_set_audio_enabled(plm, false);
    plm_set_loop(plm, true);

    // Decode forever until power is removed
    while (true) {
        auto *frame = plm_decode_video(plm);
        show_frame(frame);
    }
}

void* my_malloc(const char*, std::size_t size) {
    // printf("malloc %s: %d\n", what, (int)size);
    return malloc(size);
}

void* my_realloc(const char*, void* ptr, std::size_t new_size) {
    // printf("realloc %s: %d\n", what, (int)new_size);
    return realloc(ptr, new_size);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    // puts("starting badger2040-movie");
    badger.init();
    play_video();
}
