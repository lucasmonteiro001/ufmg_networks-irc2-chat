/*
 * user.h
 *
 *  Created on: Nov 9, 2015
 *      Author: lucas
 */

#ifndef USER_H_
#define USER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_OF_NICK 17
#define SIZE_OF_COMMAND 8
#define SIZE_OF_MESSAGE 500 + 8
#define MAX_USERS 30
#define MUTED 1
#define UNMUTED 0

typedef struct User {
	int index;
	char nick[SIZE_OF_NICK];
	short int muted[MAX_USERS];
} User;

User createUser(int index, char nick[SIZE_OF_NICK], int users_ids[MAX_USERS],
		User all_users[MAX_USERS]);

void removeUserById(int index, int users[MAX_USERS], User all_users[MAX_USERS]);

void updateCurrUsers(int index, int users_ids[MAX_USERS],
		User all_users[MAX_USERS]);

User removeUser(char nick[SIZE_OF_NICK], int users[MAX_USERS],
		User all_users[MAX_USERS]);

void updateRemovedUser(int index, int users_ids[MAX_USERS],
		User all_users[MAX_USERS]);

void muteUser(int who_is_muting, int muted, User all_users[MAX_USERS]);

void unmuteUser(int who_is_unmuting, int muted, User all_users[MAX_USERS]);

User getUserByName(char name[SIZE_OF_NICK], User all_users[MAX_USERS]);

User getUserByIndex(int index, User all_users[MAX_USERS]);

#endif /* USER_H_ */
