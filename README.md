# Simple HTTP Web Server

A simple HTTP GET web server with a test website.
The web server treats "current_directory/www" as root.

# Usage

```bash
make
./web_server <IP> <PORT>
```

Where <IP> is the IP address we are listening at and <PORT> is number like: 9999
Using NULL for the IP address binds to localhost.



# Test

While web_server is running. Open a browser and go to http://<IP>:<PORT>/ (Where <IP> is the IP address we are listening and <PORT> is number like: 9999)

Or, you can call test.sh on the same IP address port.

```bash
./test.sh <IP> <PORT>
```

The above mentioned shell script is meant as a break test.
It preforms the following command 2200 times.

```bash
curl http://localhost:9999/ > index.html
```

# Architecture

A simple thread-per-connection pattern is used.
Connection threads are implemented in a synchronous
fashion.
