#include <iostream>
#include <string>

#include "../src/qrcode.h"
#include "QrCode.hpp"

static uint32_t check(const qrcodegen::QrCode &nayuki, QRCode *ricmoo) {
    uint32_t wrong = 0;

    if (nayuki.size != ricmoo->size) { wrong += (1 << 20); }

    int border = 4;
    for (int y = -border; y < nayuki.size + border; y++) {
        for (int x = -border; x < nayuki.size + border; x++) {
            if (!!nayuki.getModule(x, y) != qrcode_getModule(ricmoo, x, y)) {
                wrong++;
            }
        }
    }

    return wrong;
}

int main() {
    std::clock_t t0, totalNayuki, totalRicMoo;

    int total = 0, passed = 0;
    for (char version = 1; version <= 40; version++) {
        if (LOCK_VERSION != 0 && LOCK_VERSION != version) { continue; }

        for (char ecc = 0; ecc < 4; ecc++) {
            const qrcodegen::QrCode::Ecc *errCorLvl;
            switch (ecc) {
                case 0:
                    errCorLvl = &qrcodegen::QrCode::Ecc::LOW;
                    break;
                case 1:
                    errCorLvl = &qrcodegen::QrCode::Ecc::MEDIUM;
                    break;
                case 2:
                    errCorLvl = &qrcodegen::QrCode::Ecc::QUARTILE;
                    break;
                case 3:
                    errCorLvl = &qrcodegen::QrCode::Ecc::HIGH;
                    break;
            }

            for (char tc = 0; tc < 3; tc++) {
                char *data;
                switch(tc) {
                    case 0:
                        data = (char*)"HELLO";
                        break;
                    case 1:
                        data = (char*)"Hello";
                        break;
                    case 2:
                        data = (char*)"1234";
                        break;
                }
                t0 = std::clock();
                const qrcodegen::QrCode nayuki = qrcodegen::QrCode::encodeText(data, version, *errCorLvl);
                totalNayuki += std::clock() - t0;

                t0 = std::clock();
                QRCode ricmoo;
                uint8_t ricmooBytes[qrcode_getBufferSize(version)];
                qrcode_initText(&ricmoo, ricmooBytes, version, ecc, data);
                totalRicMoo += std::clock() - t0;

                uint32_t badModules = check(nayuki, &ricmoo);
                if (badModules) {
                    printf("Failed test case: version=%d, ecc=%d, data=\"%s\", faliured=%d\n", version, ecc, data, badModules);
                } else {
                    passed++;
                }

                total++;
            }
        }
    }

    printf("Tests complete: %d passed (out of %d)\n", passed, total);
    printf("Timing: Nayuki=%lu, RicMoo=%lu\n", totalNayuki, totalRicMoo);
}
