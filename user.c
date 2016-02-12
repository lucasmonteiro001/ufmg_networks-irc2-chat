/*
 * user.c
 *
 *  Created on: Nov 9, 2015
 *      Author: lucas
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "user.h"

User createUser(int index, char nick[SIZE_OF_NICK], int users_ids[MAX_USERS],
		User all_users[MAX_USERS]) {

	// seta que o usuario existe
	users_ids[index] = 1;

	int i = 0;

	all_users[index].index = index;
	strcpy(all_users[index].nick, nick);

	// inicia todos os clientes como mutados
	for (i = 0; i < MAX_USERS; i++) {
		all_users[index].muted[i] = MUTED;
	}

	for (i = 0; i < MAX_USERS; i++) {
		// se ha um cliente disponivel, seta que ele
		// pode ouvir do novo usuario se conectando
		if (users_ids[i] != 0 && index != i) {
			all_users[index].muted[i] = UNMUTED;
		}
	}

	updateCurrUsers(index, users_ids, all_users);

	return all_users[index];

}

/**
 * Funcao que percorre todos os usuarios que ja estao no sistema e seta
 * que cada usuario existente pode receber mensagens do usuario que
 * acabou de ser criado
 */
void updateCurrUsers(int index, int users_ids[MAX_USERS],
		User all_users[MAX_USERS]) {

	for (int i = 0; i < MAX_USERS; i++) {

		if (i != index && users_ids[i] != 0) {

			all_users[i].muted[index] = UNMUTED;
		}
	}
}

/**
 * Funcao que remove das estruturas de controle o usuario que esta
 * no index <index>
 */
void removeUserById(int index, int users[MAX_USERS], User all_users[MAX_USERS]) {
	users[index] = 0;
	updateRemovedUser(index, users, all_users);
}

/**
 * Remove um usuario das estruturas de controle dado um nick
 */
User removeUser(char nick[SIZE_OF_NICK], int users[MAX_USERS],
		User all_users[MAX_USERS]) {

	User removed;

	// percorre todos os usuarios
	for (int i = 0; i < MAX_USERS; i++) {

		// se acho o usuario que se quer remover
		if (strcmp(nick, all_users[i].nick) == 0) {

			removed.index = all_users[i].index;
			all_users[i].index = -1; // coloca index invalido

			strcpy(removed.nick, all_users[i].nick);
			strcpy(all_users[i].nick, "");

			for (int j = 0; j < MAX_USERS; j++) {
				removed.muted[j] = all_users[i].muted[j];
				all_users[i].muted[j] = MUTED; // reinicia estrutura de muted
			}

			users[i] = 0;

			updateRemovedUser(i, users, all_users);

		}
	}

	return removed;

}

/**
 * Percorre todos os usuarios atuais do sistema e atualiza a posicao do
 * usuario que foi removido.
 */
void updateRemovedUser(int index, int users_ids[MAX_USERS],
		User all_users[MAX_USERS]) {

	// realiza update no vetor que contem os indexes sobre quais posicoes estao ocupadas
	for (int i = 0; i < MAX_USERS; i++) {
		if (i == index) {
			users_ids[i] = 0;
			break;
		}

	}

	// atualiza o index nos usuarios
	for (int i = 0; i < MAX_USERS; i++) {
		all_users[i].muted[index] = MUTED;
	}
}

/**
 * Muta um usuario, para tal atualiza a estrutura de controle do usuario
 */
void muteUser(int who_is_muting, int muted, User all_users[MAX_USERS]) {

	all_users[muted].muted[who_is_muting] = MUTED;

}

/**
 * "Desmuta" um usuario, para tal atualiza a estrutura de controle do usuario
 */
void unmuteUser(int who_is_unmuting, int muted, User all_users[MAX_USERS]) {
	all_users[muted].muted[who_is_unmuting] = UNMUTED;
}

/**
 * Retorna um usuario dado seu nome
 */
User getUserByName(char name[SIZE_OF_NICK], User all_users[MAX_USERS]) {

	for (int i = 0; i < MAX_USERS; i++) {
		if (strcmp(name, all_users[i].nick) == 0) {
			return all_users[i];
		}
	}

}

/**
 * Retorna um usuario dado seu index na estrutura de controle
 */
User getUserByIndex(int index, User all_users[MAX_USERS]) {

	return all_users[index];

}
