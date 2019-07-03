# DNS client

Basic DNS client base on BSD sockets. This program simualtes behaviour of **nslookup** utility. The program is written in C++ and uses libraries from Windows. 

### Examples
The first parameter is the name of address and the second parameter is not mandatory and you can specify the ip address of dns server.
```sh
$ dnsclient www.seznam.cz 8.8.8.8
Server:	8.8.8.8

Non-authoritative answer:
Name:	www.seznam.cz
Addr:	77.75.75.172
Name:	www.seznam.cz
Addr:	77.75.75.176
```

IP address of dns server is taken from the operating system. Also is added the support for a name in IDN format.

```sh
$ dnsclient www.háčkyčárky.cz
Server:	194.228.41.65

Non-authoritative answer:
www.xn--hkyrky-ptac70bc.cz	canonical name = xn--hkyrky-ptac70bc.cz
Name:	xn--hkyrky-ptac70bc.cz
Addr:	217.31.205.51
```