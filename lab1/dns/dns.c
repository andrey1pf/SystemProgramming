
/*************************************************************************
   LAB 1

	Edit this file ONLY!

*************************************************************************/

#include "dns.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define SIZE 13001

//prime numbers for hash
#define A 54059
#define B 76963
#define C 86969
#define FIRST 131

extern unsigned int getNumOfLines(FILE*);

typedef struct Node_ {
	IPADDRESS ip;
	char* host_name[201];
	struct Node* next_node;
} Node;

unsigned int GetHash(const char* host_name) {
	unsigned int hash = FIRST;
	while (*host_name) {
		hash = (hash * A) ^ (*host_name * B);
		++host_name;
	}
	hash %= C;

	return hash % SIZE;
}

void InsertNode(DNSHandle hDNS, const char* host_name, IPADDRESS ip) {
	Node* current_node, * next_node;
	unsigned int index = GetHash(host_name);
	current_node = (Node*)malloc(sizeof(Node));
	next_node = ((Node**)hDNS)[index];
	current_node->next_node = (struct Node*)next_node;
	current_node->ip = ip;
	strcpy_s((char*)current_node->host_name, 201, host_name);
	((Node**)hDNS)[index] = (Node*)current_node;
}

DNSHandle InitDNS() {
	DNSHandle hDNS = (unsigned int)(Node*)calloc(SIZE, sizeof(Node));

	if ((Node*)hDNS != NULL) {
		return hDNS;
	}

	return INVALID_DNS_HANDLE;
}

void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath) {
	FILE* fInput = NULL;
	unsigned int dnCount = 0;

	fopen_s(&fInput, hostsFilePath, "r");
	if (NULL == fInput)
		return;

	dnCount = getNumOfLines(fInput);
	fseek(fInput, 0, SEEK_SET);

	for (size_t i = 0; i < dnCount && !feof(fInput); i++) {
		char buffer[201] = { 0 };
		unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;

		if (5 != fscanf_s(fInput, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, buffer, 201))
			continue;

		IPADDRESS ip = (ip1 & 0xFF) << 24 |
			(ip2 & 0xFF) << 16 |
			(ip3 & 0xFF) << 8 |
			(ip4 & 0xFF);

		InsertNode(hDNS, buffer, ip);
	}

	fclose(fInput);
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName) {
	unsigned int index = GetHash(hostName);
	Node* temp_node = ((Node**)hDNS)[index];

	while (temp_node != NULL) {
		if (strcmp(hostName, (const char*)temp_node->host_name) == 0) {
			return temp_node->ip;
		}

		temp_node = (Node*)temp_node->next_node;
	}

	return INVALID_IP_ADDRESS;
}

void ShutdownDNS(DNSHandle hDNS) {
	for (size_t i = 0; i < SIZE; ++i) {
		Node* current = ((Node**)hDNS)[i];
		Node* next_node = NULL;

		while (current != NULL) {
			next_node = (Node*)current->next_node;
			free(current);
			current = next_node;
		}
	}

	if ((Node*)hDNS != NULL) {
		free((Node*)hDNS);
	}
}