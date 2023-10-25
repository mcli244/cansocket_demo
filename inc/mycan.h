#ifndef __MYCAN_H__
#define __MYCAN_H__

#include <string>
#include <stdint.h>

using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

class CANDEV
{
    
public:
    enum class SocketType {ST_CAN, ST_CANFD, ST_CAN_ISOTP, ST_CANFD_ISOTP};

public:
    CANDEV(string dev, SocketType type = CANDEV::SocketType::ST_CAN);
    ~CANDEV();

    int send(int canId, uint8_t *dat, int len);

private:
    /* data */
    SocketType stype;   // FD
    int fd;
    int txId;
    int rxId;
    string ifname;
};



#ifdef __cplusplus
}
#endif

#endif /*__MYCAN_H__*/