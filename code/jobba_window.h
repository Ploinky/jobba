#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "settings.h"

class JobbaWindow {
public:
	JobbaWindow(JobbaResolution initial_resolution);

	void Show(int cmd_show);
	void SetResolution(JobbaResolution new_resolution);
	void SetVideoMode(JobbaVideoMode new_video_mode);

	HWND get_window_handle() const;
	JobbaResolution get_current_resolution() const;
	JobbaVideoMode get_current_video_mode() const;
private:
	HWND window_handle_;
	JobbaResolution window_resolution_;
	JobbaVideoMode video_mode_;
};