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
#include <fstream>
#include <iostream>
#include <vector>

#include <sodium.h>

#include "spdlog/spdlog.h"

#include "File.hpp"
#include "packet.hpp"

#ifndef _XOPEN_SOURCE_EXTENDED
    #if defined(__APPLE__)
        #define _XOPEN_SOURCE_EXTENDED 1
    #elif defined(__linux__) && defined(__arm__)
        #define _XOPEN_SOURCE_EXTENDED 1
    #endif
#endif

#define SEND_PORT 33739
#define BIND_PORT 33740
#define BUFSIZE 65535

char *MY_IP_ADDR = (char*)"192.168.0.32";
char *PS5_IP_ADDR = (char*)"192.168.0.26";

// heartbeat delay is ~10 seconds
// SET TO TWO SECONDS FOR DEBUG
std::uint32_t hb_delay_micros = 2000000;

std::uint32_t max_packets = pow(2, 16);

std::ofstream output_file;

/**
 * @brief
 *
 * @param signal
 */
void SIGINT_handler(int signal);

/**
 * @brief
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char **argv);
