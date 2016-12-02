#pragma once
#include "libraries.h"

class Data {
private:
	char userName[CHAR_NAME_SIZE];
	char pathToPassword[CHAR_NAME_SIZE];
	char pathToKey[CHAR_NAME_SIZE];
	char pathToClient[CHAR_NAME_SIZE];

	char pathToInbox[CHAR_NAME_SIZE];
	char pathToOutbox[CHAR_NAME_SIZE];

public:
	Data(char *, char *);
	~Data();

	void moveToClient(char *);
	void deleteSiteInfo(SOCKET, char *);

	void moveToInbox(char *, char *);
	void moveToOutbox(char *, char *);
	void inboxSend(SOCKET);
	void outboxSend(SOCKET);
};