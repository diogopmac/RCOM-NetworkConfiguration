To test the FTP download application, you can use one of the following FTP servers:

# NetLab FTP Server
## The NetLab FTP server is available at: ftp://netlab1.fe.up.pt

The server can be accessed in anonymous mode and with user-password authentication.

The credentials for the authenticated access mode are:

- User: rcom
- Pass: rcom

**The two access modes list different files.** This allows testing each mode individually and checking if the access mode is correct, since both modes will produce different results.

**NOTE:** The server can only be accessed within the laboratory (I322) network and only when the bench is fully configured according to experiment 6.

# Files Hosted at the NetLab FTP Server
## The following files can be used to test the FTP download application:

- pipe.txt
- /files/crab.mp4
- /debian/ls-lR.gz (about 15 MB)
- /pub/parrot/iso/testing/Parrot-architect-5.3_amd64.iso (about 386 MB)

# Public FTP Servers
## You can also test the FTP download application with public FTP servers available in the Internet. The following list contains examples of URLs that you can use to test both the anonymous mode and user-password authentication:

- URL 1: ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz
- URL 2: ftp://demo:password@test.rebex.net/readme.txt
- URL 3: ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin

# In Case of Problems with DNS Name Resolution
## If there are problems related to DNS name resolution, try changing the DNS servers to one of the following ones (instead of the NetLab's DNS server):

- 193.136.28.10
- 193.136.28.9
- 8.8.8.8