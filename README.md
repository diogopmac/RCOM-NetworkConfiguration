# Network Configuration Laboratory Assignment 
## Redes de Computadores - 2025/2026

This project was made in the ambit of the Course Unit ”Redes de Computadores” at the Faculty of Engineering of University of Porto (FEUP), and it consists of a study about the computer network existent on the netlab Laboratory at FEUP, as well as a Download Application for that network, using the FTP download protocol.

# Folder Breakdown
## [Download code](src)
This folder contains the source code for the download application developed in the first part of the assignment. 

### To run the application:
In your terminal, run in the root:
```bash
make
```
then navigate to the `bin` folder. 
```bash
cd bin
```
Then run:
```bash
./download <link>
```
No further inputs are necessary.

## [Download code documentation](docs)
This folder has 2 Markdown files containing the [instructions](docs/APP.md) provided by the professors to develop the download application, and the [servers](docs/SERVERS.md) to test the application.

## [Download examples](examples)
In this file can be found the code examples provided by the professors to [get the ip address](examples/getip.c) of a certain server and a simple code to [establish a TCP connection](examples/clientTCP.c) to that server.

## [Experience Logs](logs)
Here can be found the instructions to perform each one of the experiences in the Laboratory, every command performed and the logs and results obtained.
> Important note: Some experiences and commands may be `incremental`. Some commands are not performed or shown twice since the previous experience has already done them. E.g. in experience 5 the network configuration is not documented because it is the same as experience 4.

## [Complete Report](RCOM_Report_2.pdf)
Complete report for this assignment. 