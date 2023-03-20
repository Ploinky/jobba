#pragma once

#include "settings.h"

JobbaResolution NextResolution(JobbaResolution res) {
    return static_cast<JobbaResolution>((res + 1) % (kFullhd + 1));
}

void GetJobbaResolutionValue(JobbaResolution res, unsigned int* width, unsigned int* height) {
    switch (res) {
    case kQnhd: {
        *width = 320;
        *height = 180;
        break;
    }
    case kNhd: {
        *width = 640;
        *height = 360;
        break;
    }
    case kQhd: {
        *width = 960;
        *height = 540;
        break;
    }
    case kHd: {
        *width = 1280;
        *height = 720;
        break;
    }
    case kHdplus: {
        *width = 1600;
        *height = 900;
        break;
    }
    case kFullhd: {
        *width = 1920;
        *height = 1080;
        break;
    }
    default: {
        *width = 1280;
        *height = 720;
        break;
    }
    }
}