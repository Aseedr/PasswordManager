#include "data.h"

Data::Data(char *userName, char *password) {
	CreateDirectoryA(userName, NULL);

	FILE *pass;
	FILE *key;
	char path_pass[CHAR_NAME_SIZE];
	char path_key[CHAR_NAME_SIZE];
	char path_client[CHAR_NAME_SIZE];
	strcpy_s(path_pass, userName);
	strcpy_s(path_key, userName);
	strcpy_s(path_client, userName);
	strcat(path_pass, "/password");
	strcat(path_key, "/key");
	strcat(path_client, "/");

	strcpy(pathToPassword, path_pass);
	strcpy(pathToKey, path_key);
	strcpy(pathToClient, path_client);

	pass = fopen(path_pass, "w+b");
	key = fopen(path_key, "w+b");
	fprintf(pass, "%s", password);

	int _key = int(userName[0]) - int(password[0]);
	if (_key < 0) {
		_key = int(password[0]) - int(userName[0]);
	}
	if (_key == 0) {
		_key = SYS_KEY;
	}
	fprintf(key, "%d", _key);

	fclose(pass);
	fclose(key);
}

Data::~Data() {}

void Data::moveToClient(char *site_name) {
	FILE *temp;
	FILE *toClient;
	char path[CHAR_NAME_SIZE];
	char buffer;

	strcpy(path, pathToClient);
	strcat(path, site_name);
	strcat(path, ".txt");

	temp = fopen(site_name, "r+b");
	toClient = fopen(path, "w+b");

	while (true) {
		if (fread(&buffer, 1, 1, temp) == 0) break;
		fwrite(&buffer, 1, 1, toClient);
	}

	fclose(temp);
	fclose(toClient);
}

void Data::moveToInbox(char *to_user, char *date) {
	FILE *temp;
	FILE *toInbox;
	char path[CHAR_NAME_SIZE];
	char buffer;

	strcpy(path, to_user);
	strcat(path, "/inbox/");
	strcat(path, date);

	temp = fopen(TEMP_MESSAGE_FILE, "r+b");
	toInbox = fopen(path, "w+b");

	while (true) {
		if (fread(&buffer, 1, 1, temp) == 0) break;
		fwrite(&buffer, 1, 1, toInbox);
	}

	fclose(temp);
	fclose(toInbox);
}

void Data::moveToOutbox(char *from_user, char *date) {
	FILE *temp;
	FILE *toOutbox;
	char path[CHAR_NAME_SIZE];
	char buffer;

	strcpy(path, from_user);
	strcat(path, "/outbox/");
	strcat(path, date);

	temp = fopen(TEMP_MESSAGE_FILE, "r+b");
	toOutbox = fopen(path, "w+b");

	while (true) {
		if (fread(&buffer, 1, 1, temp) == 0) break;
		fwrite(&buffer, 1, 1, toOutbox);
	}

	fclose(temp);
	fclose(toOutbox);
}

void Data::inboxSend(SOCKET socket) {
	string folder(((const char*)pathToClient));
	folder.append("\\");
	string fileMask = folder + "*";
	char FileRead[MAX_PATH];
	bool flag = false;

	char *result;
	WIN32_FIND_DATA FindFileData;
	HANDLE find_Handle;
	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);
	send(socket, "\nSites:\n", strlen("\nSites:\n"), NULL);
	if (find_Handle != INVALID_HANDLE_VALUE) {
		do {
			result = strchr(FindFileData.cFileName, '.txt');
			if (result > 0) {
				flag = true;
				char *site_name_buff = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
				char *bufferForPassword = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));

				FILE *inboxMail;
				strcpy(FileRead, folder.c_str());
				strcat(FileRead, FindFileData.cFileName);
				inboxMail = fopen(FileRead, "r");

				send(socket, "  Site name: ", strlen("  Site name: "), NULL);

				fscanf(inboxMail, "%s\n", bufferForPassword);

				bool _flag = false;
				int _count = 0;
				for (int z = 0; z < strlen(FileRead); z++) {
					if (FileRead[z] == '.' && FileRead[z + 1] == 't' &&
						FileRead[z + 2] == 'x' && FileRead[z + 3] == 't') {
						break;
					}
					if (_flag == true) {
						site_name_buff[_count] = FileRead[z];
						_count++;
					}
					if (FileRead[z] == '\\') {
						_flag = true;
					}
				}

				send(socket, site_name_buff, strlen(site_name_buff), NULL);
				send(socket, "\n  Site password: ", strlen("\n  Site password: "), NULL);
				send(socket, bufferForPassword, strlen(bufferForPassword), NULL);
				send(socket, "\n  Site description: ", strlen("\n  Site description: "), NULL);

				while (!feof(inboxMail)) {
					char *bufferForMessage = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));
					fscanf(inboxMail, "%s\n", bufferForMessage);
					bufferForMessage[strlen(bufferForMessage)] = ' ';
					send(socket, bufferForMessage, strlen(bufferForMessage), NULL);
					free(bufferForMessage);
				}
				send(socket, "\n", ONE_BYTE, NULL);

				fclose(inboxMail);

				free(site_name_buff);
				free(bufferForPassword);
			}
		} while (FindNextFile(find_Handle, &FindFileData) != 0);
		FindClose(find_Handle);
	}

	if (flag == false) {
		send(socket, "  No sites!\n", strlen("  No sites!\n"), NULL);
	}
}

