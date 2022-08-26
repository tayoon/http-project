#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUF_LEN 256

struct URL {
  char host[BUF_LEN];
  char path[BUF_LEN];
  unsigned short port;
};

int main(int argc, char **argv) {
  char send_buf[BUF_LEN];

  struct URL url = {"example.com", "/", 80};

  printf("GET http://%s%s\n", url.host, url.path);

  // Name resolve
  struct addrinfo hints, *res;
  struct in_addr addr;
  // 0 clear
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;
  char *serviceType = "http";
  int err;
  if ((err = getaddrinfo(url.host, serviceType, &hints, &res)) != 0) {
    printf("error %d\n", err);
    return 1;
  }

  // Socket file descriptor
  int s;
  // Create a socket
  if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    fprintf(stderr, "socket creation failed\n");
    return 1;
  }

  // Connect to the server
  if (connect(s, res->ai_addr, res->ai_addrlen) != 0) {
    fprintf(stderr, "connect failed\n");
    return 1;
  }

  // HTTP GET
  sprintf(send_buf, "GET %s HTTP/1.1\r\n", url.path);
  write(s, send_buf, strlen(send_buf));

  sprintf(send_buf, "Host: %s:%d\r\n", url.host, url.port);
  write(s, send_buf, strlen(send_buf));

  sprintf(send_buf, "\r\n");
  write(s, send_buf, strlen(send_buf));

  // Read everything
  while (1) {
    char buf[BUF_LEN];
    int read_size;
    read_size = read(s, buf, BUF_LEN);

    if (read_size > 0) {
      write(1, buf, read_size);
    } else {
      break;
    }
  }

  // Close socket
  close(s);

  return 0;
}