#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "typewise-alert.h"

// Mocking class to capture output
class MockAlert {
public:
    MOCK_METHOD(void, sendToController, (BreachType), ());
    MOCK_METHOD(void, sendToEmail, (BreachType), ());
};

MockAlert mockAlert;

// Override send functions with mocks for testing
void sendToController(BreachType breachType) {
    mockAlert.sendToController(breachType);
}

void sendToEmail(BreachType breachType) {
    mockAlert.sendToEmail(breachType);
}

// Test for getTemperatureLimits with invalid cooling type
TEST(TypeWiseAlertTestSuite, GetTemperatureLimitsInvalidCoolingType) {
    EXPECT_EXIT(getTemperatureLimits(static_cast<CoolingType>(-1)), 
                ::testing::ExitedWithCode(EXIT_FAILURE), "Error: Invalid cooling type.");
}

// Test inferBreach for TOO_LOW and NORMAL cases
TEST(TypeWiseAlertTestSuite, InferBreachTooLowAndNormal) {
    EXPECT_EQ(inferBreach(10, 20, 30), TOO_LOW);  // Test TOO_LOW
    EXPECT_EQ(inferBreach(25, 20, 30), NORMAL);   // Test NORMAL
}

// Test sendToController with breach type
TEST(TypeWiseAlertTestSuite, SendToControllerWithBreachType) {
    EXPECT_CALL(mockAlert, sendToController(TOO_LOW)).Times(1);
    sendToController(TOO_LOW);
}

// Test sendToEmail with invalid breach type
TEST(TypeWiseAlertTestSuite, SendToEmailInvalidBreachType) {
    EXPECT_EXIT(sendToEmail(static_cast<BreachType>(-1)), 
                ::testing::ExitedWithCode(EXIT_FAILURE), "Error: Invalid breach type for email notification.");
}

// Test handleAlert with invalid alert target
TEST(TypeWiseAlertTestSuite, HandleAlertInvalidAlertTarget) {
    EXPECT_EXIT(handleAlert(static_cast<AlertTarget>(-1), TOO_HIGH), 
                ::testing::ExitedWithCode(EXIT_FAILURE), "Error: Invalid alert target.");
}

// Test checkAndAlert for high temperature
TEST(TypeWiseAlertTestSuite, CheckAndAlertHighTemperature) {
    BatteryCharacter batteryChar;
    batteryChar.coolingType = HI_ACTIVE_COOLING;
    
    EXPECT_CALL(mockAlert, sendToEmail(TOO_HIGH)).Times(1);
    checkAndAlert(TO_EMAIL, batteryChar, 46);  // Exceeds HI_ACTIVE_COOLING limit
}
