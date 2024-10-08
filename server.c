#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[BUFFER_SIZE];
    FILE *file;

    if (argc != 2)
    {
        printf("Usage: %s <file_to_send>\n", argv[0]);
        return 1;
    }

    // Open the file
    file = fopen(argv[1], "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

    while (1)
    {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);

        // Send file size first
        fseek(file, 0L, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0L, SEEK_SET);
        send(connfd, &file_size, sizeof(long), 0);

        // Send file contents
        size_t bytes_read;
        while ((bytes_read = fread(sendBuff, 1, BUFFER_SIZE, file)) > 0)
        {
            send(connfd, sendBuff, bytes_read, 0);
        }

        fseek(file, 0L, SEEK_SET); // Reset file pointer for next client
        close(connfd);
        sleep(1);
    }

    fclose(file);
    return 0;
}