#include <gtest/gtest.h>
#include <perfmon/pfmlib.h>
#include "PmuArch.h"

class PmuArchTest : public ::testing::Test {
protected:
    void SetUp() override {
      pfm_initialize();
    }

     void TearDown() override {
        pfm_terminate();
    }
};

/*
 * Test that we can find encodings for valid events, and we correctly fail with invalid events. The actual logic to
 * obtain the event encoding is performed by libpfm and is therefore not tested by us.
 *
 * getPerfEventAttr will generate error logs when an event's encoding cannot be found. Those should be ignored.
 */
TEST_F(PmuArchTest, getEventEncoding){
    PmuEvent valid = PmuEvent("RETIRED_INSTRUCTIONS", PmuEvent::HARDWARE);
    ASSERT_TRUE(PmuArch::getPerfEventAttr(valid).has_value());

    PmuEvent validPerfStandardEvent = PmuEvent("cycles", PmuEvent::SOFTWARE);
    ASSERT_TRUE(PmuArch::getPerfEventAttr(validPerfStandardEvent).has_value());

    PmuEvent invalid = PmuEvent("caca", PmuEvent::HARDWARE);
    ASSERT_FALSE(PmuArch::getPerfEventAttr(invalid).has_value());
}