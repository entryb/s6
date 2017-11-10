/* ISC license. */

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <skalibs/djbunix.h>
#include <skalibs/unix-transactional.h>
#include <s6/s6-supervise.h>

int s6_svc_writectl (char const *service, char const *subdir, char const *s, size_t len)
{
  size_t svlen = strlen(service) ;
  size_t sublen = strlen(subdir) ;
  int r ;
  char fn[svlen + sublen + 10] ;
  memcpy(fn, service, svlen) ;
  fn[svlen] = '/' ;
  memcpy(fn + svlen + 1, subdir, sublen) ;
  memcpy(fn + svlen + 1 + sublen, "/control", 9) ;
  r = s6_svc_write(fn, s, len) ;
  if (r != -2) return r ;

  /* Now we need to investigate what went wrong... */
  {
    int fd, fdsub ;
    fd = open(service, O_RDONLY | O_DIRECTORY) ;
    if (fd < 0) return -1 ;
    fdsub = open2_at(fd, subdir, O_RDONLY | O_DIRECTORY) ;
    fd_close(fd) ;
    if (fdsub < 0) return (errno == ENOENT) ? 0 : -2 ;
    fd_close(fdsub) ;
    return -2 ;
  }
}
