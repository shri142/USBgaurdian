#include <Windows.h>
#include <iostream>

// Window procedure to handle messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Check if the message is WM_DEVICECHANGE
    if (message == WM_DEVICECHANGE) {
        // Cast lParam to PDEV_BROADCAST_HDR to access device information
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lParam;
        // Check if the device type is volume change
        if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
            // Cast lpdb to PDEV_BROADCAST_VOLUME to access volume information
            PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
            // Check if the volume change event is an arrival (new volume mounted)
            if (lpdbv->dbcv_flags == 0) {
                // Print a message indicating a USB drive was inserted
                std::cout << "USB drive inserted!" << std::endl;
                // TODO: Add code to disable auto-run here
            }
        }
    }
    // Call the default window procedure for other messages
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int main() {
    // Register a window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc; // Set the window procedure
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"USBWatcher"; // Name of the window class
    RegisterClass(&wc); // Register the window class

    // Create an invisible window to receive messages
    HWND hWnd = CreateWindow(wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
    if (!hWnd) {
        // Error handling if window creation fails
        std::cerr << "Error creating window" << std::endl;
        return 1;
    }

    // Register for device notifications
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {0};
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    // Register to receive notifications about device interface classes
    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

    // Start the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unregister device notifications when the program exits
    UnregisterDeviceNotification(hDevNotify);

    return 0;
}
