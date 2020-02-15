# NAME

libsockfilter - connection filtering for dynamically linked applications

# SYNOPSIS

* server

LD\_PRELOAD=libsockfilter\_accept.so *COMMAND* *ARG* *...*

* client

LD\_PRELOAD=libsockfilter\_connect.so *COMMAND* *ARG* *...*

# DESCRIPTION

An experimental library for adding connection
filtering to any dynamically linked application using
[tcprules](https://cr.yp.to/ucspi-tcp/tcprules.html) rules.

In contrast to managing a firewall, `libsockfilter` allows applications
to enforce network access without requiring any special privileges.

libsockfilter requires [libcdb](https://www.corpit.ru/mjt/tinycdb.html).

# BUILD

    apt install libcdb-dev
    apt install ucspi-tcp-ipv6 # or ucspi-tcp
    make

# ENVIRONMENT VARIABLES

## common

`LIBSOCKFILTER_DEBUG`
: Write errors to stdout (default: disabled).

## libsockfilter\_accept

`LIBSOCKFILTER_ACCEPT`
: Path to rules database. If the rules database is not accessible,
  all connections are dropped.

## libsockfilter\_connect

`LIBSOCKFILTER_CONNECT`
: Path to rules database. If the rules database is not accessible,
  all connections are dropped.

# EXAMPLES

~~~
$ sudo apt install ucspi-tcp-ipv6 # or ucspi-tcp

# default is deny
# 192.168.1.1: deny specific match
# 127: deny any IP beginning with 127
# reset default to allow
cat <<EOF > rules.txt
192.168.1.1:deny
127:deny
:allow
EOF

cat rules.txt | tcprules rules.cdb rules.cdb.tmp

LD_PRELOAD=./libsockfilter_connect.so \
 LIBSOCKFILTER_CONNECT=./rules.cdb \
 nc -vvv 127.0.0.1 22

LD_PRELOAD=./libsockfilter_accept.so \
 LIBSOCKFILTER_ACCEPT=./rules.cdb \
 nc -vvv -k -l 9999
~~~

## ipsum

Using [ipsum](https://github.com/stamparm/ipsum):

~~~
(
curl --compressed https://raw.githubusercontent.com/stamparm/ipsum/master/ipsum.txt 2>/dev/null | awk '/^#/{ next } $2 ~ /^[1-2]$/{next} {print $1 ":deny"}'
echo :allow
) | tcprules rules.cdb rules.cdb.tmp
~~~

# SEE ALSO

_connect_(2), _accept_(2), _tcprules_(1), _hosts.allow_(5), _hosts.deny_(5)
