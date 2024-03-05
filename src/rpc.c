#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

#include "rpc.h"

static int rpc_sock;
static int dev_fd;
static short is_rpc_init;

int 
sinae2prom_rpc_init(const int device_fd)
{
    dev_fd = device_fd;
    struct sockaddr_in rpc_addr;

    if((rpc_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return RPC_ERR_CREAT;
    
    rpc_addr.sin_family = AF_INET;
    rpc_addr.sin_addr.s_addr = htons(INADDR_ANY);
    rpc_addr.sin_port = htons(RPC_SOCKET_PORT);

    int opt = 1;
    if (setsockopt(rpc_sock, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt))) 
        {close(rpc_sock); return RPC_ERR_SETOPT;}

    if(bind(rpc_sock, (struct sockaddr*) &rpc_addr, sizeof(rpc_addr)) < 0)
        {close(rpc_sock);return RPC_ERR_BIND;}
    
    if(listen(rpc_sock, 100) < 0)
        {close(rpc_sock); return RPC_ERR_LISTEN;}

    is_rpc_init = 1;
    printf("[rpc_init] Rpc Socket Created Successfully\n");
    
    return rpc_sock;
}

static int 
sinae2prom_rpc_con_handler(const int cli_sock)
{
    char req[sizeof(struct RPC_req_s)];
    struct RPC_req_s *req_s = alloca(sizeof(*req)); 
    char read_buf[E2PROM_MAX_READ_BYTE_SIZE];
    int st;

    while(1)
    {
        if(read(cli_sock, req, sizeof(req)) == 0)
            return RPC_ERR_RECV;

        req_s = (struct RPC_req_s*) req;

        if(req_s->r_ver != RPC_REQ_VER_CODE)
            continue;

        uint16_t req_hdr = req_s->r_hdr;
        uint32_t req_addr = req_s->r_addr;
        uint8_t req_r_pld = req_s->r_pld.r_r_pld;
        uint8_t req_w_pld[E2PROM_MAX_WRITE_BUF_SIZE];
        strcpy(req_w_pld, req_s->r_pld.r_w_pld);
        uint8_t req_pld[RPC_MAX_REQ_PLD_LEN ];
        if(req_hdr==RPC_REQ_R_HDR)
            sprintf(req_pld, "%d", req_r_pld);
        else if(req_pld==RPC_REQ_W_HDR)
            sprintf(req_pld, "%s", req_w_pld);

        printf("[rpc_con_handler] Received Request From Connection (%d)\n", cli_sock);
        printf("[rpc_con_handler] \treq_hdr : %d, req_addr : %d, req_pld : %s\n", req_hdr, req_addr, req_pld);

        switch(req_hdr)
        {
            case RPC_REQ_R_HDR:
                if((st = sinae2prom_e2prom_read(dev_fd, req_addr, read_buf, (size_t) req_r_pld)) != I2c_ERR_NOERR)
                    return RPC_ERR_INTERNAL;
                printf("[rpc_con_handler] \tRead E2prom Request Done\n");

                if(send(cli_sock, (void*) read_buf, sizeof(*read_buf), 0) < 0)
                    return RPC_ERR_SEND;
                printf("[rpc_con_handler] Response Sent To Connection (%d)\n", cli_sock);

                break;
            
            case RPC_REQ_W_HDR:
                if((st = sinae2prom_e2prom_write(dev_fd, req_addr, req_w_pld, strlen(req_w_pld))) != I2c_ERR_NOERR)
                    return RPC_ERR_INTERNAL;
                printf("[rpc_con_handler] \tWrite E2prom Request Done\n");

                break;
            
            default: break;
        }
    }
}

int
sinae2prom_rpc_handler()
{
    if(is_rpc_init != 1)
        return RPC_ERR_NOT_INIT;

    int cli_sock;
    struct sockaddr_in cli_addr;
    int cli_addr_len = sizeof(cli_addr);

    printf("[rpc_handler] Listening To Accept Connection...\n");
    if((cli_sock = accept(rpc_sock,(struct sockaddr*) &cli_addr, &cli_addr_len)) < 0)
        return RPC_ERR_ACCEPT;
    printf("[rpc_handler] New Connection Accepted (%d)\n", cli_sock);
    
    switch(fork())
    {
        case -1:
            printf("[rpc_handler] Failed To Fork For Connection (%d)\n", cli_sock);
            break;
        case 0:
            if(sinae2prom_rpc_con_handler(cli_sock) < 0)
            {
                printf("[rpc_handler] Terminating Connection (%d)...\n", cli_sock);
                exit(EXIT_SUCCESS);
            }
            break;
        
        default:
            break;
    }
   
    return RPC_ERR_NOERR;
}


int 
sinae2prom_rpc_pack_req(struct RPC_req_s *req ,uint16_t req_hdr, uint32_t req_addr, uint8_t *req_w_pld, uint8_t req_r_pld)
{
    if(req == 0 || (req_hdr == RPC_REQ_W_HDR && req_w_pld == 0))
        return RPC_ERR_NULL;

    req->r_ver = RPC_REQ_VER_CODE;          /* RPC_REQ_VER_CODE is 8 bit thus no need to change endian */
    req->r_hdr = htons(req_hdr);
    req->r_addr = htonl(req_addr);
    if(req_hdr == RPC_REQ_R_HDR)
        req->r_pld.r_r_pld = req_r_pld;     /* req_r_pld is 8 bit thus no need to change endian */
    else if(req_hdr == RPC_REQ_W_HDR)
        strncpy(req->r_pld.r_w_pld, req_w_pld, RPC_MAX_REQ_PLD_LEN);

    return RPC_ERR_NOERR;
}