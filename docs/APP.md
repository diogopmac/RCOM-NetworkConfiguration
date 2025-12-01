# Develop application download ftp://ftp.netlab.fe.up.pt/pub/...
- Application downloads a single file
- Implements FTP application protocol, as described in [RFC959](https://datatracker.ietf.org/doc/html/rfc959)
- Adopts URL syntax, as described in [RFC1738](https://www.rfc-editor.org/rfc/rfc1738.html)
`ftp://[<user>:<password>@]<host>/<url-path>`
# Steps
- Experiments using Telnet application (Telnet, SMTP, POP, HTTP and FTP); focus on FTP
- Specification/design of a download application
    - unique use case: connect, login host, passive, get path, success (file saved in CWD) or un-success (indicating failing phase)
    - challenging programming aspects: gethostbyname, sockets, control connection, passive, data connection
- Implement a very simple FTP client at home
    - reuse existing programs: clientTCP.c, getIP.c
# Learning objectives
- Describe client - server concept and its peculiarities in TCP/IP
- Characterize application protocols in general, characterize URL, describe in detail the behaviour of FTP
- Locate and read RFCs
- Implement a simple FTP client in C language
- Use sockets and TCP in C language
- Understand service provided DNS and use it within a client program

# API - Application Programming Interface
- operating system: UNIX
- programming language: C
- communication protocols
    - TCP/IP
    - UNIX
    - XNS
- Address data structures
- Primitives: socket(), bind(), connect(), listen(), accept(), recvfrom(), sendto(), close()
- Association - socket pair