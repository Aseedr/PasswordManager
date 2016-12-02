#include "server.h"

Server::Server() {
	clientCount = 0;
}

Server::~Server() {
	if (Connections) {
		delete Connections;
	}
}

bool Server::initSokcets() {
	WSAData data;
	WORD version = MAKEWORD(2, 2);          // sokcet version
	int res = WSAStartup(version, &data);   // init socket
	if (res != 0) {
		cout << "Sockets error!";
		return false;
	}
	return true;
}

addrinfo *Server::initConnections() {
	struct addrinfo hints;
	struct addrinfo *result;

	// init memory for sockects
	Connections = (SOCKET*)calloc(SOCKET_MEM, sizeof(SOCKET));

	// clear the struct
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;        // socket family
	hints.ai_flags = AI_PASSIVE;      // socket flags
	hints.ai_socktype = SOCK_STREAM;  // socket type "tcp"
	hints.ai_protocol = IPPROTO_TCP;  // socket protocol

	getaddrinfo(NULL, PORT, &hints, &result); // get info about socket and server

	return result;
}

void Server::initConnectSokcets(addrinfo *result) {
	// init connect socket
	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);  // announce server
	listen(Listen, SOMAXCONN);    // wait time to connect
	freeaddrinfo(result);         // clear the result struct info
}

void Server::startServer() {
	cout << "Server started..." << endl;
	while (1) { // start server
		if (Connect = accept(Listen, NULL, NULL)) { // if connect is OK
			cout << "Client connected..." << endl;
			Connections[clientCount] = Connect;     // save client socket
													// send messege to client
			clientCount++;
			// create client thread
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)distributionMessages,
				this, NULL, NULL);
		}
		Sleep(SLEEP_TIME);
	}
}

void Server::distributionMessages(void *arr) {
	Data *userData = NULL;
	char *buffer;
	bool flag = false;
	int newMessage = 0;
	int currentUser = ((Server*)arr)->clientCount - 1;
	while (1) { // client started
		buffer = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));

		// if client have a message
		if (recv(((Server*)arr)->Connections[currentUser], buffer, CHAR_BUFFER_SIZE, NULL)) {
			if (flag == false) {
				char login[CHAR_NAME_SIZE];
				char password[CHAR_NAME_SIZE];
				bool _flag = false;
				int ii = 0;
				bool log = 0;
				for (int z = 0; z < strlen(buffer); z++) {
					if (buffer[z] == '\n') {
						break;
					}
					if (buffer[z] == '&') {
						_flag = true;
						continue;
					}
					if (buffer[z] == '$') {
						log = 1;
						_flag = true;
						continue;
					}
					if (_flag == false) {
						login[z] = buffer[z];
					}
					else {
						password[ii] = buffer[z];
						ii++;
					}
				}

				// check password
				if (log == 1) {
					FILE *_pass;
					char _path[CHAR_NAME_SIZE];
					strcpy(_path, login);
					strcat(_path, "/password");
					_pass = fopen(_path, "r");
					if (!_pass) {
						send(((Server*)arr)->Connections[currentUser], "OK!", strlen("OK!"), NULL);
					}
					else {
						send(((Server*)arr)->Connections[currentUser], "EXIST!", strlen("EXIST!"), NULL);
						fclose(_pass);
						return;
					}
				}
				else {
					FILE *_pass;
					char _path[CHAR_NAME_SIZE];
					strcpy(_path, login);
					strcat(_path, "/password");
					_pass = fopen(_path, "r+b");
					if (!_pass) {
						send(((Server*)arr)->Connections[currentUser], "ERROR!", strlen("ERROR!"), NULL);
						return;
					}
					else {
						char buff_to_pass[CHAR_BUFFER_SIZE];
						fscanf(_pass, "%s", buff_to_pass);
						if (strcmp(buff_to_pass, password) == 0) {
							send(((Server*)arr)->Connections[currentUser], "OK!", strlen("OK!"), NULL);
						}
						else {
							send(((Server*)arr)->Connections[currentUser], "ERROR!", strlen("ERROR!"), NULL);
							fclose(_pass);
							return;
						}
						fclose(_pass);
					}
				}

				strcpy_s(((Server*)arr)->clientsLogins[currentUser], login);
				flag = true;

				// create user data
				userData = new Data(((Server*)arr)->clientsLogins[currentUser], password);
			}
			else {
				if ((int)buffer[0] == 0) {
					cout << "Client " << ((Server*)arr)->clientsLogins[currentUser]
						<< " disconnected!" << endl;
					break;
				}

				if (strstr(buffer, COMMAND_DEL)) {
					if (buffer[3] == '&') {
						char command_buff[CHAR_NAME_SIZE];
						for (int t = 4; t < strlen(buffer); t++) {
							command_buff[t - 4] = buffer[t];
						}

						((Server*)arr)->sendDelcommand(command_buff,
							((Server*)arr)->Connections[currentUser], userData);
					}
					continue;
				}

				if (strcmp(buffer, COMMAND_SEE_INBOX) == 0 || strcmp(buffer, COMMAND_SEE_OUTBOX) == 0) {
					if (strcmp(buffer, COMMAND_SEE_INBOX) == 0) {
						((Server*)arr)->sendMessageToCient(COMMAND_SEE_INBOX,
							((Server*)arr)->Connections[currentUser], userData);
					}

					if (strcmp(buffer, COMMAND_SEE_OUTBOX) == 0) {
						((Server*)arr)->sendMessageToCient(COMMAND_SEE_OUTBOX,
							((Server*)arr)->Connections[currentUser], userData);
					}
				}
				else {
					FILE *temp;
					temp = fopen(TEMP_MESSAGE_FILE, "a+b");
					fprintf(temp, "%s", buffer);
					fclose(temp);
					newMessage++;
				}
			}
		}

		if (newMessage == NEW_MESSAGE) {
			newMessage = 0;
			FILE *receivedMessage;
			FILE *_temp;
			receivedMessage = fopen(TEMP_MESSAGE_FILE, "r+b");
			char site_name[CHAR_NAME_SIZE];
			char _buff;
			fscanf(receivedMessage, "%s", site_name);

			_temp = fopen(site_name, "w+b");
			fread(&_buff, 1, 1, receivedMessage);
			while (true) {
				if (fread(&_buff, 1, 1, receivedMessage) == 0) break;
				fwrite(&_buff, 1, 1, _temp);
			}

			fclose(receivedMessage);
			fclose(_temp);

			userData->moveToClient(site_name);
			remove(site_name);
			remove(TEMP_MESSAGE_FILE);
		}

		free(buffer);
		Sleep(SLEEP_TIME);
	}
}

void Server::sendMessageToCient(char *command, SOCKET socket, Data *userData) {
	if (strcmp(command, COMMAND_SEE_INBOX) == 0) {
		userData->inboxSend(socket);
	}
	if (strcmp(command, COMMAND_SEE_OUTBOX) == 0) {
		userData->outboxSend(socket);
	}
}

void Server::sendDelcommand(char *command, SOCKET socket, Data *userData) {
	userData->deleteSiteInfo(socket, command);
}