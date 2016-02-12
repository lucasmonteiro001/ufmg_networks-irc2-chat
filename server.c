/*
 * @brief
 * manage multiple connections with FD_SET
 *
 * @author Silver Moon
 *
 * */

#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include "user.h"

#define TRUE   1
#define FALSE  0
#define MAX_PENDING_CONNECTIONS 30

/**
 * Imprime uma lista que verifica se o socket na posicao i contem algum cliente
 */
void printUserIds(int users_ids[MAX_USERS]) {
	printf("\n");
	for (int i = 0; i < MAX_USERS; i++) {
		printf("[%d]\t", i);
	}
	printf("\n");
	for (int i = 0; i < MAX_USERS; i++) {
		printf(" %d\t", users_ids[i]);
	}
	printf("\n\n");
}

/**
 * Codigo para depuracao que imprime a lista de usuarios que mutaram o User New
 */
void printMutedList(User new) {
	printf("\nUSER: %s >>> MUTED LIST", new.nick);
	for (int i = 0; i < MAX_USERS; i++) {
		printf("\n[%d] %d\t", i, new.muted[i]);
	}
	printf("\n");
}

/**
 * Codigo para depuracao que imprime a lista de usuarios que mutaram o User
 * com index <index>
 */
void printMutedListWithIndex(int index, User all_users[MAX_USERS]) {
	printf("\nUSER: %s >>> MUTED LIST", all_users[index].nick);
	for (int i = 0; i < MAX_USERS; i++) {
		printf("\n[%d] %d\t", i, all_users[index].muted[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		perror("Parametros invalidos\n");
		printf("Correto: ./server <port_number>\n");
		return EXIT_FAILURE;
	}

	int opt = TRUE;

	int master_socket, addrlen, new_socket, client_socket[MAX_USERS],
			max_clients = MAX_USERS, activity, i, valread, s;

	// Vetor de controle, o qual contem um indicador para checar se o
	// socket em cada posicao contem um cliente ativo ou nao
	int users_ids[MAX_USERS];
	// Vetor que vai conter as informacoes de todos os usuarios
	User all_users[MAX_USERS];
	memset(users_ids, 0, MAX_USERS * sizeof(int));

	struct sockaddr_in address;

	char buffer[SIZE_OF_MESSAGE];

	fd_set readfds;

	// inicializa todos os cliente com 0, o que significa que esses
	// espacos estao disponiveis para receber clientes
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	// cria o master_socket
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// seta o master_socket para receber multiplas conexoes
	if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
			sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( atoi(argv[1]) );

	// Faz o bind do socket com o localhost e porta passada como parametro
	if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// especifica o numero maximo de conexoes para o master_socket
	if (listen(master_socket, MAX_PENDING_CONNECTIONS) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// libera o master_socket para receber conexoes
	addrlen = sizeof(address);
	puts("Waiting for connections...");
	while (TRUE) {

		/**********************************/
		/* AS IMPRESSOES ABAIXO SAO PARA VERIFICACAO
		 * DA ACURACIA DOS DADOS */
		/**********************************/
		//		printUserIds(users_ids);
		//		printf("Clientes:\t");
		//		for (int j = 0; j < MAX_USERS; j++) {
		//			if (users_ids[j] > 0) {
		//				printf("%s\t", all_users[j].nick);
		//			}
		//		}
		//		printf("\nClient-socket:\n");
		//		for (int j = 0; j < MAX_USERS; j++) {
		//			printf("%d\t", client_socket[j]);
		//		}
		//		printf("\n");
		/**********************************/
		/**********************************/

		// limpa o socket set
		FD_ZERO(&readfds);

		// adiciona o master_socket para o set
		FD_SET(master_socket, &readfds);

		// adiciona os outros sockets (referentes aos clientes que
		// podem se conectar
		for (i = 0; i < max_clients; i++) {
			s = client_socket[i];
			if (s > 0) {
				FD_SET( s , &readfds);
			}
		}

		// espera por alguma atividade em algum socket
		activity = select(max_clients + MAX_PENDING_CONNECTIONS, &readfds,
				NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			printf("select error");
		}

		// Se houve alguma atividade no master_socket, isso quer dizer que ha uma
		// nova conexao. Logo, faz um tratamento para criar um novo usuario e
		// inserir nas estruturas de controle.
		if (FD_ISSET(master_socket, &readfds)) {

			if ((new_socket = accept(master_socket,
					(struct sockaddr *) &address, (socklen_t*) &addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			// adiciona o novo socket para o lista de sockets em alguma posicao que esteja vazia
			for (i = 0; i < max_clients; i++) {
				s = client_socket[i];
				if (s == 0) {
					client_socket[i] = new_socket;
					//printf("Adding to list of sockets as %d\n", i);

					// Adiciona para a lista controle de usuarios
					createUser(i, "", users_ids, all_users);

					i = max_clients;

				}
			}
		}

		// Se nao, eh alguma msg chegando de algum usuario conectado,
		// abaixo faz-se os devidos tratamentos
		for (i = 0; i < max_clients; i++) {
			s = client_socket[i];

			if (FD_ISSET( s , &readfds)) {

				if ((valread = read(s, buffer, sizeof(buffer))) == 0) {

					// Se a conexao foi fechada no lado do cliente

					getpeername(s, (struct sockaddr*) &address,
							(socklen_t*) &addrlen);

					// Fecha o socket e marco-o como 0 para reuso
					close(s);
					client_socket[i] = 0;

					// remove o usuario da lista de controle de usuarios
					removeUserById(i, users_ids, all_users);
				}

				// Se chegou alguma mensagem do usuario
				else {

					buffer[valread] = '\0';

					char first[SIZE_OF_COMMAND];
					char second[SIZE_OF_MESSAGE];

					// Inicializa a string first como vazia
					for (int k = 0; k < SIZE_OF_COMMAND; k++) {
						first[k] = '\0';
					}

					// Inicializa a string second como vazia
					for (int k = 0; k < SIZE_OF_MESSAGE; k++) {
						second[k] = '\0';
					}

					// Os dois (for)'s seguintes dividem a msg recebida
					// entre <comando> e <parametro>
					int pos_k = 0;
					for (int k = 0; k < SIZE_OF_COMMAND; k++) {
						if (buffer[k] != ' ' && buffer[k] != '\n') {
							first[k] = buffer[k];
						} else {
							pos_k = k;
							break;
						}
					}

					for (int k = pos_k + 1; k < strlen(buffer) - 1; k++) {
						if (buffer[k] != '\0' && buffer[k] != '\n') {
							second[k - pos_k - 1] = buffer[k];
						} else {
							second[k - pos_k - 1] = '\0';
							break;
						}
					}

					if (strcmp(first, "NICK") == 0) {

						// salva o nick do usuario
						strcpy(all_users[i].nick, second);

						// printf("[%d] Novo usuario conectado: %s\n", i,
						// all_users[i].nick);

					} else if (strcmp(first, "POST") == 0) {

						// Descobre o usuario que enviou a msg
						User sender = getUserByIndex(i, all_users);

						//printMutedList(sender);

						// Envia a mensagem para todos os usuarios que nao mutaram
						// o usuario sender.
						for (int k = 0; k < MAX_USERS; k++) {

							if (sender.muted[k] == UNMUTED) {

								User receiver = getUserByIndex(i, all_users);

								// monta a mensagem
								char msg[strlen("NEW ") + strlen(receiver.nick)
										+ 1 + SIZE_OF_MESSAGE];
								memcpy(msg, "NEW ", strlen("NEW "));
								memcpy(&msg[4], receiver.nick, strlen(receiver.nick));
								memcpy(&msg[4 + strlen(receiver.nick)], " ", 1);
								memcpy(&msg[4 + strlen(receiver.nick) + 1], second, strlen(second));
								msg[4 + strlen(receiver.nick) + 1 + strlen(
										second)] = '\0';

								// envia a mensagem formatada
								send(client_socket[k], msg, sizeof(msg), 0);
							}
						}

					} else if (strcmp(first, "CLOSE") == 0) {

						getpeername(s, (struct sockaddr*) &address,
								(socklen_t*) &addrlen);

						// if (strlen(all_users[i].nick) == 0) {
						//	printf("usuario sem nick desconectado.");
						// } else {
						//	printf("O usuario: %s foi desconectado.",
						//			all_users[i].nick);
						// }

						// Fecha o socket e marco-o como zero para reuso
						close(s);
						client_socket[i] = 0;

						removeUserById(i, users_ids, all_users);

					} else if (strcmp(first, "MUTE") == 0) {

						User muting = getUserByIndex(i, all_users);
						User toBeMuted = getUserByName(second, all_users);

						// printf("O usuario <%s> esta mutando o usuario <%s>\n",
						//		muting.nick, toBeMuted.nick);


						// muta o usuario
						muteUser(muting.index, toBeMuted.index, all_users);

					} else if (strcmp(first, "UNMUTE") == 0) {

						//User unmuting = getUserByIndex(i, all_users);
						User toBeUnmuted = getUserByName(second, all_users);

						// printf(
						//		"O usuario <%s> esta desmutando o usuario <%s>\n",
						//		unmuting.nick, toBeUnmuted.nick);

						// "desmuta" o usuario
						unmuteUser(i, toBeUnmuted.index, all_users);

					} else {

						// Apenas zera os vetores de controle
						for (int k = 0; k < SIZE_OF_MESSAGE; k++)
							buffer[k] = '\0';
						for (int k = 0; k < sizeof(first); k++)
							first[k] = '\0';
						for (int k = 0; k < sizeof(second); k++)
							second[k] = '\0';
					}

				}
			}
		}
	}

	return 0;
}
