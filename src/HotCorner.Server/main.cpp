#include "pch.h"

import instance_info;
import server;

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
	instance_info::set(instance);
	winrt::init_apartment();

	//TODO: Register class objects
	CoResumeClassObjects();
	server::wait();

	//TODO: Unregister class objects
}
