#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "unistd.h"

// Constants
#define HEADER_BYTE 0x55
#define END_BYTE 0xAA
#define UART_DEVICE_NAME "/dev/ttyS0"
#define BAUD_RATE 115200

// Structure to represent data packet from module to screen
typedef struct {
    uint8_t header;    // Start byte indicating the beginning of a command
    uint8_t connected; // Number of connected devices
    uint16_t reserved; // Reserved field
    uint8_t actived;   // Is there a device that is casting
    uint8_t end;       // End byte indicating the end of a command
} ModuleToScreenPacket;

// Structure to represent data packet from screen to module
typedef struct {
    uint8_t header;     // Start byte indicating the beginning of a response
    uint8_t brightness; // Brightness of the screen
    uint8_t contrast;   // Contrast of the screen
    uint8_t end;        // End byte indicating the end of a response
} ScreenToModulePacket;

// Helper functions prototypes
int uart_open(const char* device_name, int baud_rate);
int uart_write(uint8_t* data, int data_size);
int uart_read(uint8_t* data, int data_size);
int uart_close();
void register_status_callback(void (*callback)(int connected, int actived));
void framebuffer_get_screen(uint8_t** pointer);
void framebuffer_gen_img_buffer(uint8_t* buf, int size, int brightness, int contrast);
void framebuffer_update();

// Function to send status update to the screen
void send_status_update(int connected, int actived) {
    ModuleToScreenPacket packet;
    packet.header = HEADER_BYTE;
    packet.connected = connected;
    packet.reserved = 0;
    packet.actived = actived;
    packet.end = END_BYTE;

    // Convert packet to byte array
    uint8_t data[sizeof(ModuleToScreenPacket)];
    memcpy(data, &packet, sizeof(ModuleToScreenPacket));

    // Write data to UART
    uart_write(data, sizeof(data));
}

// Function to receive configuration from the screen
void receive_configuration(int* brightness, int* contrast) {
    ScreenToModulePacket packet;

    // Read data from UART
    uint8_t data[sizeof(ScreenToModulePacket)];
    uart_read(data, sizeof(data));

    // Parse the received data directly into the structure
    packet.header = data[0];
    packet.brightness = data[1];
    packet.contrast = data[2];
    packet.end = data[3];

    *brightness = packet.brightness;
    *contrast = packet.contrast;
}

// Function to display user configuration on the frame buffer
void display_user_configuration(int brightness, int contrast) {
    uint8_t* framebuffer;
    framebuffer_get_screen(&framebuffer);

    int user_img_size = 640 * 480;
    uint8_t* user_img_buffer = (uint8_t*)malloc(user_img_size);

    framebuffer_gen_img_buffer(user_img_buffer, user_img_size, brightness, contrast);

    // Copy user configuration to the left bottom corner of the frame buffer
    for (int y = 0; y < 480; y++) {
        memcpy(framebuffer + (1080 - 480 + y) * 1920, user_img_buffer + y * 640, 640);
    }

    free(user_img_buffer);
    framebuffer_update();
}

int main(int argc, char* argv[]) {
    int brightness = 0;
    int contrast = 0;
    int connected_devices = 0;
    int casting_active = 0;

    // Open UART communication
    if (uart_open(UART_DEVICE_NAME, BAUD_RATE) == -1) {
        printf("Error: Unable to open UART communication\n");
        return -1;
    }

    // Register status callback
    register_status_callback(send_status_update);

    // Send status update every second
    while (1) {
        send_status_update(connected_devices, casting_active);

        // If no device is casting, show the user configuration on the frame buffer
        if (!casting_active) {
            receive_configuration(&brightness, &contrast);
            display_user_configuration(brightness, contrast);
        }

        Sleep(1000); // Update status every second
    }

    // Close UART communication
    uart_close();

    return 0;
}

int uart_open(const char* device_name, int baud_rate) {
    // Implementation of UART open
    printf("UART communication opened successfully.\n");
    return 0;
}

int uart_write(uint8_t* data, int data_size) {
    // Implementation of UART write
    printf("UART write successfully.\n");
    return 0;
}

int uart_read(uint8_t* data, int data_size) {
    // Implementation of UART read
    printf("UART read successfully.\n");
    uint8_t received_data[] = { HEADER_BYTE, 0x50, 0x40, END_BYTE }; // Example data
    memcpy(data, received_data, sizeof(received_data));
    return 0;
}

int uart_close() {
    // Implementation of UART close
    printf("UART closed successfully.\n");
    return 0;
}

void register_status_callback(void (*callback)(int connected, int actived)) {
    // Register the callback function
    printf("Status callback registered successfully.\n");
}

void framebuffer_get_screen(uint8_t** pointer) {
    // Simulate getting the frame buffer
    static uint8_t framebuffer[1920 * 1080];
    *pointer = framebuffer;
}

void framebuffer_gen_img_buffer(uint8_t* buf, int size, int brightness, int contrast) {
    // Simulate generating the user configuration image
    memset(buf, brightness, size); // Example implementation
}

void framebuffer_update() {
    // Simulate updating the frame buffer to the screen
    printf("Frame buffer updated.\n");
}