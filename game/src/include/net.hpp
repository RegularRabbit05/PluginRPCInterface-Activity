#pragma once
#include "rpc.hpp"

typedef struct {
    int size;
    char buffer[1024*1024];
} NetPacket;

NetPacket netReceivePacket() {
    NetPacket in;
    in.size = rpcReceiveSync(in.buffer, sizeof(in.buffer));
    if (in.size < -1) in.size = -1;
    if (in.size > ((int) sizeof(in.buffer))) {
        TraceLog(LOG_ERROR, "Packet was too big for buffer %d/%d", in.size, sizeof(in.buffer));
        in.size = -2;
    }
    return in;
}

json netParsePacket(NetPacket * p) {
    if (p->size <= 0) return {};
    return json::parse(p->buffer);
}