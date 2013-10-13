#include <wrap.h>
#include <http.h>

static void usage(char * binary)
{
  fprintf(stderr, "usage: $ %s directory\n", binary);
  exit(EXIT_FAILURE);
}

static void sigchld_handler(int s)
{
  while(waitpid(-1,NULL,WNOHANG) > 0);
}

int 
main (int argc, char *argv[])
{
  if (argc != 2)
    usage(argv[0]);

  Chdir(argv[1]);
  
  uid_t uid = getuid();
  short int port = 20000 + (int)uid;

  int serv_sock;
  struct sockaddr_in serv_addr;

  serv_sock = Socket(PF_INET, SOCK_STREAM, 0);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  bzero(&serv_addr.sin_zero, sizeof(serv_addr.sin_zero));

  Bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  printf ("binded to port %d\n", port);

  Listen(serv_sock, 10);

  // Kill little motherfuckers
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  Sigaction(SIGCHLD, &sa, NULL);

  // Main loop
  struct sockaddr_in cli_addr;
  socklen_t cli_len = sizeof(cli_addr);
  int cli_sock;
  pid_t pid;
  char hbuf[256];
  char sbuf[256];

  for(;;) {
    cli_sock = Accept(serv_sock, (struct sockaddr*)&cli_addr, &cli_len);
    
    if ((pid = Fork()) == 0) {
    /* Child */
      close(serv_sock);
      handle_client(cli_sock);
      close(cli_sock);
      exit(EXIT_SUCCESS);
    }
    else {
    /* Parent */
      Getnameinfo((struct sockaddr*)&cli_addr, cli_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), 0);
      printf ("Connection from %s:%s - PID %d\n", hbuf, sbuf, pid);
      close(cli_sock);
    }
  }
  return 0;
}
