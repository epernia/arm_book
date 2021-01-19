//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define CODE_DIGITS                              3
#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4
#define TIME_INCREMENT_MS                       10
#define DEBOUNCE_BUTTON_TIME_MS                 40
#define START_HOUR                               8
#define END_HOUR                                16

//=====[Declaration of public data types]======================================

typedef enum {
    DOOR_CLOSED,
    DOOR_UNLOCKED,
    DOOR_OPEN
} doorState_t;

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {D19, D18, D17, D16};

DigitalIn doorHandle(BUTTON1);

DigitalOut doorUnlockedLed(LED1);
DigitalOut doorLockedLed(LED2);
DigitalOut incorrectCodeLed(LED3);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

int accumulatedDebounceMatrixKeypadTime = 0;

char matrixKeypadLastkeyReleased = '\0';
char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};
matrixKeypadState_t matrixKeypadState;

doorState_t doorState;

struct tm RTCTime;

time_t seconds;

char codeSequence[CODE_DIGITS] = {'1','4','7'};

//=====[Declarations (prototypes) of public functions]=========================

void uartTask();
void availableCommands();
void doorInit();
void doorUpdate();
void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    doorInit();
    matrixKeypadInit();
    while (true) {
        doorUpdate();
        uartTask();
    }
}

//=====[Implementations of public functions]===================================

void uartTask()
{
    int aux;

    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();
        switch (receivedChar) {

        case 's':
        case 'S':
            uartUsb.printf("\r\nType de current year (YYYY) and press enter: ");
            uartUsb.scanf("%d",&aux);
            RTCTime.tm_year = aux - 1900;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Type de current month (1-12) and press enter: ");
            uartUsb.scanf("%d",&aux);
            RTCTime.tm_mon = aux - 1;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Type de current day (1-31) and press enter: ");
            uartUsb.scanf("%d",&RTCTime.tm_mday);
            uartUsb.printf("%d\r\n",RTCTime.tm_mday);

            uartUsb.printf("Type de current hour (0-23) and press enter: ");
            uartUsb.scanf("%d",&RTCTime.tm_hour);
            uartUsb.printf("%d\r\n",RTCTime.tm_hour);

            uartUsb.printf("Type de current minutes (0-59) and press enter: ");
            uartUsb.scanf("%d",&RTCTime.tm_min);
            uartUsb.printf("%d\r\n",RTCTime.tm_min);

            uartUsb.printf("Type de current seconds (0-59) and press enter: ");
            uartUsb.scanf("%d",&RTCTime.tm_sec);
            uartUsb.printf("%d\r\n",RTCTime.tm_sec);
            
            uartUsb.printf("Date and time has been set\r\n");

            while ( uartUsb.readable() ) {
                uartUsb.getc();
            }

            RTCTime.tm_isdst = -1;

            set_time( mktime( &RTCTime ) );
            break;

        case 't':
        case 'T':
            seconds = time(NULL);
            uartUsb.printf("Date and Time = %s", ctime(&seconds));
            break;

        default:
            availableCommands();
            break;
        }
    }
}                                                                              

void availableCommands()
{
    uartUsb.printf( "Available commands:\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the time\r\n\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the time\r\n\r\n" );
}

void doorInit()
{
    doorUnlockedLed = OFF;
    doorLockedLed = ON;
    incorrectCodeLed = OFF;
    doorState = DOOR_CLOSED;
}                                                                              

void doorUpdate()
{
    bool incorrectCode;
    char keyReleased;
    struct tm * currentTime;
    char prevkeyReleased;
    int i;

    switch( doorState ) {
    case DOOR_CLOSED:
        keyReleased = matrixKeypadUpdate();
        if ( keyReleased == 'A' ) {
            seconds = time(NULL);
            currentTime = localtime ( &seconds );

            if ( ( currentTime->tm_hour >= START_HOUR ) &&
                 ( currentTime->tm_hour <= END_HOUR ) ) {
                incorrectCode = false;
                prevkeyReleased = 'A';

                for ( i = 0; i < CODE_DIGITS; i++) {
                    while ( ( keyReleased == '\0' ) ||
                            ( keyReleased == prevkeyReleased ) ) {

                        keyReleased = matrixKeypadUpdate();
                    }
                    prevkeyReleased = keyReleased;
                    if ( keyReleased != codeSequence[i] ) {
                        incorrectCode = true;
                    }
                }

                if ( incorrectCode ) {
                    incorrectCodeLed = ON;
                    delay (1000);
                    incorrectCodeLed = OFF;
                } else {
                    doorState = DOOR_UNLOCKED;
                    doorLockedLed = OFF;
                    doorUnlockedLed = ON;
                }
            }
        }
        break;

    case DOOR_UNLOCKED:
        if ( doorHandle ) {
            doorUnlockedLed = OFF;
            doorState = DOOR_OPEN;
        }
        break;

    case DOOR_OPEN:
        if ( !doorHandle ) {
            doorLockedLed = ON;
            doorState = DOOR_CLOSED;
        }
        break;

    default:
        doorInit();
        break;
    }
}                                                                              

void matrixKeypadInit()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}                                                                              

char matrixKeypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0;

    for( r=0; r<KEYPAD_NUMBER_OF_ROWS; r++ ) {

        for( i=0; i<KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF;

        for( c=0; c<KEYPAD_NUMBER_OF_COLS; c++ ) {
            if( keypadColPins[c] == OFF ) {
                return matrixKeypadIndexToCharArray[r*KEYPAD_NUMBER_OF_ROWS + c];
            }
        }
    }
    return '\0';
}                                                        

char matrixKeypadUpdate()
{
    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ) {

    case MATRIX_KEYPAD_SCANNING:
        keyDetected = matrixKeypadScan();
        if( keyDetected != '\0' ) {
            matrixKeypadLastkeyReleased = keyDetected;
            accumulatedDebounceMatrixKeypadTime = 0;
            matrixKeypadState = MATRIX_KEYPAD_DEBOUNCE;
        }
        break;

    case MATRIX_KEYPAD_DEBOUNCE:
        if( accumulatedDebounceMatrixKeypadTime >=
            DEBOUNCE_BUTTON_TIME_MS ) {
            keyDetected = matrixKeypadScan();
            if( keyDetected == matrixKeypadLastkeyReleased ) {
                matrixKeypadState = MATRIX_KEYPAD_KEY_HOLD_PRESSED;
            } else {
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;
            }
        }
        accumulatedDebounceMatrixKeypadTime =
            accumulatedDebounceMatrixKeypadTime + TIME_INCREMENT_MS;
        break;

    case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
        keyDetected = matrixKeypadScan();
        if( keyDetected != matrixKeypadLastkeyReleased ) {
            if( keyDetected == '\0' ) {
                keyReleased = matrixKeypadLastkeyReleased;
            }
            matrixKeypadState = MATRIX_KEYPAD_SCANNING;
        }
        break;

    default:
        matrixKeypadInit();
        break;
    }
    return keyReleased;
}                                                                              