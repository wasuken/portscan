#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

typedef struct{
  char ip_address[16];
  int start_port;
  int end_port;
} ScanInfo;

void *port_scan(void *arg){
  int sockfd;
  struct sockaddr_in target;
  ScanInfo *info = (ScanInfo *)arg;

  for(int i=info->start_port; i<info->end_port; i++){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    target.sin_family = AF_INET;
    target.sin_port = htons(i);
    target.sin_addr.s_addr = inet_addr(info->ip_address);

    if (connect(sockfd, (struct sockaddr *)&target, sizeof(target)) == 0) {
      printf("Port %d is open\n", i);
    } else {
      printf("Port %d is closed\n", i);
    }

    close(sockfd);

  }
  pthread_exit(NULL);
  free(arg);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <IP Address>\n", argv[0]);
    exit(1);
  }
  pthread_t threads[50];
  for (int i = 0; i < 50; ++i){
    ScanInfo *info = malloc(sizeof(ScanInfo));
    strcpy(info->ip_address, argv[1]);
    info->start_port = i * (65535 / 50);
    info->end_port = (i + 1) * (65535 / 50) - 1;
    if(pthread_create(&threads[i], NULL, port_scan, info) != 0){
      perror("pthread_create");
      exit(1);
    }
  }

  for (int i = 0; i < 50; ++i) {
    if(pthread_join(threads[i], NULL) != 0){
      perror("pthread_join");
      exit(1);
    }
  }

  return 0;
}

