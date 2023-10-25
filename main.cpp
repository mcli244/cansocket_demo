#include <iostream>
#include <stdint.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include "mycan.h"

using namespace std;

int main(int argc, char **argv)
{
	cout << "can app cpp test" << endl;

    int opt;
    extern int optind, opterr, optopt;
    CANDEV::SocketType stype;
    char ifname[20]={0};

    while ((opt = getopt(argc, argv, "t:i:")) != -1) {
	    switch (opt) {
	    case 't':
		    if (0 != strncmp(optarg, "can", strlen("can")))
            {
                stype = CANDEV::SocketType::ST_CAN;
                cout << "optarg: "<< optarg << endl;
            }
                
            else if (0 == strncmp(optarg, "canfd", strlen("canfd")))
                stype = CANDEV::SocketType::ST_CANFD;
            else if (0 == strncmp(optarg, "cantp", strlen("cantp")))
                stype = CANDEV::SocketType::ST_CAN_ISOTP;
		    break;

	    case 'i':
		    strcpy(ifname, optarg);
		    break;
	    default:
		    fprintf(stderr, "Unknown option %c\n", opt);
		    exit(1);
		    break;
	    }
    }

    cout << "ifname: "<< ifname << endl;

    CANDEV can(ifname, stype);
    
    uint8_t dat[4096];

    for(int i=0; i<sizeof(dat); i++)
        dat[i] = i+0xA0;
    int len = sizeof(dat);
    can.send(0x1714, dat, len);
    

    return 0;
}