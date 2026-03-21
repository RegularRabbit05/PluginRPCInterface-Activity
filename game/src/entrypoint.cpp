#pragma once
#include <raylib.h>
#include "include/rpc.hpp"
#include "include/pfs.hpp"

#include "pages/spinner.cpp"
#include "pages/home.cpp"

int main(int argc, char *argv[]) {
    PHYSFS_init(argv[0]);
    void *current_state;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1920, 1080, "Activity Dashboard");
    SetLoadFileDataCallback(PFS_LoadFileDataCallback);
    SetLoadFileTextCallback(PFS_LoadFileTextCallback);
    if (!PFS_RemoteLoad("/dist/pkgs/system.zip", "system/")) goto cleanup;
    MaximizeWindow();

    current_state = spinner_init();
    spinner_loop((SpinnerState*)current_state);
    spinner_deinit((SpinnerState*)current_state);
    if (WindowShouldClose()) goto cleanup;

    current_state = home_init();
    home_loop((HomeState*)current_state);
    home_deinit((HomeState*)current_state);
    if (WindowShouldClose()) goto cleanup;

    cleanup:
    PHYSFS_unmount("system.zip");
    PHYSFS_deinit();
    CloseWindow();
    return 0;
}