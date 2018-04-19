#include "socketServer.h"

int main (int argc, char *argv[])
{
	createServer(32, 3 * 60 * 1000);
	return 0;
}
