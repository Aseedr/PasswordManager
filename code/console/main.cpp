#include "libraries.h"
#include "server.h"
#include "client.h"

int main() {
	char serverOrClient = 0;
	cout << "'s' - server" << endl;
	cout << "'c' - client" << endl << endl;
	cout << "input: ";
	cin >> serverOrClient;

	if (serverOrClient == 's') {
		system("cls");
		// server functional
		Server *server = new Server();
		if (server->initSokcets() == false) {
			return 1;
		}

		server->initConnectSokcets(server->initConnections());
		server->startServer();

		delete server;
	} if (serverOrClient == 'c') {
		system("cls");
		// client functional
		char logOrNew = 0;
		cout << "'l' - login" << endl;
		cout << "'n' - new" << endl << endl;
		cout << "input: ";
		cin >> logOrNew;

		system("cls");
		if (logOrNew == 'l' || logOrNew == 'n') {
			Client *client = new Client(logOrNew);
			if (client->initSokcets() == false) {
				return 1;
			}

			if (client->initOpenSokcet() == false) {
				return 1;
			}

			char ip[IP_BUFF_SIZE];
			cout << "Input ip address: ";
			cin >> ip;
			system("cls");
			client->connectToServer(ip);
			client->startClient();

			delete client;
		}
		else {
			cout << "Incorrect input!" << endl << endl;
			system("pause");
			return 1;
		}

	}
	else {
		cout << "Incorrect input!" << endl << endl;
		system("pause");
		return 1;
	}

	return 0;
}