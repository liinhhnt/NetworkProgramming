#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char *SERVER_IP = "127.0.0.1";
int SERV_PORT = 1255;
int MAXLINE = 255;

main(int argc, char **argv)
{
  int sockfd, n, from_len;
  struct sockaddr_in servaddr, from_socket;
  socklen_t addrlen = sizeof(from_socket);
  char sendline[MAXLINE], recvline[MAXLINE + 1];
  FILE *fp;

  if (argc != 1 && argc != 3)
  {
    fprintf(stderr, "Usage: client\n");
    fprintf(stderr, "Usage: client IP PORT\n");
    exit(EXIT_FAILURE);
  }
  else if (argc == 3)
  {
    SERVER_IP = argv[1];
    SERV_PORT = atoi(argv[2]);
  }

  // 1. Define address of server
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

  // 2. Construct socket
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
  { /* Call socket() */
    perror("\nError: ");
    exit(EXIT_FAILURE);
  }
  printf("creat socket\n");

  // 3. Join server
  while (1)
  {
    sprintf(sendline, "hello, server"); // Hello server
    n = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (n < 0)
    {
      perror("\nError: ");
      continue;
    }
    // wait for response from server
    n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&from_socket, &addrlen);
    if (n < 0)
    {
      perror("\nError: ");
      continue;
    }

    if (strstr(recvline, "Waiting for others...") != NULL)
    {
      recvline[n] = '\0';
      printf("%s\n", recvline);
      break;
    }
    sleep(1);
  }

  // Communicate with server
  pid_t child_id = fork();
  if (child_id == 0)
  {
    while (1)
    {
      memset(sendline, '\0', (strlen(sendline) + 1));
      fgets(sendline, MAXLINE, stdin);
      if (sendline[strlen(sendline) - 1] == '\n')
      {
        sendline[strlen(sendline) - 1] = '\0';
      }
      if (strcmp(sendline, "") == 0)
      {
        close(sockfd);
        kill(getppid(), SIGKILL);
        exit(EXIT_SUCCESS);
      }
      from_len = sizeof(struct sockaddr_in);
      n = sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, from_len);
      if (n < 0)
      {
        perror("\nError: ");
        close(sockfd);
        exit(EXIT_FAILURE);
      }
    }
  }
  else
  {
    while (1)
    {
      memset(recvline, '\0', strlen(recvline) + 1);
      int recv_len = sizeof(struct sockaddr_in);
      n = recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&servaddr, &recv_len);
      if (n < 0)
      {
        perror("\nError: ");
        close(sockfd);
        exit(EXIT_FAILURE);
      }

      recvline[n] = '\0';
      if (recvline[n - 1] == '\n')
      {
        recvline[n - 1] = '\0';
      }
      printf("Your friend: %s\n", recvline);
    }
  }
}
