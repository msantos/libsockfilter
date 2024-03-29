/* Copyright (c) 2019-2023, Michael Santos <michael.santos@gmail.com>
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
static int (*sys_accept)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
static int (*sys_accept4)(int sockfd, struct sockaddr *addr, socklen_t *addrlen,
                          int flags);
#pragma GCC diagnostic ignored "-Wpedantic"
int __attribute__((visibility("default")))
accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int __attribute__((visibility("default")))
accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
#pragma GCC diagnostic warning "-Wpedantic"

static char *env_accept = NULL;
static int opt = 0;

void _init(void) {
  const char *err;
  char *debug;

  env_accept = getenv("LIBSOCKFILTER_ACCEPT");
  debug = getenv("LIBSOCKFILTER_DEBUG");

  if (debug)
    opt |= LIBSOCKFILTER_DEBUG;

#pragma GCC diagnostic ignored "-Wpedantic"
  sys_accept = dlsym(RTLD_NEXT, "accept");
  sys_accept4 = dlsym(RTLD_NEXT, "accept4");
#pragma GCC diagnostic warning "-Wpedantic"
  err = dlerror();

  if (err != NULL) {
    (void)fprintf(stderr, "libsockfilter:dlsym (accept):%s\n", err);
  }
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int fd;

  fd = sys_accept(sockfd, addr, addrlen);
  if (fd < 0)
    return fd;

  if (env_accept == NULL)
    return fd;

  if (sockcmp(env_accept, opt, addr, *addrlen) < 0) {
    (void)close(fd);
    errno = EPERM;
    return -1;
  }

  return fd;
}

int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
  int fd;

  fd = sys_accept4(sockfd, addr, addrlen, flags);
  if (fd < 0)
    return fd;

  if (env_accept == NULL)
    return fd;

  if (sockcmp(env_accept, opt, addr, *addrlen) < 0) {
    (void)close(fd);
    errno = EPERM;
    return -1;
  }

  return fd;
}
