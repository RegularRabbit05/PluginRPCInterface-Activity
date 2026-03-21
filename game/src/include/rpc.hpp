#pragma once
#include <emscripten.h>
#include <atomic>

EM_JS(void, rpcGetVariable, (const char* name, char* buffer, int bufferSize), {
    const key = UTF8ToString(name);
    if (!window.gameEnv || !(key in window.gameEnv)) {
        buffer[0] = 0;
        return;
    }
    const value = window.gameEnv[key] || "";
    stringToUTF8(value, buffer, bufferSize-1);
    buffer[bufferSize-1] = 0;
});

EM_JS(void, rpcSendCommand, (const char* command), {
    const cmd = UTF8ToString(command);
    window.gameFunc(cmd)
});

EM_JS(int, rpcReceiveSync, (char* buffer, int bufferSize), {
    if (!window.gwsq) return -1;
    if (window.gwsq.size == 0) return -1;
    const packet = window.popPacketFunc();
    if (packet === undefined || packet == "") return 0;
    stringToUTF8(packet, buffer, bufferSize-1);
    buffer[bufferSize-1] = 0;
    if (bufferSize > packet.length) buffer[packet.length] = 0;
    return packet.length;
});

EM_JS(bool, rpcIsMobile, (), {
    return /Android|Mac|iPod|iPhone|iPad/.test(navigator.platform) && !window.MSStream;
});

struct RPCFile {
  std::atomic<bool> success{false};
  std::atomic<bool> error{false};
  char* data = nullptr;
  int length = 0;
};

void rpcDownloadFileSuccess(void* arg, void* data, int size) {
  RPCFile* result = (RPCFile*) arg;
  result->data = (char*) malloc(size);
  memcpy(result->data, data, size);
  result->length = size;
  TraceLog(LOG_INFO, "rpcDownloadFileSuccess: Downloaded %d bytes.", size);
  result->success = true;
}

void rpcDownloadFileError(void* arg) {
  RPCFile* result = (RPCFile*) arg;
  result->data = nullptr;
  result->length = -1;
  TraceLog(LOG_ERROR, "rpcDownloadFileError: Failed to download file.");
  result->error = true;
}

char* rpcDownloadFile(const char* url, int* out_len) {
  RPCFile result = {};
  emscripten_async_wget_data(url, &result, rpcDownloadFileSuccess, rpcDownloadFileError);
  while (!result.success && !result.error) {
    emscripten_sleep(33);
  }

  emscripten_sleep(33);
  if (result.data == nullptr) {
    *out_len = -1;
    return nullptr;
  }
  *out_len = result.length;
  return result.data;
}
