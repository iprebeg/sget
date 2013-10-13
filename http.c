#include <http.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

void handle_client(int sock)
{
  char buf[1024];
  char * path;
  char * http_resp;
  int fd;
  struct stat stat;

  bzero(buf, sizeof buf);
  read(sock, buf, sizeof buf);

  http_resp = malloc(4098);
  bzero(http_resp, 4098);
  path = preprocess_http_req(buf);
  if (path == NULL) {
    char error_content[] = "<head>\n<title>YOU DIDZ BADZ</title>\n</head>\n<body>\n<h1>YOU ARE BAD</h1></body>\n";
    sprintf (http_resp, "HTTP/1.0 404 YOU DID BADZ\r\nServer: iprebeg's sget/0.1\r\nContent-type: text/html\r\nContent-Length: %d\r\n\r\n", 
      (int)strlen(error_content));
    strcat(http_resp + strlen(http_resp), error_content);
  } else if (strcmp(path,"/") == 0) {
    char * dir_listing = return_dir_listing();
    sprintf (http_resp, "HTTP/1.0 200 OK\r\nServer: iprebeg's sget/0.1\r\nContent-type: text/html\r\nContent-Length: %d\r\n\r\n", 
    (int)strlen(dir_listing));
    strcat(http_resp + strlen(http_resp), dir_listing);
    write(sock, http_resp, strlen(http_resp));
    free(dir_listing);
  } else if((fd = open(path+1, O_RDONLY)) > 0) {
    fstat (fd, &stat);
    printf ("sending file: %d bytes\n", (int)stat.st_size);
    sprintf (http_resp, "HTTP/1.0 200 OK\r\nServer: iprebeg's sget/0.1\r\nContent-Length: %d\r\n\r\n", (int)stat.st_size);
    write(sock, http_resp, strlen(http_resp));
    sendfile (sock, fd, NULL, stat.st_size);
  }
  free(http_resp);
}

char * preprocess_http_req(char * buf)
{ 
  char * tok;
  char * path;

  tok = strtok(buf, " "); 
  if (strcmp(tok, "GET") != 0) {
    printf ("YOUR REQUEST IS CRAP, I WANTZ ONLY GETS\n");
    return NULL;
  }

  path = strtok(NULL, " "); 
  if (strstr (path,"..") != 0) {
    printf ("CANTZ DO BEAKWORDZ BITCHZ!\n");
    return NULL;
  }

  if (strstr(path,"/") == 0) {
    printf ("ROOTZ NO ACCESSZ!\n");
  } 

  tok = strtok(NULL, "\r"); 
  /*
  if (strcmp(tok, "HTTP/1.1") != 0) {
    printf ("YOUR VERSION IS NO COMPATZ:P\n");
    return NULL;
  }
  */

  return path;
}

char * return_dir_listing()
{
  int buf_size = 1024;
  int buf_pos = 0;
  char * http_content;

  http_content = malloc(buf_size);
  bzero(http_content, buf_size);

  sprintf(http_content + buf_pos, "<title>Directory listing for /</title>\n");
  buf_pos = strlen(http_content);

  sprintf(http_content + buf_pos, "<h2>Directory listing for /</h2>\n<hr>\n<ul>\n");
  buf_pos = strlen(http_content);

  DIR *dp;
  struct dirent *ep;
  dp = opendir ("./");
  if(dp != NULL) {
    while ((ep = readdir(dp)) != NULL) {
      if ((strcmp(ep->d_name, ".") == 0) ||
          (strcmp(ep->d_name, "..") == 0))
          continue;
      sprintf(http_content + buf_pos, "<li><a href=\"%s\">%s</a\n", ep->d_name, ep->d_name);
      buf_pos = strlen(http_content);

      if (buf_pos > ((3./4.)*buf_size)) {
        http_content = realloc(http_content, buf_size << 1);
        if (http_content != NULL)
          buf_size <<= 1;
        else {
          perror("realloc");
        }
      }
    }
  }
  else {
    perror ("Couldn't open dir\n");
  }

  sprintf(http_content + buf_pos, "</ul>\n<hr>\n");
  buf_pos = strlen(http_content);

  return http_content;
}
