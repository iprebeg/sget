#include <wrap.h>

int Socket(int domain, int type, int protocol)
{
  int s;
  if ((s = socket(domain, type, protocol)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  return s;
}

int Bind(int sockfd, const struct sockaddr *addr,
         socklen_t addrlen)
{
  int err;
  if ((err = bind(sockfd, addr, addrlen)) < 0) {
    perror ("bind");
    exit(EXIT_FAILURE);
  }
  return err;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
  int s;
  if ((s = accept(sockfd, addr, addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return s;
}


pid_t Fork(void)
{
  pid_t pid;
  if ( (pid = fork()) < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  return pid;
}


int Getnameinfo(const struct sockaddr *sa, socklen_t salen,
                char *host, size_t hostlen,
                char *serv, size_t servlen, int flags)
{
  int err;

  if ((err = getnameinfo(sa, salen, host, hostlen, serv, servlen, flags)) != 0) {
    gai_strerror(err);
  }
  return err;
}

int Listen(int sockfd, int backlog)
{
  int err;
  if ((err = listen(sockfd, backlog)) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  return err;
}

int Sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
  int err;
  if ((err = sigaction(signum, act, oldact)) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  return err;
}

int Chdir(const char *path)
{
  int err;
  if ((err = chdir(path)) < 0) {
    perror("chdir");
    exit(EXIT_FAILURE);
  }
  return err;
}
