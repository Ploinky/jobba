#pragma once

enum JobbaResolution {
    kQnhd,
    kNhd,
    kQhd,
    kHd,
    kHdplus,
    kFullhd,
};

enum JobbaVideoMode {
    kWindowedFullscreen,
    kWindowed
};

JobbaResolution NextResolution(JobbaResolution res);
void GetJobbaResolutionValue(JobbaResolution res, int* width, int* height);

class Settings{
public:
	inline static JobbaResolution current_resolution;
    inline static JobbaResolution current_virtual_resolution;
    inline static JobbaVideoMode current_video_mode;
};