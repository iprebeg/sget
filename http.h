#include <wrap.h>

void handle_client(int sock);
char * preprocess_http_req(char * buf);
char * return_dir_listing();
