/*
 * frinstrclient.c
 *
 *  Created on: Mar 12, 2014
 *      Author: luigi
 */

/*
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include "frlog.hpp"
#include "frjvmti.hpp"
#include "frinstr.hpp"

static void _SendData(int sockfd, const void* data, int datalen) {
	int sent = 0;

	while (sent != datalen) {

		int res = send(sockfd, ((unsigned char *) data) + sent,
				(datalen - sent), 0);
		check_std_error(res, "Error while sending data to server");
		sent += res;
	}
}

static void _ReceiveData(int sockfd, void* data, int datalen) {
	int received = 0;

	while (received != datalen) {

		int res = recv(sockfd, ((unsigned char *) data) + received,
				(datalen - received), 0);
		check_std_error(res, "Error while receiving data from server");

		received += res;
	}
}

static int _ReceiveInt(int sockfd) {
	int data;
	_ReceiveData(sockfd, &data, sizeof(int));

	return ntohl(data);
}

static int _FrConnect() {
	struct addrinfo * addrinfo;
	int gai_res = getaddrinfo("127.0.0.1", "11357", NULL, &addrinfo);
	check_std_error(gai_res, "error getaddrinfo");

	int sockfd = socket(addrinfo->ai_family, SOCK_STREAM, 0);
	check_std_error(sockfd, "error socket");

	int conn_res = connect(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen);
	check_std_error(conn_res, "error connect");

	int flag = 1;
	int set_res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag,
			sizeof(int));
	check_std_error(set_res, "error setsockopt");

	freeaddrinfo(addrinfo);

	return sockfd;
}

void InstrClassClientServer(jvmtiEnv* jvmti, unsigned char* data, int len,
		const char* className, int* newlen, unsigned char** newdata,
		JNIEnv* jni, InstrArgs* args) {

	int sockfd;

	if (tldget()->socketfd == -1) {
		tldget()->socketfd = _FrConnect();
	}

	sockfd = tldget()->socketfd;

	jint classNameLen = htonl(strlen(className));
	jint dl = htonl(len);

	_SendData(sockfd, &classNameLen, sizeof(jint));
	_SendData(sockfd, &dl, sizeof(jint));

	_SendData(sockfd, className, strlen(className));
	_SendData(sockfd, data, len);

	size_t newClassNameLen = _ReceiveInt(sockfd);
	jint newClassBytesLen = _ReceiveInt(sockfd);

	ASSERT(newClassNameLen == strlen(className), "");
	ASSERT(newClassBytesLen > 0, "");

	char* newClassName = (char*) malloc(newClassNameLen + 1);
	_ReceiveData(sockfd, newClassName, newClassNameLen);
	newClassName[newClassNameLen] = '\0';

	ASSERT(strcmp(newClassName, className) == 0, "");

	free(newClassName);

	*newlen = newClassBytesLen;
	FrAllocate(jvmti, newClassBytesLen, newdata);

	_ReceiveData(sockfd, *newdata, newClassBytesLen);
}
