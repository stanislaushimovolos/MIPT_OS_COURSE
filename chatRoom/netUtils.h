//
// Created by superstraz on 11/23/18.
//

#ifndef UNTITLED3_NETUTILS_H
#define UNTITLED3_NETUTILS_H

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>

#define ACCEPT_QUEUE_SIZE  5

int open_server_sock(uint16_t port);

int server_sock();

int do_accept(int sd);

void fill_address(struct sockaddr_in *addr, uint16_t port);

void do_bind(int sd, struct sockaddr_in addr);

void do_listen(int sd);

void display_address();


#endif //UNTITLED3_NETUTILS_H
