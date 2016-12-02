#pragma once
#include "info.h"

Info::Info() {
	site_name = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
	site_password = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
	site_description = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));

	cout << "Input site name: ";
	cin >> site_name;
	while (cin.get() != '\n');

	cout << "Input site password: ";
	cin >> site_password;
	while (cin.get() != '\n');

	cout << "Input site description: ";
	int count = strlen(site_description);
	char buff;
	while (1) {
		buff = cin.get();
		if (buff == '\n') {
			break;
		}
		site_description[count] = buff;
		count++;
	}
}

Info::~Info() {
	free(site_name);
	free(site_password);
	free(site_description);
}

void Info::convert() {
	FILE *sendMessage;
	sendMessage = fopen(SEND_MESSAGE_FILE, "w");

	fprintf(sendMessage, "%s\n", site_name);
	fprintf(sendMessage, "%s\n", site_password);
	fprintf(sendMessage, "%s\n", site_description);

	fclose(sendMessage);
}

void Info::sendMessage(SOCKET socket) {
	FILE *sendFile;
	sendFile = fopen(SEND_MESSAGE_FILE, "r+b");

	char buffer[ONE_BYTE];
	while (true) {
		if (fread(&buffer, ONE_BYTE, ONE_BYTE, sendFile) == 0) break;
		send(socket, buffer, ONE_BYTE, NULL);
	}

	fclose(sendFile);
	remove(SEND_MESSAGE_FILE);
}