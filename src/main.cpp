#include "../include/main.hpp"

void SIGINT_handler(int signal) {
  printf("\n%s%d\n", "handled signal ", signal);
  _exit(0);
}

/**
 * @brief main
 *
 * @param argc number of arguments
 * @param argv array of arguments
 * @return main
 */
int main(int argc, char **argv) {
  std::string USAGE_("Usage: ./main <track> [ps5-ip]");

  /* log all messages above DEBUG */
  spdlog::set_level(spdlog::level::debug);

  /* setup libsodium library for use */
  if (sodium_init() < 0) {
    spdlog::error("libsodium could not initialize");
  }

  if (argc < 2) {
    spdlog::error("not enough args");
    spdlog::error(USAGE_.c_str());
    return -1;
  } else if (argc == 3) {
    PS5_IP_ADDR = argv[2];
  } else if (argc == 4) {
    PS5_IP_ADDR = argv[2];
    MY_IP_ADDR = argv[3];
  }

  /* setup some output paths */
  const std::string TRACK(argv[1]);
  const std::string DATA_PATH = "data/";
  const std::string TRACK_PATH = DATA_PATH + TRACK;
  std::string TRACK_DATA_PATH = TRACK_PATH + "/data.csv";

  /* don't crash on bad or existing dir */
  if (createDir(TRACK_PATH) < 0) {
    spdlog::warn("could not create data path; using temp file");
    TRACK_DATA_PATH = DATA_PATH + "/tmp.csv";
  }

  /* setup control+c exit handler */
  struct sigaction sig_int_handler;
  sig_int_handler.sa_handler = SIGINT_handler;
  sigemptyset(&sig_int_handler.sa_mask);
  sig_int_handler.sa_flags = 0;
  sigaction(SIGINT, &sig_int_handler, NULL);

  /* setup UDP socket vars */
  struct sockaddr_in client_addr;                  /* our address */
  struct sockaddr_in server_addr;                  /* remote address */
  socklen_t server_addr_len = sizeof(server_addr); /* length of addresses */
  std::uint32_t server_recv_len;                   /* num bytes received */
  unsigned char server_recv_buf[BUFSIZE];          /* receive buffer */
  std::uint32_t client_fd;                         /* our socket */

  /* setup our local client socket */
  memset((char *)&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr("192.168.0.13");
  client_addr.sin_port = htons(BIND_PORT);

  /* setup a send/server socket */
  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(PS5_IP_ADDR);
  server_addr.sin_port = htons(SEND_PORT);

  /* create a UDP socket */
  std::uint32_t so_status =
      (client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
  if (so_status < 0) {
    spdlog::error("cannot create socket");
    return -1;
  } else {
    spdlog::debug("client_fd created");
  }

  /* bind socket */
  // bind_socket(&client_fd, &client_addr);
  std::size_t b_status =
      bind(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr));
  if (b_status < 0) {
    spdlog::error("bind failed");
    return -1;
  } else {
    spdlog::debug("socket bound");
  }

  /*
    set the socket timeout

    packets come every 1/60 seconds, so we have 0.02 seconds between each packet
  */
  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 20000;

  if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) <
      0)
    spdlog::error("setsockopt failed\n");

  if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout) <
      0)
    spdlog::error("setsockopt failed\n");

  // prepare our output file
  output_file = std::ofstream{TRACK_DATA_PATH};
  output_file << "id,x,y,z\n";

  // TODO: make this periodic, every 10s
  send_heartbeat(client_fd, &server_addr, "ABC");

  // set our initial time
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  // main loop
  spdlog::info("waiting to receive...");
  spdlog::info("max packets: {:d}\n", max_packets);
  for (std::uint32_t i = 0; i < max_packets; i++) {
    // send heartbeat every 5s @ 60 p/sec = 300 packets

    // receive raw bytes
    server_recv_len =
        recvfrom(client_fd, server_recv_buf, BUFSIZE, 0,
                 (struct sockaddr *)&server_addr, &server_addr_len);

    if (server_recv_len == -1) {
      // skip the rest of the loop in this case
      std::chrono::steady_clock::time_point end =
          std::chrono::steady_clock::now();

      std::uint32_t diff_ms =
          std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
              .count();

      // check if we are over time
      if (diff_ms > hb_delay_micros) {
        spdlog::warn("received 0 bytes outside of diff tolerance {:d} µs",
                     diff_ms);

        // extract port info to print (debug)
        std::uint16_t port = ntohs(server_addr.sin_port);
        spdlog::debug("server addr port is: {:d}", port);

        // setting port back to send port
        server_addr.sin_port = htons(SEND_PORT);

        // check again
        port = ntohs(server_addr.sin_port);
        spdlog::debug("** UPDATED server addr port is: {:d}", port);

        send_heartbeat(client_fd, &server_addr, "A");
        begin = std::chrono::steady_clock::now();
      }

      continue;
    } else if (server_recv_len > 0) {
      // spdlog::info("received packet");
      // final value terminator?
      server_recv_buf[server_recv_len] = 0;
    } else {
      spdlog::error("uh oh!");
    }

    // dynamically allocate space for the packet into appropriately sized buffer
    // this is used during decryption, and useless afterwards
    unsigned char packet[server_recv_len];
    memcpy(&packet, &server_recv_buf, server_recv_len);

    // parse the nonce (iv) from the packet
    unsigned char nonce[8];
    parse_nonce(nonce, packet);

    // full key is "Simulator Interface Packet GT7 ver 0.0"
    const unsigned char key[33] = "Simulator Interface Packet GT7 v";

    // decrypt and store in server_recv_buf
    crypto_stream_salsa20_xor(server_recv_buf, packet, server_recv_len, nonce,
                              key);

    // printf("decryption status: %d\n", salsa20_result);
    printf("b000: ");
    for (std::uint16_t m = 0; m < server_recv_len; m++) {
      printf("%2x  ", server_recv_buf[m]);
      if (m % 4 == 3)
        printf(" ");
      if (m % 16 == 15)
        printf("\nb%03d: ", m);
    }
    printf("\n\n");
    

    /* decrypt packet bytes here */
    GT7Packet ppacket = parse_bytes(server_recv_buf, server_recv_len);
    output_file << std::to_string(ppacket.ticks) + "," +
                       std::to_string(ppacket.position[0]) + "," +
                       std::to_string(ppacket.position[1]) + "," +
                       std::to_string(ppacket.position[2]) + "\n";
  }

  close(client_fd);

  return 0;
}