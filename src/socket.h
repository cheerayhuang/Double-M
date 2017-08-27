/**************************************************************************
 *
 * Copyright (c) 2015 Cheeray Huang. All Rights Reserved
 *
 * @file: socket.h
 * @author: Huang Qiyu
 * @email: cheeray.huang@gmail.com
 * @date: 05-18-2017 16:10:11
 * @version $Revision$
 *
 **************************************************************************/

#ifndef __DOUBLE_M_SOCKET_H__
#define __DOUBLE_M_SOCKET_H__

#include <stdint.h>
#include <endian.h>

namespace mm {

class Socket {
public:
    explicit Socket(int fd)
        :sock_fd(fd) {
    }

    ~Socket();

    int sock_fd() const { return sock_fd_; }
    void BindAddress(const InetAddress& localaddr);
    void Listen();

    int Accept(InetAddress* peeraddr);
    void ShutdownWrite();

private:
    // sock can't be changed in life cycle.
    const int sock_fd_;

};

namespace endian {
    inline uint64_t hostToNetwork64(uint64_t host64)
    {
        return htobe64(host64);
    }

    inline uint32_t hostToNetwork32(uint32_t host32)
    {
        return htobe32(host32);
    }

    inline uint16_t hostToNetwork16(uint16_t host16)
    {
        return htobe16(host16);
    }

    inline uint64_t networkToHost64(uint64_t net64)
    {
        return be64toh(net64);
    }

    inline uint32_t networkToHost32(uint32_t net32)
    {
        return be32toh(net32);
    }

    inline uint16_t networkToHost16(uint16_t net16)
    {
        return be16toh(net16);
    }
}

namespace socketops {
    void AddrToIpPort();
    void AddrToIp();

    void IpPortToAddr();
    void IptoAddr
}

}


#endif

