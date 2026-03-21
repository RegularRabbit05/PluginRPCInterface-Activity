#pragma once
#include <stdlib.h>
#include <alloca.h>
#include <cstring>
#include <physfs.h>
#include "rpc.hpp"

static void PFS_RemoteUnmountCallback(void *d) {
    free(d);
}

static bool PFS_RemoteLoad(const char* url, const char* mount) {
    char *uClone = (char *) alloca(strlen(url)+1);
    strcpy(uClone, url);
    const char *fileName = strchr(url, '/');
    if (!fileName) fileName = url;
    int fileSize = 0;
    void * fileData = rpcDownloadFile(uClone, &fileSize);
    if (fileSize <= 0 || !fileData) {
        err:
        if (fileData) free(fileData);
        TraceLog(LOG_ERROR, "Downloading %s (%s) has failed!", url, mount);
        return false;
    }

    TraceLog(LOG_INFO, "Finished downloading %d bytes from URL %s.", fileSize, url);
    if (PHYSFS_mountMemory(fileData, fileSize, PFS_RemoteUnmountCallback, fileName, mount, 1) == 0) {
        TraceLog(LOG_ERROR, "Mounting %s to %s has failed with %s!", url, mount, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        goto err;
    }
    TraceLog(LOG_INFO, "%s mounted successfully to %s", url, mount);
    return true;
}

static unsigned char* PFS_LoadFileDataCallback(const char* fileName, int* dataSize) {
    TraceLog(LOG_INFO, "FS: Opening file %s", fileName);
    PHYSFS_File* f = PHYSFS_openRead(fileName);
    if (f == nullptr) {
        TraceLog(LOG_WARNING, "FS: Failed to open file %s", fileName);
        return nullptr;
    }
    *dataSize = (int) PHYSFS_fileLength(f);
    auto * mem = (unsigned char*) malloc(*dataSize);
    if (mem == nullptr) {
        TraceLog(LOG_WARNING, "FS: Failed to allocate memory for file %s", fileName);
        return nullptr;
    }
    PHYSFS_readBytes(f, mem, *dataSize);
    PHYSFS_close(f);
    return mem;
}

static char* PFS_LoadFileTextCallback(const char* fileName) {
    TraceLog(LOG_INFO, "FS: Opening file %s", fileName);
    PHYSFS_File* f = PHYSFS_openRead(fileName);
    if (f == nullptr) {
        TraceLog(LOG_WARNING, "FS: Failed to open file %s", fileName);
        return nullptr;
    }
    const int dataSize = (int) PHYSFS_fileLength(f) + 1;
    auto * mem = (char*) malloc(dataSize);
    if (mem == nullptr) {
        TraceLog(LOG_WARNING, "FS: Failed to allocate memory for file %s", fileName);
        return nullptr;
    }
    PHYSFS_readBytes(f, mem, dataSize);
    PHYSFS_close(f);
    mem[dataSize-1] = 0;
    return mem;
}