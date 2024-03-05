#ifndef SINAE2PROM_RPC_H
#define SINAE2PROM_RPC_H

#include "i2c.h"

#define RPC_SOCKET_PORT         3036
#define RPC_SOCKET_IP           "192.168.8.7"

#define RPC_MAX_REQ_PLD_LEN     E2PROM_MAX_WRITE_BUF_SIZE

enum 
{
    RPC_REQ_W_HDR,
    RPC_REQ_R_HDR
};

struct RPC_req_s
{
    int     r_hdr : 4;
    int     r_addr;
    union
    {
        char    r_w_pld[E2PROM_MAX_WRITE_BUF_SIZE];
        int     r_r_pld;
    } r_pld ;
} __attribute__((__packed__));

enum 
{
    RPC_ERR_NOERR   =    1,
    RPC_ERR_CREAT   =   -1,
    RPC_ERR_BIND    =   -2,
    RPC_ERR_RECV    =   -3,
    RPC_ERR_SEND    =   -4,
    RPC_ERR_NULL    =   -5,
    RPC_ERR_INTERNAL=   -6,
    RPC_ERR_SETOPT  =   -7,
    RPC_ERR_LISTEN  =   -8,
    RPC_ERR_ACCEPT  =   -9
};

int                 sinae2prom_rpc_init();
int                 sinae2prom_rpc_handler(const int);

#endif // SINAE2PROM_RPC_H