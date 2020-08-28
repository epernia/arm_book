//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "code.h"
#include "siren.h"
#include "smart_home_system.h"
#include "fire_alarm.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"
#include "display.h"
#include "GLCD_fire_alarm.h"

//=====[Declaration of private defines]======================================

#define DISPLAY_REFRESH_TIME_REPORT_MS 1000
#define DISPLAY_REFRESH_TIME_ALARM_MS 300

//=====[Declaration of private data types]=====================================

typedef enum{
    DISPLAY_ALARM_STATE,
    DISPLAY_REPORT_STATE
} displayState_t;

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static displayState_t displayState = DISPLAY_REPORT_STATE;
static int displayAlarmGraphicSequence;
static int displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceMatrixKeypadUpdate();
static void incorrectCodeIndicatorUpdate();
static void systemBlockedIndicatorUpdate();
static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    incorrectCodeLed = OFF;
    systemBlockedLed = OFF;
    matrixKeypadInit( SYSTEM_TIME_INCREMENT_MS );
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    userInterfaceMatrixKeypadUpdate();
    incorrectCodeIndicatorUpdate();
    systemBlockedIndicatorUpdate();
    userInterfaceDisplayUpdate();
}

bool incorrectCodeStateRead()
{
    return incorrectCodeState;
}

void incorrectCodeStateWrite( bool state )
{
    incorrectCodeState = state;
}

bool systemBlockedStateRead()
{
    return systemBlockedState;
}

void systemBlockedStateWrite( bool state )
{
    systemBlockedState = state;
}

bool userInterfaceCodeCompleteRead()
{
    return codeComplete;
}

void userInterfaceCodeCompleteWrite( bool state )
{
    codeComplete = state;
}

//=====[Implementations of private functions]==================================

static void userInterfaceMatrixKeypadUpdate()
{
    static int numberOfHashKeyReleased = 0;
    char keyReleased = matrixKeypadUpdate();

    if( keyReleased != '\0' ) {

        if( sirenStateRead() && !systemBlockedStateRead() ) {
            if( !incorrectCodeStateRead() ) {
                codeSequenceFromUserInterface[numberOfCodeChars] = keyReleased;
                numberOfCodeChars++;
                if ( numberOfCodeChars >= CODE_NUMBER_OF_KEYS ) {
                    codeComplete = true;
                    numberOfCodeChars = 0;
                }
            } else {
                if( keyReleased == '#' ) {
                    numberOfHashKeyReleased++;
                    if( numberOfHashKeyReleased >= 2 ) {
                        numberOfHashKeyReleased = 0;
                        numberOfCodeChars = 0;
                        codeComplete = false;
                        incorrectCodeState = OFF;
                    }
                }
            }
        }
    }
}

static void userInterfaceDisplayTextInit()
{
    displayModeWrite( DISPLAY_MODE_CHAR );

    displayCommandWrite(DISPLAY_CMD_CLEAR);
    delay(2);
    
    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "Temperature:" );

    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "Gas:" );
    
    displayCharPositionWrite ( 0,2 );
    displayStringWrite( "Alarm:" );
}

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_TYPE_GLCD_ST7920, DISPLAY_CONNECTION_SPI,
                        16, 4,
                        8, 16,
                        128, 64 );
    userInterfaceDisplayTextInit();
}


static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
    char temperatureString[4];
    
    if( accumulatedDisplayTime >=
        displayRefreshTimeMs ) {

        accumulatedDisplayTime = 0;

        switch ( displayState ) {
            case DISPLAY_REPORT_STATE:
                sprintf(temperatureString, "%.0f", temperatureSensorReadCelsius());
                displayCharPositionWrite ( 12,0 );
                displayStringWrite( temperatureString );
                displayCharPositionWrite ( 14,0 );
                displayStringWrite( " C" );

                displayCharPositionWrite ( 4,1 );

                if ( gasDetectorStateRead() ) {
                    displayStringWrite( "Detected    " );
                } else {
                    displayStringWrite( "Not Detected" );
                }
                displayCharPositionWrite ( 6,2 );
                displayStringWrite( "OFF" );

                if ( sirenStateRead() ) {
                    displayState = DISPLAY_ALARM_STATE;
                    displayCommandWrite(DISPLAY_CMD_CLEAR);
                    delay(2);
                    displayModeWrite( DISPLAY_MODE_GRAPHIC );
                    displayAlarmGraphicSequence = 0;
                    displayRefreshTimeMs = DISPLAY_REFRESH_TIME_ALARM_MS;
                }

            break;

            case DISPLAY_ALARM_STATE:
                switch( displayAlarmGraphicSequence ){
                    case 0:
                        displayBitmapWrite( GLCD_fire_alarm_0 );
                        displayAlarmGraphicSequence++;
                    break;
                    case 1:
                        displayCommandWrite(DISPLAY_CMD_CLEAR);
                        delay(2);
                        displayAlarmGraphicSequence++;
                    break;
                    case 2:
                        displayBitmapWrite( GLCD_fire_alarm_1 );
                        displayAlarmGraphicSequence++;
                    break;
                    case 3:
                        displayCommandWrite(DISPLAY_CMD_CLEAR);
                        delay(2);
                        displayAlarmGraphicSequence++;
                    break;
                    case 4:
                        displayBitmapWrite( GLCD_fire_alarm_2 );
                        displayAlarmGraphicSequence++;
                    break;
                    case 5:
                        displayCommandWrite(DISPLAY_CMD_CLEAR);
                        delay(2);
                        displayAlarmGraphicSequence = 0;
                    break;
                }
                

                if ( !sirenStateRead() ) {
                    displayState = DISPLAY_REPORT_STATE;
                    userInterfaceDisplayTextInit();
                    displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;
                }
                
            break;

            default:
                displayState = DISPLAY_REPORT_STATE;
            break;
        }

    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;        
    }
    
}

static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}