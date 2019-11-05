/* Copyright (c) 2019, Michael Santos <michael.santos@gmail.com>
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
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cdb.h>

#include "sockcmp.h"

int sockcmp(const char *rules, int debug, const struct sockaddr *addr,
            socklen_t addrlen) {
  char buf[INET6_ADDRSTRLEN] = {0};
  char *p;

  int fd;
  struct cdb c;

  int rv = -1;

  if (rules == NULL) {
    (void)fprintf(stderr, "libsockfilter:rules is NULL!\n");
    return -1;
  }

  fd = open(rules, O_RDONLY);
  if (fd < 0) {
    if (debug)
      (void)fprintf(stderr, "libsockfilter:%s:%s\n", rules, strerror(errno));
    return -1;
  }

  if (cdb_init(&c, fd) != 0) {
    if (debug)
      (void)fprintf(stderr, "libsockfilter:unable to open:%s\n", rules);
    (void)close(fd);
    return -1;
  }

  switch (addr->sa_family) {
  case AF_INET:
    if (addrlen < sizeof(struct sockaddr_in))
      goto LIBSOCKFILTER_DONE;

    (void)inet_ntop(AF_INET, &(((const struct sockaddr_in *)addr)->sin_addr),
                    buf, INET6_ADDRSTRLEN);

    if (debug)
      (void)fprintf(stderr, "libsockfilter:%s:AF_INET:%s:%u\n", rules, buf,
                    ntohs(((const struct sockaddr_in *)addr)->sin_port));
    break;

  case AF_INET6:
    if (addrlen < sizeof(struct sockaddr_in6))
      goto LIBSOCKFILTER_DONE;

    (void)inet_ntop(AF_INET6, &(((const struct sockaddr_in6 *)addr)->sin6_addr),
                    buf, INET6_ADDRSTRLEN);

    if (debug)
      (void)fprintf(stderr, "libsockfilter:%s:AF_INET6:%s:%u\n", rules, buf,
                    ntohs(((const struct sockaddr_in *)addr)->sin_port));
    break;

  default:
    goto LIBSOCKFILTER_DONE;
    break;
  }

  for (;;) {
    if (debug)
      (void)fprintf(stderr, "match=\"%s\"\n", buf);

    if (cdb_find(&c, buf, (unsigned int)strlen(buf)) > 0) {
      char data[1] = {0};
      if (cdb_read(&c, data, sizeof(data), cdb_datapos(&c)) < 0)
        _exit(111);
      switch (data[0]) {
      case 'D':
        if (debug)
          (void)fprintf(stderr, "libsockfilter:%s:blocked\n", buf);
        rv = -1;
        break;
      default:
        if (debug)
          (void)fprintf(stderr, "libsockfilter:%s:allowed\n", buf);
        rv = 0;
        break;
      }
      goto LIBSOCKFILTER_DONE;
    }

    if (buf[0] == '\0')
      break;

    /* XXX IPv4 only: 127.1.2.3, 127.1.2, 127.1, 127 */
    p = strrchr(buf, '.');
    if (p == NULL)
      buf[0] = '\0';
    else
      *p = '\0';
  }

LIBSOCKFILTER_DONE:
  cdb_free(&c);
  (void)close(fd);

  return rv;
}
