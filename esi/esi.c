#include "socketClient.h"

int main(){

	configure_logger();

	int planificador = create_client("127.0.0.1","12345");

	int coordinador = create_client("127.0.0.1","12346");

	send_message(planificador);

	exit_gracefully(0);
}
