#include "typewise-alert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const TemperatureLimits coolingLimits[] = {
    {0, 35},  // PASSIVE_COOLING
    {0, 45},  // HI_ACTIVE_COOLING
    {0, 40}   // MED_ACTIVE_COOLING
};

TemperatureLimits getTemperatureLimits(CoolingType coolingType) {
    if (coolingType < PASSIVE_COOLING || coolingType > MED_ACTIVE_COOLING) {
        fprintf(stderr, "Error: Invalid cooling type.\n");
        exit(EXIT_FAILURE);
    }
    return coolingLimits[coolingType];
}

BreachType inferBreach(double value, double lowerLimit, double upperLimit) {
    if (value < lowerLimit) {
        return TOO_LOW;
    }
    if (value > upperLimit) {
        return TOO_HIGH;
    }
    return NORMAL;
}

BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
    TemperatureLimits limits = getTemperatureLimits(coolingType);
    return inferBreach(temperatureInC, limits.lowerLimit, limits.upperLimit);
}
void sendToController(BreachType breachType) {
    const unsigned short header = 0xfeed;
    printf("Header: %x, Breach Type: %d\n", header, breachType);
}

void sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    const char* messages[] = {
        "Hi, the temperature is normal",   // NORMAL (0)
        "Hi, the temperature is too low",  // TOO_LOW (1)
        "Hi, the temperature is too high"  // TOO_HIGH (2)
    };

    // Check that breachType is within bounds of the messages array
    if (breachType >= NORMAL && breachType <= TOO_HIGH) {
        printf("To: %s\n", recipient);
        printf("%s\n", messages[breachType]);
    } else {
        fprintf(stderr, "Error: Invalid breach type for email notification.\n");
    }
}

void handleAlert(AlertTarget alertTarget, BreachType breachType) {
    if (alertTarget == TO_CONTROLLER) {
        sendToController(breachType);
    } else if (alertTarget == TO_EMAIL) {
        sendToEmail(breachType);
    } else {
        fprintf(stderr, "Error: Invalid alert target.\n");
    }
}

void checkAndAlert(AlertTarget alertTarget, BatteryCharacter batteryChar, double temperatureInC) {
    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);
    handleAlert(alertTarget, breachType);
}

int main() {
    
    BatteryCharacter batteryChar;
    batteryChar.coolingType = HI_ACTIVE_COOLING;
    strncpy(batteryChar.brand, "SampleBrand", sizeof(batteryChar.brand) - 1);
    batteryChar.brand[sizeof(batteryChar.brand) - 1] = '\0'; // Ensure null termination
   
    double temperatureInC = 50.0; // Example temperature
    AlertTarget alertTarget = TO_EMAIL; // Example alert target

    checkAndAlert(alertTarget, batteryChar, temperatureInC);
    return 0;
}
