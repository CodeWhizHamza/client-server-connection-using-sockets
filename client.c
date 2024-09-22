#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int sockfd = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in serv_addr;
    long file_size;
    FILE *fp;

    if (argc != 3)
    {
        printf("\n Usage: %s <ip of server> <filename_to_save>\n", argv[0]);
        return 1;
    }

    memset(recvBuff, '0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    // Receive file size
    recv(sockfd, &file_size, sizeof(long), 0);
    printf("File size: %ld bytes\n", file_size);

    // Open file for writing
    fp = fopen(argv[2], "wb");
    if (fp == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    long total_bytes_received = 0;
    size_t bytes_received;
    while (total_bytes_received < file_size &&
           (bytes_received = recv(sockfd, recvBuff, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(recvBuff, 1, bytes_received, fp);
        total_bytes_received += bytes_received;
        printf("\rProgress: %.2f%%", (float)total_bytes_received / file_size * 100);
        fflush(stdout);
    }

    printf("\nFile received successfully\n");

    fclose(fp);
    close(sockfd);
    return 0;
}