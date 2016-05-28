#ifdef SIMULATOR
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "stub.h"

// const char IP[] = "127.0.0.1";
const char IP[] = "10.133.7.101";
int sock;
uint32_t start_time;

uint32_t _micros() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return 1000000 * tv.tv_sec + tv.tv_usec;
}

int init_sock(const char *ip, int port) {
  struct sockaddr_in serv_addr;
  int one = 1;
  int twelve = 12;
  int sock;

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
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0)
  {
    perror("inet_pton error occured");
    exit(1);
  }

  if( connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Connect Failed");
    exit(1);
  }
  return sock;
}
__attribute__((constructor)) void init(void) {
  start_time = _micros();

  printf("Connecting to remote socket\n");
  sock = init_sock(IP, 1337);
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

void Serial::begin(int speed) {
  this->sock = -1;
  printf("Connecting to serial port\n");
  this->sock = init_sock(IP, 1338);
}

int Serial::available() {
  fd_set set;
  struct timeval timeout;
  int n = 0;

  if (this->sock == -1) {
    printf("Reconnecting to serial\n");
    begin(9600);
    this->write("HELO");
  }

  FD_ZERO (&set);
  FD_SET (this->sock, &set);

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  select(this->sock + 1, &set, NULL, NULL, &timeout);
  if (FD_ISSET(this->sock, &set)) {
    ioctl(sock, FIONREAD, &n);
    if (n == 0) {
      close(this->sock);
      this->sock = -1;
    }
    return n;
  }
  return 0;
}

void Serial::write(char value) {
  ::write(this->sock, &value, 1);
}

void Serial::write(std::string str) {
  ::write(this->sock, str.c_str(), str.length());
}

void Serial::write(char *buf, int len) {
  ::write(this->sock, buf, len);
}

void Serial::readBytes(char *buf, int len) {
  ::read(this->sock, buf, len);
}

long map(long x, long in_min, long in_max, long out_min, long out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void delay(int milliseconds) {
  usleep(milliseconds * 1000);
}

uint32_t micros() {
  return _micros() - start_time;
}

#endif
