#include <libcli.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "rpc.h"

#define CLI_HOSTNAME    "sinae2prom"
#define CLI_BANNER      "write/read to/from e2prom"
#define CLI_PORT        8080        

static int cli_socket;
static int rpc_socket;
static int dev_fd;

static int 
cli_write_handler(struct cli_def *cli, const char *command, char *argv[], int args)
{
    if(args != 2 || ((args == 1 && strcmp(argv[0], "--help") == 0)))
        {cli_print(cli,"Usage: write <addr> <dataToWrite>"); return CLI_ERROR;} 

    struct RPC_req_s req;  
    if((strcmp(argv[0], "0") != 0) && atol(argv[0]) == 0)
        {cli_print(cli,"invalid addr! it must be numeric and not negative"); return CLI_ERROR;}

    sinae2prom_rpc_pack_req(&req, RPC_REQ_W_HDR, atol(argv[0]), argv[1], 0);

    if(send(rpc_socket,(void*) &req, sizeof(req), 0) < 0)
        {printf("Unable to send Request\n"); return CLI_ERROR;}
    printf("[cli_write_cmnd] Request Sent To Server\n");
    
    cli_print(cli, "Wrote %s To E2prom in address 0x%lX", argv[1], atol(argv[0]));
    
    return CLI_OK;
}

static int 
cli_read_handler(struct cli_def *cli, const char *command, char *argv[], int args)
{
    if(args != 2 || ((args == 1 && strcmp(argv[0], "--help") == 0)))
        {cli_print(cli,"Usage: read <addr> <byteLen>\n"); return CLI_ERROR;} 

    if((strcmp(argv[0], "0") != 0) && atol(argv[0]) == 0)
        {cli_print(cli,"invalid addr! it must be numeric and not negative"); return CLI_ERROR;}

    if(atoi(argv[1]) == 0)
        {cli_print(cli,"invalid byte len! it must be numeric and positive"); return CLI_ERROR;}

    struct RPC_req_s req;  
    uint8_t buf[atoi(argv[1])+2];

    sinae2prom_rpc_pack_req(&req, RPC_REQ_R_HDR, atol(argv[0]), NULL, atoi(argv[1]));

    if(send(rpc_socket,(void*) &req, sizeof(req), 0) < 0)
        {printf("unable to send request"); return CLI_ERROR;}
    printf("[cli_read_cmnd] Request Sent To Server\n");

    if(recv(rpc_socket, buf, atoi(argv[1]), 0) > 0)
        cli_print(cli, "Read %s Byte From E2prom -> %s", argv[1], buf);

    return CLI_OK;     
}

struct cli_def* 
sinae2prom_cli_init()
{
    struct cli_def *cli = cli_init();
    struct cli_command *write_cmnd, *read_cmnd;

    cli_set_hostname(cli, CLI_HOSTNAME);
    cli_set_banner(cli, CLI_BANNER);

    write_cmnd = cli_register_command(cli, NULL, "write", cli_write_handler, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "write to e2prom");
    read_cmnd = cli_register_command(cli, NULL, "read", cli_read_handler, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "read from e2prom");

    printf("[cli_init] Interface Init Successfull\n");

    struct sockaddr_in saddr;
    int on = 1;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htons(INADDR_ANY);
    saddr.sin_port = htons(CLI_PORT);

    if((cli_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {printf("[cli_init] Failed Init Cli Socket\n"); return NULL;}
    printf("[cli_init] Socket Init Successfull\n");

    if(setsockopt(cli_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
        {printf("[cli_init] Failed To Set Reuse Addr To Socket\n"); return NULL;}
    printf("[cli_init] Socket Set Option Successfull\n");

    if(bind(cli_socket ,(struct sockaddr*) &saddr, sizeof(saddr)) < 0) 
        {printf("[cli_init] Failed To Bind Socket\n"); return NULL;}
    printf("[cli_init] Socket Bind Successfull\n");

    if(listen(cli_socket , 100) < 0)
        {printf("[cli_init] Failed To Listen Socket\n"); return NULL;}
    printf("[cli_init] Listen Successfull\n");

    struct sockaddr_in server_addr;
    rpc_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(rpc_socket < 0)
        {printf("[cli_init] Error while creating socket\n"); return NULL;}

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(RPC_SOCKET_PORT);
    server_addr.sin_addr.s_addr = inet_addr(RPC_SOCKET_IP);

    if(connect(rpc_socket,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        {printf("[cli_init] Unable to connect to server\n"); return NULL;}
    printf("[cli_init] Connected To Server Successfully\n");

    return cli;
}

int
main()
{
    struct cli_def *cli;
    if((cli=sinae2prom_cli_init()) == NULL)
    {
        printf("Failed To Init Cli\n");
        return -1;
    } 
    
    printf("[cli] Listening To Accept Connection...\n");
    int con;
    while((con = accept(cli_socket, NULL, 0)))
    {
        printf("[cli] New Connection Accepted\n");
        cli_loop(cli, con);
        close(con);
        printf("[cli] Connection Terminated\n");
    }

    cli_done(cli);
    return 0;
}