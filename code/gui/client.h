#ifndef CLIENT
#define CLIENT
#pragma once
#include "libraries.h"

class Client {
private:
    char clientName[CHAR_NAME_SIZE];
    char clientPassword[CHAR_NAME_SIZE];
    char logOrNew;
    SOCKET my_sock;

public:
    Client(char);
    ~Client();

    bool initSokcets();
    bool initOpenSokcet();
    void connectToServer(char *);
    void startClient();
    void help();
    void getMessageFromServer(char *);
    char * getInfo();
};
#endif // CLIENT

