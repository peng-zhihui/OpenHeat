/**
 *  QRCode
 *
 *  A quick example of generating a QR code.
 *
 *  This prints the QR code to the serial monitor as solid blocks. Each module
 *  is two characters wide, since the monospace font used in the serial monitor
 *  is approximately twice as tall as wide.
 *
 */

#include "qrcode.h"

void setup() {
    Serial.begin(115200);

    // Start time
    uint32_t dt = millis();
  
    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, "HELLO WORLD");
  
    // Delta time
    dt = millis() - dt;
    Serial.print("QR Code Generation Time: ");
    Serial.print(dt);
    Serial.print("\n");

    // Top quiet zone
    Serial.print("\n\n\n\n");

    for (uint8_t y = 0; y < qrcode.size; y++) {

        // Left quiet zone
        Serial.print("        ");

        // Each horizontal module
        for (uint8_t x = 0; x < qrcode.size; x++) {

            // Print each module (UTF-8 \u2588 is a solid block)
            Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588\u2588": "  ");

        }

        Serial.print("\n");
    }

    // Bottom quiet zone
    Serial.print("\n\n\n\n");
}

void loop() {

}
