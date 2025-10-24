#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <winsock2.h>           // Always include this FIRST
#include <ws2tcpip.h>           // For inet_pton, etc. (optional but recommended)
#include <windows.h>            // Include Windows AFTER winsock2
#include <iostream>             // For cout, cerr, endl
#include <Xinput.h>
#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

float LTValue = 0, RTValue = 0, joystickValue = 0;

int main() {
    // Initialize Winsock
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Failed to initialize Winsock." << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Failed to create socket." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4210); // Must match ESP32 port
    addr.sin_addr.s_addr = inet_addr("192.168.4.1"); // ESP32 IP

    cout << "Starting controller loop..." << endl;

    while (true) {
        // Read controller 0
        XINPUT_STATE state{};
        DWORD dwResult = XInputGetState(0, &state);

        if (dwResult == ERROR_SUCCESS) {
            // Store raw values
            LTValue = (float)state.Gamepad.bLeftTrigger;
            RTValue = (float)state.Gamepad.bRightTrigger;
            joystickValue = (float)state.Gamepad.sThumbLX;

            // Debug print (optional)
            cout << "\rLT: " << LTValue
                << " | RT: " << RTValue
            << " | Joystick: " << joystickValue << "    " << flush;


            // Prepare message
            char msg[64];
            sprintf_s(msg, "%f,%f,%f", LTValue, RTValue, joystickValue);

            // Send to ESP32
            sendto(sock, msg, strlen(msg), 0, (sockaddr*)&addr, sizeof(addr));
        }
        else cout << "Controller not connected.                     \r";

        Sleep(25);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
