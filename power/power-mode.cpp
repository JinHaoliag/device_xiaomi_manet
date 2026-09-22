/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <aidl/android/hardware/power/BnPower.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define TOUCH_DOUBLETAP_MODE 14
#define TOUCH_DEV_PATH "/dev/xiaomi-touch"
#define TOUCH_ID 0

struct TouchFeatureMode {
    uint8_t touchId;
    uint8_t command;
    uint16_t mode;
    uint16_t valueCount;
    uint16_t reserved;
    int32_t values[128];
};

static_assert(sizeof(TouchFeatureMode) == 520);

#define TOUCH_IOC_SETMODE _IOWR('T', 0, TouchFeatureMode)
#define TOUCH_IOC_SELECT_TOUCH _IOW('T', 3, int)

namespace aidl {
namespace android {
namespace hardware {
namespace power {
namespace impl {

using ::aidl::android::hardware::power::Mode;

bool isDeviceSpecificModeSupported(Mode type, bool* _aidl_return) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE:
            *_aidl_return = true;
            return true;
        default:
            return false;
    }
}

bool setDeviceSpecificMode(Mode type, bool enabled) {
    switch (type) {
        case Mode::DOUBLE_TAP_TO_WAKE: {
            int fd = open(TOUCH_DEV_PATH, O_RDWR | O_CLOEXEC);
            if (fd < 0) return false;

            if (ioctl(fd, TOUCH_IOC_SELECT_TOUCH, TOUCH_ID) < 0) {
                close(fd);
                return false;
            }

            TouchFeatureMode request{};
            request.touchId = TOUCH_ID;
            request.mode = TOUCH_DOUBLETAP_MODE;
            request.valueCount = 1;
            request.values[0] = enabled ? 1 : 0;

            const int rc = ioctl(fd, TOUCH_IOC_SETMODE, &request);
            close(fd);
            return rc >= 0;
        }
        default:
            return false;
    }
}

}  // namespace impl
}  // namespace power
}  // namespace hardware
}  // namespace android
}  // namespace aidl
