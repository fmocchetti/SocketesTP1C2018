#include "socketClient.h"

int main(){

	configure_logger();

	int server = create_client("127.0.0.1","12345");

	send_message(server);

	exit_gracefully(0);
}
