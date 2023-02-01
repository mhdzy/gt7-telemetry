#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>

#include <sodium.h>

#include "spdlog/spdlog.h"

#include "packet.hpp"

#define _XOPEN_SOURCE_EXTENDED

#define IP_ADDR "192.168.0.14"
#define SEND_PORT 33739
#define PORT 33740
#define BUFSIZE 65535

// not const since this can shrink if in batch mode
std::uint32_t MAXPACKETS = pow(2, 16);

void SIGINT_handler(int signal);

int main();
