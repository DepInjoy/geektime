#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char const* argv[]) {
    const int port = 29980;  // server listened port
    int server_fd, established_fd;
    int opt = 1;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    int backlog = 3;
    int recv_len = -1;
    char buffer[1024] = {0};
    std::string message("Got it! Happy to make a connection to you");

    /**
     * @brief           1. create socket file descriptor
     * 
     * @param domain(1) communication domain
     *                        AF_INET : IPv4
     *                        AF_INET6 : IPv6
     * @param type(2)   communication type
     *                        SOCK_STREAM : TCP
     *                        SOCK_DGRAM : UDP
     * @param protocol(3) Protocol value for Internet Protocol(IP), which is 0
     *                    This is the same number which appears on protocol field in the IP header of a packet
     *                    (man protocols for more details)
     */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        close(server_fd);
        perror("Create socket failed!");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief 1.1 manipulate options for the socket referred by the file descriptor sockfd
     *            This is completely optional.
     *              1) help to reuse of address and port. Prevents "address already in use" error
     */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        close(server_fd);
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
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(server_fd);
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
    if (listen(server_fd, backlog) < 0) {
        close(server_fd);
        perror("Listen failed!");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief       4. Extract the first connection request on the pending connections from the listening socket.
     *                 Create and return the connected socketfd. At this point, connection is established
     *                      between client and server, and they are ready to transfer data.
     */
    if ((established_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&addr),
            reinterpret_cast<socklen_t*>(&addr_len))) < 0) {
        perror("accept error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /**
         * @brief       5. Connection is eatablished, to send and recv data is allowed
         */
        if ((recv_len = recv(established_fd, buffer, 1024, 0)) <= 0) {
            if (recv_len == 0) continue;
            perror("read failed!");
        }
        std::cout << "read(" << recv_len << "): " << buffer << std::endl << std::endl;

        if (send(established_fd, message.c_str(), message.size(), 0) < 0) {
            perror("send failed!");
        }
        usleep(1000);
    }
    /**
     * @brief           6. transfer data has been finished, close it.
     */
    close(established_fd);

    return 0;
}
