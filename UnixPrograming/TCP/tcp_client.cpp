#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char const* argv[]) {
    const int port = 29980;
    const std::string host_addr("127.0.0.1");
    int client_fd;
    struct sockaddr_in server_addr;
    int read_len = -1;
    char buffer[1024];
    std::string message("Helllo, I'm trying to make a connection to you.");

    /**
     * @brief   1. create socket file descriptor
     */
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Create socket failed!");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    // convert IPv4 and Ipv6 address from text to binary
    if (inet_pton(AF_INET, host_addr.c_str(), &server_addr.sin_addr.s_addr) <= 0) {
        perror("Address convert failed, maybe invalid address or address not supported");
        exit(EXIT_FAILURE);
    }
    /**
     * @brief       2. connect the socket referred to the file descriptor sockfd to the address specified by addr
     * 
     * @param sockfd(1)     
     * @param addr(2)
     * @param addr_len(3)       
     * 
     */
    if (connect(client_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        perror("Connect failed!");
        exit(EXIT_FAILURE);
    }

    int times = 1;  // times for try to send message
    while (times < 16) {
        /**
         * @brief   3. connection is established, send and recv data is allowed.
         */
        if (send(client_fd, message.c_str(), message.size(), 0) < 0) {
            perror("Client send message failed!");
            exit(EXIT_FAILURE);
        }

        if ((read_len = recv(client_fd, buffer, sizeof(buffer), 0)) < 0) {
            perror("Client read message failed!");
            exit(EXIT_FAILURE);
        }
        std::cout << "Time " << times <<  " Client read(" << read_len << ") :"
                << buffer << std::endl;
        times++;
        usleep(1000);
    }
    /**
     * @brief   4. transfer data has been finished, close it.
     */
    close(client_fd);

    std::cout << std::endl << "---------- Finished! ----------" << std::endl;
    return 0;
}
