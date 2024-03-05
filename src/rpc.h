#ifndef SINAE2PROM_RPC_H
#define SINAE2PROM_RPC_H

#include "i2c.h"

#define RPC_SOCKET_PORT         3036
#define RPC_SOCKET_IP           "192.168.8.7"

#define RPC_MAX_REQ_PLD_LEN     E2PROM_MAX_WRITE_BUF_SIZE

#define RPC_REQ_VER_CODE        123

enum 
{
    RPC_REQ_W_HDR,
    RPC_REQ_R_HDR
};

struct RPC_req_s
{
    uint8_t     r_ver; 
    uint16_t    r_hdr;
    uint32_t    r_addr;
    union
    {
        uint8_t    r_w_pld[E2PROM_MAX_WRITE_BUF_SIZE];
        uint8_t    r_r_pld;
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
    RPC_ERR_ACCEPT  =   -9,
    RPC_ERR_VER_REQ =   -10,
    RPC_ERR_NOT_INIT=   -11
};

int                 sinae2prom_rpc_init(const int);
int                 sinae2prom_rpc_handler();
int                 sinae2prom_rpc_pack_req(struct RPC_req_s* ,uint16_t, uint32_t, uint8_t*, uint8_t);

#endif // SINAE2PROM_RPC_H