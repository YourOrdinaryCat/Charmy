#include "pch.h"

import server;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
	winrt::init_apartment();

	//TODO: Register class objects
	CoResumeClassObjects();
	server::wait();

	//TODO: Unregister class objects
}
