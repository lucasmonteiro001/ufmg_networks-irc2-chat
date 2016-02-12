/*
 ============================================================================
 Name        : Cliente_Simples.c
 Author      : Douglas Lopes
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "user.h"

void error(const char *msg) {
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[]) {

	char buffer[SIZE_OF_MESSAGE + 1];
	int mysocket, portno;
	struct sockaddr_in dest;
	struct hostent *server;

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 500000;

	if (argc < 3) {
		fprintf(stderr, "usage %s hostname port nick\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);
	mysocket = socket(AF_INET, SOCK_STREAM, 0);

	if (mysocket < 0) {
		error("ERROR opening socket");
	}

	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	memset(&dest, 0, sizeof(dest)); /* zero the struct */
	bzero((char *) &dest, sizeof(dest));
	dest.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &dest.sin_addr.s_addr,
			server->h_length);
	dest.sin_port = htons(portno);

	// Faz a conexao entre cliente e servidor
	connect(mysocket, (struct sockaddr *) &dest, sizeof(struct sockaddr));

	while (1) {

		fd_set rfds;
		FD_ZERO(&rfds);

		// seta que o socket do servidor e a leitura do teclado serao feitas
		FD_SET(mysocket, &rfds);
		FD_SET(STDIN_FILENO, &rfds);

		if (select(FD_SETSIZE, &rfds, NULL, NULL, &tv) < 0) {
			perror("select");
			exit(-1);
		}

		// Se o dado chegou do servidor
		if (FD_ISSET(mysocket, &rfds)) {

			// zera o buffer
			for (int i = 0; i < sizeof(buffer); i++) {
				buffer[i] = '\0';
			}

			// le o dado do servidor
			read(mysocket, buffer, sizeof(buffer));

			// realiza a impressao da msg retirando o NEW e colocando o nome do usuario entre [user]
			int count_space = 0;
			for (int i = 0; i < strlen(buffer); i++) {

				if (buffer[i] == ' ')
					count_space++;

				if (i == 4) {
					printf("[%c", buffer[i]);
				} else if (i > 4 && count_space != 2) {
					printf("%c", buffer[i]);
				}

				if (count_space == 2) {
					printf("] ");
					count_space++;
				}

			}
			printf("\n");

		}

		// Se o usuario digitou alguma coisa
		if (FD_ISSET(STDIN_FILENO, &rfds)) {
			printf("\n");

			// Le a msg do teclado
			fgets(buffer, sizeof(buffer), stdin);

			// Faz o parsing da msg
			char pch[SIZE_OF_COMMAND];
			for (int k = 0; k < strlen(buffer); k++) {
				if (buffer[k] != ' ' && buffer[k] != '\n')
					pch[k] = buffer[k];
				else {
					pch[k] = '\0';
					break;
				}
			}

			// Se o nick possuir espacos, desconsidera o nick como valido e nao faz nada
			int counter = 0;
			for (int k = 0; k < strlen(buffer); k++) {
				if (buffer[k] == ' ')
					counter++;
			}

			if (strcmp(pch, "NICK") == 0) {

				// se o nick contem espacos, vai desconsiderar
				if (counter <= 1) {

					// se o nick estiver correto, salvo-o
					// Entretanto, se o nick possuir mais de 16 caracteres,
					// realiza o truncamento do nick
					if (strlen(buffer) > strlen("NICK ")) {
						int size_of_nick_message = strlen("NICK ")
								+ SIZE_OF_NICK + 1;
						char msg[size_of_nick_message];
						strncpy(msg, buffer, size_of_nick_message);

						msg[size_of_nick_message - 1] = '\0';

						send(mysocket, msg, strlen(msg), 0);
					}
				}

			} else if (strcmp(pch, "POST") == 0) {

				// considera que o post pode ter no maximo ("POST " + 500) caracteres
				if (strlen(buffer) > strlen("POST ")) {
					int size_of_post = strlen("POST ") + SIZE_OF_MESSAGE + 1;
					char msg[size_of_post];
					strncpy(msg, buffer, size_of_post);

					msg[size_of_post - 1] = '\0';

					send(mysocket, msg, strlen(msg), 0);
				}

			} else if (strcmp(pch, "MUTE") == 0) {

				// se o nick contem espacos, vai desconsiderar
				if (counter <= 1) {

					if (strlen(buffer) > strlen("MUTE ")) {
						int size_of_nick_message = strlen("MUTE ")
								+ SIZE_OF_NICK + 1;
						char msg[size_of_nick_message];
						strncpy(msg, buffer, size_of_nick_message);

						msg[size_of_nick_message - 1] = '\0';

						send(mysocket, msg, strlen(msg), 0);
					}
				}

			} else if (strcmp(pch, "UNMUTE") == 0) {

				// se o nick contem espacos, vai desconsiderar
				if (counter <= 1) {

					if (strlen(buffer) > strlen("UNMUTE ")) {
						int size_of_nick_message = strlen("UNMUTE ")
								+ SIZE_OF_NICK + 1;
						char msg[size_of_nick_message];
						strncpy(msg, buffer, size_of_nick_message);

						msg[size_of_nick_message - 1] = '\0';

						send(mysocket, msg, strlen(msg), 0);
					}
				}

			} else if (strcmp(pch, "CLOSE") == 0) {
				send(mysocket, "CLOSE", strlen("CLOSE"), 0);
				close(mysocket);
				return EXIT_SUCCESS;
			}

			/**
			 * Se o comando digitado nao for um dos acima, nao faz
			 * nada e espera pela proxima acao
			 */

		}
	}
	return EXIT_SUCCESS;
}
