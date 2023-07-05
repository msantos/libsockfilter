#!/usr/bin/env bats

load test_helper

@test "connect(2): filtered destination" {
  run env LD_PRELOAD=./libsockfilter_connect.so LIBSOCKFILTER_CONNECT=./rules.cdb nc -w1 -v 127.0.0.1 9797
  expect="nc: connect to 127.0.0.1 port 9797 (tcp) failed: Operation not permitted"
  cat <<EOF
|$output|
---
|$expect|
EOF

  [ "$status" -ne 0 ]
  [ "$output" = "$expect" ]
}

@test "accept(2): filtered connection" {
  sh -c "(sleep 2; nc -z 127.0.0.1 9798) &"
  run env LD_PRELOAD=./libsockfilter_accept.so LIBSOCKFILTER_ACCEPT=./rules.cdb nc -l 127.0.0.1 9798
  expect="nc: accept: Operation not permitted"
  cat <<EOF
|$output|
---
|$expect|
EOF

  [ "$status" -ne 0 ]
  [ "$output" = "$expect" ]
}
