#pragma once

enum JobbaResolution {
    kQnhd,
    kNhd,
    kQhd,
    kHd,
    kHdplus,
    kFullhd,
};

JobbaResolution NextResolution(JobbaResolution res);
void GetJobbaResolutionValue(JobbaResolution res, unsigned int* width, unsigned int* height);

class Settings{
public:
	inline static JobbaResolution current_resolution;
    inline static JobbaResolution current_virtual_resolution;
};