void Data::outboxSend(SOCKET socket) {
	string folder(((const char*)pathToOutbox));
	folder.append("\\");
	string fileMask = folder + "*";
	char FileRead[MAX_PATH];
	bool flag = false;

	char *result;
	WIN32_FIND_DATA FindFileData;
	HANDLE find_Handle;
	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);
	send(socket, "\nMail outbox:\n", strlen("\nMail outbox:\n"), NULL);
	if (find_Handle != INVALID_HANDLE_VALUE) {
		do {
			result = strchr(FindFileData.cFileName, '.txt');
			if (result > 0) {
				flag = true;
				char *bufferForFromUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
				char *bufferForToUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));

				FILE *inboxMail;
				strcpy(FileRead, folder.c_str());
				strcat(FileRead, FindFileData.cFileName);
				inboxMail = fopen(FileRead, "r");

				send(socket, "	To user:  ", strlen("To user:  "), NULL);

				fscanf(inboxMail, "%s\n", bufferForFromUser);
				fscanf(inboxMail, "%s\n", bufferForToUser);
				send(socket, bufferForToUser, strlen(bufferForToUser), NULL);
				send(socket, "\n			", strlen("\n			"), NULL);

				while (!feof(inboxMail)) {
					char *bufferForMessage = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));
					fscanf(inboxMail, "%s\n", bufferForMessage);
					bufferForMessage[strlen(bufferForMessage)] = ' ';
					send(socket, bufferForMessage, strlen(bufferForMessage), NULL);
					free(bufferForMessage);
				}
				send(socket, "\n", ONE_BYTE, NULL);

				fclose(inboxMail);

				free(bufferForFromUser);
				free(bufferForToUser);
			}
		} while (FindNextFile(find_Handle, &FindFileData) != 0);
		FindClose(find_Handle);
	}

	if (flag == false) {
		send(socket, "		No mail!\n", strlen("		No mail!\n"), NULL);
	}
}

void Data::deleteSiteInfo(SOCKET socket, char *del_site_name) {
	string folder(((const char*)pathToClient));
	folder.append("\\");
	string fileMask = folder + "*";
	char FileRead[MAX_PATH];
	bool flag = false;

	char *result;
	WIN32_FIND_DATA FindFileData;
	HANDLE find_Handle;
	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);
	if (find_Handle != INVALID_HANDLE_VALUE) {
		do {
			result = strchr(FindFileData.cFileName, '.txt');
			if (result > 0) {
				flag = true;
				char *site_name_buff = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));

				strcpy(FileRead, folder.c_str());
				strcat(FileRead, FindFileData.cFileName);

				bool _flag = false;
				int _count = 0;
				for (int z = 0; z < strlen(FileRead); z++) {
					if (FileRead[z] == '.' && FileRead[z + 1] == 't' &&
						FileRead[z + 2] == 'x' && FileRead[z + 3] == 't') {
						break;
					}
					if (_flag == true) {
						site_name_buff[_count] = FileRead[z];
						_count++;
					}
					if (FileRead[z] == '\\') {
						_flag = true;
					}
				}

				if (strcmp(site_name_buff, del_site_name) == 0) {
					send(socket, "\n  Ok!\n", strlen("\n  Ok!\n"), NULL);
					char del_file[CHAR_NAME_SIZE];
					strcpy(del_file, pathToClient);
					strcat(del_file, site_name_buff);
					strcat(del_file, ".txt");
					remove(del_file);
				}

				free(site_name_buff);
			}
		} while (FindNextFile(find_Handle, &FindFileData) != 0);
		FindClose(find_Handle);
	}

	if (flag == false) {
		send(socket, "  No sites!\n", strlen("  No sites!\n"), NULL);
	}
	send(socket, COMMAND_END_OF_SEND, strlen(COMMAND_END_OF_SEND), NULL);
}