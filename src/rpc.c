#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "rpc.h"

static int rpc_sock;

int 
sinae2prom_rpc_init()
{
    struct sockaddr_in rpc_addr;

    if((rpc_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return RPC_ERR_CREAT;
    
    rpc_addr.sin_family = AF_INET;
    rpc_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    rpc_addr.sin_port = htons(RPC_SOCKET_PORT);

    int opt = 1;
    if (setsockopt(rpc_sock, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt))) 
        {close(rpc_sock); return RPC_ERR_SETOPT;}

    if(bind(rpc_sock, (struct sockaddr*) &rpc_addr, sizeof(rpc_addr)) < 0)
        {close(rpc_sock);return RPC_ERR_BIND;}
    
    if(listen(rpc_sock, 10) < 0)
        {close(rpc_sock); return RPC_ERR_LISTEN;}
    
    return rpc_sock;
}

int
sinae2prom_rpc_handler(const int dev_fd)
{
    int cli_sock;
    struct sockaddr_in cli_addr;
    int cli_addr_len = sizeof(cli_addr);
    char req[sizeof(struct RPC_req_s)];
    struct RPC_req_s *req_s = alloca(sizeof(*req_s));
    char read_buf[E2PROM_MAX_READ_BYTE_SIZE];

    if(req_s == 0)
        return RPC_ERR_NULL;

    if((cli_sock = accept(rpc_sock,(struct sockaddr*) &cli_addr, &cli_addr_len)) < 0)
        return RPC_ERR_ACCEPT;

    while(1)
    {
        if(read(cli_sock, req, sizeof(req)) < 0)
            return RPC_ERR_RECV;

         req_s = (struct RPC_req_s*) req;

        switch(req_s->r_hdr)
        {
            int st;
            case RPC_REQ_R_HDR:
                if((st = sinae2prom_e2prom_read(dev_fd, req_s->r_addr, read_buf, req_s->r_pld.r_r_pld)) != I2c_ERR_NOERR)
                    return RPC_ERR_INTERNAL;

                if(send(cli_sock, (void*) req, sizeof(*req), 0) < 0)
                    return RPC_ERR_SEND;

                break;
            
            case RPC_REQ_W_HDR:
                if((st = sinae2prom_e2prom_write(dev_fd, req_s->r_addr, req_s->r_pld.r_w_pld, strlen(req_s->r_pld.r_w_pld))) != I2c_ERR_NOERR)
                    return RPC_ERR_INTERNAL;
                break;
            
            default: break;
        }
    }
   
    return RPC_ERR_NOERR;
}