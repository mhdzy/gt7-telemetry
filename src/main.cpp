#include "../include/main.hpp"

void SIGINT_handler(int signal) {
  printf("\n%s%d\n", "handled signal ", signal);
  _exit(0);
}

/*
  main
*/
int main() {
  /* log all messages above DEBUG */
  spdlog::set_level(spdlog::level::debug);

  /* setup libsodium library for use */
  if (sodium_init() < 0) {
    perror("libsodium could not initialize");
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

  /* create a UDP socket */
  if ((client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    std::perror("cannot create socket");
    return -1;
  } else {
    spdlog::debug("client_fd created");
  }

  /* setup our local client socket */
  memset((char *)&client_addr, 0, sizeof(client_addr));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  client_addr.sin_port = htons(PORT);

  /* setup a send/server socket */
  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("192.168.0.14");
  server_addr.sin_port = htons(SEND_PORT);

  /* force socket to bind */
  std::uint32_t ssock_opt_ = 1;
  setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &ssock_opt_,
             sizeof(ssock_opt_));
  setsockopt(client_fd, SOL_SOCKET, SO_REUSEPORT, &ssock_opt_,
             sizeof(ssock_opt_));

  if (bind(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) <
      0) {
    std::perror("bind failed");
    return -1;
  } else {
    spdlog::debug("socket bound");
  }

  // TODO: make this periodic, every 10s
  std::string hb_msg = "A";
  if (sendto(client_fd, hb_msg.c_str(), hb_msg.size(), 0,
             (struct sockaddr *)&server_addr,
             sizeof(server_addr)) != hb_msg.size()) {
    std::perror("send failed!");
    return -1;
  } else {
    spdlog::debug("sent socket heartbeat");
  }

  // main loop
  spdlog::debug("waiting to receive...");
  std::vector<unsigned char> filebytes;

  printf("max packets: %d\n", MAXPACKETS);
  for (std::uint32_t i = 0; i < MAXPACKETS; i++) {
    // receive raw bytes & fit into vector
    server_recv_len =
        recvfrom(client_fd, server_recv_buf, BUFSIZE, 0,
                 (struct sockaddr *)&server_addr, &server_addr_len);
    printf("(%d) %s %d %s %s\n", i, "received", server_recv_len, "bytes from",
           (char *)&server_addr.sin_addr.s_addr);

    // final value terminator?
    if (server_recv_len > 0)
      server_recv_buf[server_recv_len] = 0;

    // dynamically allocate space for the packet into appropriately sized buffer
    unsigned char packet[server_recv_len];
    memcpy(&packet, &server_recv_buf, server_recv_len);

    // parse the nonce from the packet
    unsigned char nonce[8];
    parse_nonce(nonce, packet);

    // full key is "Simulator Interface Packet GT7 ver 0.0"
    const unsigned char key[33] = "Simulator Interface Packet GT7 v";

    std::uint32_t salsa20_result = crypto_stream_salsa20_xor(
        server_recv_buf, packet, server_recv_len, nonce, key);

    /*
    printf("decryption status: %d\n", salsa20_result);
    printf("b000: ");
    for (std::uint16_t m = 0; m < server_recv_len; m++) {
      printf("%2x ", server_recv_buf[m]);
      if (m % 4 == 3)
        printf(" ");
      if (m % 16 == 15)
        printf("\nb%03d: ", m);
    }
    printf("\n");
    */

    /* decrypt packet bytes here */
    GT7Packet ppacket = parse_bytes(packet);
    printf("pos (x, z): %f, %f\n", ppacket.position[0], ppacket.position[2]);
  }

  close(client_fd);

  return 0;
}