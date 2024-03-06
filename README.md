# Interact With E2prom; Read And Write Operations

The main program is the one which will be uploaded on the board. it runs a semi-daemon to serve request, execute requested program and send back the result; similar to RPC. 

The cli program runs a telnet based cli on the client machine; which you can send your requests to rpc using this cli. it is on the other machine and not the board because our board is an embedded device and has limmited resources, it only have to do necessary and main works like writing and reading to e2prom.

### Main Program Notes

**The most important thing is that because main will be used on the board, it have to be compiled due to the board's specifications. therefore you must edit Makefile before compilation.**

Change `dev` struct in the main.c as your bus number and device number.

This program takes the ip address and port number of board statically; you must edit `RPC_SOCKET_IP` and `RPC_SOCKET_PORT` in rpc.h manually. 

### Cli Program Notes 

Change `CLI_PORT` if you want; it is the port number you have to use to telnet.

To telnet run command `telnet <ip-address-cli-is-running> <CLI_PORT>`; if cli is on the localhost: `telnet localhost <CLI_PORT>`.

Cli commands are:  `write <addr> <theTextYouWantToWrite>` to write to e2prom and `read <addr> <hoeManyByteYouWantToRead>` to read from e2prom.

In addition you can change `CLI_HOSTNAME` and `CLI_BANNER` in cli.h 

## How To Make?

After editing makefile, use `make main` to build main program and `make cli` to build cli program. use `make clean` to clean unused files.