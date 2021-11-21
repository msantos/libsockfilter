/* Copyright (c) 2019-2021, Michael Santos <michael.santos@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "libsockfilter.h"

void _init(void);
static int (*sys_connect)(int sockfd, const struct sockaddr *addr,
                          socklen_t addrlen);
#pragma GCC diagnostic ignored "-Wpedantic"
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
#pragma GCC diagnostic warning "-Wpedantic"

static char *env_connect = NULL;
static int opt = 0;

void _init(void) {
  const char *err;
  char *debug;

  env_connect = getenv("LIBSOCKFILTER_CONNECT");
  debug = getenv("LIBSOCKFILTER_DEBUG");

  if (debug)
    opt |= LIBSOCKFILTER_DEBUG;

#pragma GCC diagnostic ignored "-Wpedantic"
  sys_connect = dlsym(RTLD_NEXT, "connect");
#pragma GCC diagnostic warning "-Wpedantic"
  err = dlerror();

  if (err != NULL) {
    (void)fprintf(stderr, "libsockfilter:dlsym (connect):%s\n", err);
  }
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  if ((env_connect != NULL) && sockcmp(env_connect, opt, addr, addrlen) < 0) {
    errno = EPERM;
    return -1;
  }

  return sys_connect(sockfd, addr, addrlen);
}
