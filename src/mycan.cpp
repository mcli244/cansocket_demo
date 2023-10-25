#include "mycan.h"
#include <iostream>
#include <assert.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// #include "isotp.h"

using namespace std;

CANDEV::CANDEV(string dev, CANDEV::SocketType type)
    :ifname(dev), stype(type)
{
    int ret = -1;
    struct sockaddr_can addr;
    struct ifreq ifr;
    // struct can_isotp_options opts;
    // struct can_isotp_fc_options fcopts;
    int canfd_on = 1;

    switch (type)
    {
    case SocketType::ST_CAN_ISOTP:
    case SocketType::ST_CANFD_ISOTP:
        cout << "ST_CANFD_ISOTP or ST_CAN_ISOTP" << endl;
        fd = socket(AF_CAN, SOCK_DGRAM, CAN_ISOTP); // 创建 SocketCAN 套接字
        addr.can_addr.tp.tx_id = 0x722;
        addr.can_addr.tp.rx_id = 0x788;
        /* 支持Canfd */
        setsockopt(fd, CAN_ISOTP, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
        break;
    case SocketType::ST_CANFD:
        cout << "CANFD" << endl;
        fd = socket(PF_CAN, SOCK_RAW, CAN_RAW); // 创建 SocketCAN 套接字
        /* 支持Canfd */
        setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
        break;
    case SocketType::ST_CAN:
    default:
        cout << "CAN" << endl;
        fd = socket(PF_CAN, SOCK_RAW, CAN_RAW); // 创建 SocketCAN 套接字
        break;
    }

    strcpy(ifr.ifr_name, ifname.c_str());
    ioctl(fd, SIOCGIFINDEX, &ifr); 
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr)); // 将套接字与 can0 绑定
    if (ret < 0)
    {
        cout << "bind fail! ifname:" << ifname << endl;
    }
}

/* CAN DLC to real data length conversion helpers */

static const unsigned char dlc2len[] = {0, 1, 2, 3, 4, 5, 6, 7,
					8, 12, 16, 20, 24, 32, 48, 64};

/* get data length from raw data length code (DLC) */
unsigned char can_fd_dlc2len(unsigned char dlc)
{
	return dlc2len[dlc & 0x0F];
}

static const unsigned char len2dlc[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,		/* 0 - 8 */
					9, 9, 9, 9,				/* 9 - 12 */
					10, 10, 10, 10,				/* 13 - 16 */
					11, 11, 11, 11,				/* 17 - 20 */
					12, 12, 12, 12,				/* 21 - 24 */
					13, 13, 13, 13, 13, 13, 13, 13,		/* 25 - 32 */
					14, 14, 14, 14, 14, 14, 14, 14,		/* 33 - 40 */
					14, 14, 14, 14, 14, 14, 14, 14,		/* 41 - 48 */
					15, 15, 15, 15, 15, 15, 15, 15,		/* 49 - 56 */
					15, 15, 15, 15, 15, 15, 15, 15};	/* 57 - 64 */

/* map the sanitized data length to an appropriate data length code */
unsigned char can_fd_len2dlc(unsigned char len)
{
	if (len > 64)
		return 0xF;

	return len2dlc[len];
}

int CANDEV::send(int canId, uint8_t *dat, int len)
{
    int ret = -1;
    struct can_frame frame;  // 声明 can 帧结构体，can_frame 定义在头文件 can.h 中
    struct canfd_frame fdframe;

    switch (stype)
    {
    case SocketType::ST_CAN_ISOTP:
    case SocketType::ST_CANFD_ISOTP:
        ret = write(fd, dat, len);  // 写数据
        break;
    case SocketType::ST_CANFD:
        cout << "CANFD" << endl;
        memset(&fdframe, 0, sizeof(struct canfd_frame));
        fdframe.can_id = canId > CAN_SFF_MASK ? (canId | CAN_EFF_FLAG) : canId;
        fdframe.len = len > CANFD_MAX_DLEN ? CANFD_MAX_DLEN : len;
        fdframe.len = can_fd_dlc2len(can_fd_len2dlc(fdframe.len));
        memcpy(fdframe.data, dat, len > CANFD_MAX_DLEN ? CANFD_MAX_DLEN : len);
        ret = write(fd, &fdframe, CANFD_MTU);  // 写数据
        break;
    case SocketType::ST_CAN:
    default:
        cout << "CAN len:"<< len << endl;
        memset(&frame, 0, sizeof(struct can_frame));
        frame.can_id = canId > CAN_SFF_MASK ? (canId | CAN_EFF_FLAG) : canId;
        frame.can_dlc = len > CAN_MAX_DLEN ? CAN_MAX_DLEN : len;	// 数据长度，最大8字节
        memcpy(frame.data, dat, len > CAN_MAX_DLEN ? CAN_MAX_DLEN : len);
        ret = write(fd, &frame, CAN_MTU);  // 写数据
        break;
    }
	
	return ret;
}

CANDEV::~CANDEV()
{
    close(fd);
}
