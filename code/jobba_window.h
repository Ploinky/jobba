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

	HWND get_window_handle();
	JobbaResolution get_current_resolution();
private:
	HWND window_handle_;
	JobbaResolution window_resolution_;
};