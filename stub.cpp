#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "stub.h"

int sock;

__attribute__((constructor)) void init(void) {
  struct sockaddr_in serv_addr;
  int one = 1;
  int twelve = 12;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (setsockopt(sock, SOL_TCP, TCP_NODELAY, &one, sizeof(one)) != 0 || setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &twelve, sizeof(int)) != 0) {
    perror("Error setting buffer size to zero\n");
    exit(1);
  }


  if (sock < 0) {
    perror("Error opening socket");
    exit(1);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(1337);

  if(inet_pton(AF_INET, "10.133.7.101", &serv_addr.sin_addr) <= 0)
  {
    perror("inet_pton error occured");
    exit(1);
  }

  printf("Connecting to remote socket\n");
  if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connect Failed");
    exit(1);
  }
}

__attribute__((destructor)) void destroy(void) {
  close(sock);
}

void pinMode(int pin, int type) {
  return;
}

typedef enum {
  READ_PIN    = 0,
  WRITE_PIN   = 1,
  WRITE_SERVO = 2,
} packet_type;

typedef struct {
  packet_type type;
  int pin;
  int value;
} packet;

int digitalRead(int pin) {
  int result;
  packet p = {READ_PIN, pin};
  // printf("Reading from pin %d\n", pin);
  write(sock, &p, sizeof(p));

  read(sock, &result, sizeof(result));
  return result;
}

void digitalWrite(int pin, int value) {
  packet p = {WRITE_PIN, pin, value};
  // printf("Writing %d to pin %d\n", value, pin);
  write(sock, &p, sizeof(p));
}

void Servo::attach(int pin) {
  this->pin = pin;
}

void Servo::write(int value) {
  packet p = {WRITE_SERVO, this->pin, value};
  // printf("Writing %d to servo at pin %d\n", value, this->pin);
  ::write(sock, &p, sizeof(p));
}

long map(long x, long in_min, long in_max, long out_min, long out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay(int milliseconds) {
  usleep(milliseconds * 1000);
}
