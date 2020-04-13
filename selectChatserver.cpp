#include <stdio.h>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

SOCKET clients[64];
int numClients;

CRITICAL_SECTION cs;

DWORD WINAPI ClientThread(LPVOID);


int main()
{

	const char* errorMsg = "Sai cu phap. Hay gui lai.\n";
	const char* okMsg = "Xin chao. Hay nhap id theo cu phap \"client_id: [your_id]\".\n";

	InitializeCriticalSection(&cs);

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET clients[64];
	SOCKET acceptedClient[64];
	int numClients = 0;
	int numAccClients = 0;

	fd_set fdread;

	int ret;


	while (1)
	{
		FD_ZERO(&fdread);

		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		ret = select(0, &fdread, 0, 0, 0);
		if (ret < 0)
			break;

		if (FD_ISSET(listener, &fdread))
		{
			SOCKET client = accept(listener, NULL, NULL);
			printf("New client: %d\n", client);
			send(client, okMsg, strlen(okMsg), 0);
			clients[numClients] = client;
			numClients++;
		}

		for (int i = 0; i < numClients; i++)
			if (FD_ISSET(clients[i], &fdread))
			{
				char cmd[32], id[32], tmp[32];
				char buf[256];
				int flag = 0;
				int index;
				ret = recv(clients[i], buf, sizeof(buf), 0);
				if (ret <= 0)
				{
					// xoa phan tu khoi mang
					closesocket(clients[i]);
					continue;
				}

				for (int i = 0; i < numAccClients; i++) {
					if ( (int)clients[i] == (int)acceptedClient[i]) {
						flag = 1;
						index = i;
						// sau khi co index thi lay ra id theo index do o idArray
					}
				}


					// xu ly du lieu
				if (flag == 0) {
					buf[ret] = 0;
					printf("Received: %s\n", buf);
					ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
					if (ret == 2)
					{
						if (strcmp(cmd, "client_id:") == 0)
						{
							send(clients[i], okMsg, strlen(okMsg), 0);

							EnterCriticalSection(&cs);
							acceptedClient[numAccClients] = clients[i];
							numAccClients++;

							// Moi khi nhan duoc id se luu id vao idArray la 1 array cac string co index cung` index voi numAccClients;
							
							LeaveCriticalSection(&cs);
							continue;
						}
						else
						{
							send(clients[i], errorMsg, strlen(errorMsg), 0);
						}
					}
					else
					{
						send(clients[i], errorMsg, strlen(errorMsg), 0);
					}
				}

				// Forward messages
				else {
					buf[ret] = 0;
					printf("Received: %s\n", buf);
					
					// lay ra id theo index o idArray

					char sendBuf[256];
					strcpy(sendBuf, id);
					strcat(sendBuf, ": ");
					strcat(sendBuf, buf);

					for (int i = 0; i < numAccClients; i++)
						if (acceptedClient[i] != clients[i])
							send(clients[i], sendBuf, strlen(sendBuf), 0);
				}
			}
	}

	DeleteCriticalSection(&cs);
  WSACleanup();
}
//Em thưa thầy hiện em đã làm xong nhưng chạy chương trình bị lỗi ạ.Thầy kiểm tra giúp em với ạ
