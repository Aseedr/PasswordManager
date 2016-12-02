#pragma once
#include "libraries.h"

class Info {
private:
	char *site_name;
	char *site_password;
	char *site_description;

public:
	Info();
	~Info();

	void convert();
	void sendMessage(SOCKET);
};