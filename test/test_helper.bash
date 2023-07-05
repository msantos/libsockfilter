#!/bin/bash

cat<<EOF>rules.txt
192.168.1.1:deny
127:deny
::1:deny
:allow
EOF

cat rules.txt | tcprules rules.cdb rules.cdb.tmp
