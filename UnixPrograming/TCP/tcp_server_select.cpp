#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>  // FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/select.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char const* argv[]) {
    const int port = 29980;  // server listened port
    const int max_num_clients = 30;
    int master_fd, established_fd;
    int actived_fd, max_fd;
    int client_fds[max_num_clients] = {0}, client_fd;
    int opt = 1;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    int backlog = 3;
    int recv_len = -1;
    char buffer[1024] = {0};
    fd_set fds;  // set of socket file descriptor
    std::string message("Got it! Happy to make a connection to you");

    // init all items to 0
    for (int i = 0; i < max_num_clients; ++i) {
        client_fds[i] = 0;
    }
    /**
     * @brief           1. create master socket file descriptor
     */
    if ((master_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        close(master_fd);
        perror("Create socket failed!");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief 1.1 set master socket to allow multiple connections. This is completely optional.
     */
    if (setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        close(master_fd);
        perror("setsockopt failed!");
        exit(EXIT_FAILURE);
    }
    /**
     * @brief           2. bind the socket to the address and port specified in addr
     *                     which is custom sockaddr_in data struct
     */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  // localhost
    addr.sin_port = htons(port);
    if (bind(master_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(master_fd);
        perror("bind failed!");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief           3. Put the server in passive mode, where it wait for the client to
     *                      approach the server to make a connection
     * 
     * @param backlog   the maximum length to which the queue of pending connections for sockfd may grow.
     *                  If a connection request arrives when the queue is full,
     *                      1) the client may receive an ECONNREFUSED error or
     *                      2) the request may be ignored so that a later reattempt at connection succeeds
     *                          if the underlying protocol supports retransmission
     */
    if (listen(master_fd, backlog) < 0) {
        close(master_fd);
        perror("Listen failed!");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // clear the socket set
        FD_ZERO(&fds);

        // add the master file descriptor to set
        FD_SET(master_fd, &fds);
        max_fd = master_fd;

        // add children sockets to set
        for (int i = 0; i < max_num_clients; ++i) {
            client_fd = client_fds[i];
            // if valid socket file descriptor then add to read list
            if (client_fd > 0) FD_SET(client_fd, &fds);
            // highest file descriptor number, need it for the select function
            if (client_fd > max_fd) max_fd = client_fd;
        }

        // wait for activity on one of sockets, timeout is null, so wait indefinitely
        actived_fd = select(max_fd + 1, &fds, NULL, NULL, NULL);
        if (actived_fd < 0 && errno != EINTR) {
            std::cout << "select error!" << std::endl;
        }

        // if something happended on the master socket, it's an incomming connection
        if (FD_ISSET(master_fd, &fds)) {
            /**
             * @brief   4. Extract the first connection request on the pending connections
             *              from the listening socket
             */
            if ((established_fd = accept(master_fd, reinterpret_cast<struct sockaddr*>(&addr),
                        reinterpret_cast<socklen_t*>(&addr_len))) < 0) {
                    perror("accept error");
                    exit(EXIT_FAILURE);
            }

            // Connection is eatablished, send message
            if (send(established_fd, message.c_str(), message.size(), 0) < 0) {
                perror("send failed!");
            }

            // add the socket to array sockets
            for (int i = 0; i < max_num_clients; ++i) {
                if (client_fds[i] == 0) {
                    client_fds[i] = established_fd;
                    std::cout << "Adding socket to the array of sockets as " << i << std::endl;
                    break;
                }
            }
        } else {
            // check some io operation on some other sockets
            for (int i = 0; i < max_num_clients; ++i) {
                int fd = client_fds[i];
                if (FD_ISSET(fd, &fds)) {
                    // check if it's closing and read the incomming message
                    if ((recv_len = read(fd, buffer, 1024)) == 0) {
                        // Somebody disconnected, get his details and print
                        getpeername(fd, reinterpret_cast<struct sockaddr*>(&addr),
                                reinterpret_cast<socklen_t*>(&addr_len));
                        std::cout << "IP " << inet_ntoa(addr.sin_addr)
                                << " Port " << ntohs(addr.sin_port) << " disconnect" << std::endl;
                        close(fd);
                        client_fds[i] = 0;
                    } else if (recv_len > 0) {
                        buffer[recv_len] = '\0';
                        std::cout << "read(" << recv_len << "): " << buffer << std::endl << std::endl;
                        if (send(established_fd, message.c_str(), message.size(), 0) < 0) {
                            perror("send failed!");
                        }
                    }
                }
            }
        }
        usleep(1000);
    }
    /**
     * @brief           6. transfer data has been finished, close it.
     */
    close(master_fd);

    return 0;
}
