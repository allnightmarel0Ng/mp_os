#include <tcp_storage_server.h>

int main()
{   
    tcp_storage_server server(34543);
    server.run();
    return 0;
}