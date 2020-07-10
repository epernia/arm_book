//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define CODE_DIGITS 3
#define KEYPAD_AMOUNT_OF_ROWS   4
#define KEYPAD_AMOUNT_OF_COLS   4
#define START_HOUR 8
#define END_HOUR 16

//=====[Declaration of public data types]======================================

typedef enum {
    DOOR_CLOSED,
    DOOR_UNLOCKED,
    DOOR_OPEN
} doorState_t;

//=====[Declaration and intitalization of public global objects]===============

DigitalOut keypadRowPins[KEYPAD_AMOUNT_OF_ROWS] = {D23, D22, D21, D20};
DigitalIn keypadColPins[KEYPAD_AMOUNT_OF_COLS]  = {D19, D18, D17, D16};

DigitalIn mainDoorHandle(BUTTON1);

DigitalOut doorUnlockedLed(LED1);
DigitalOut doorLockedLed(LED2);
DigitalOut incorrectCodeLed(LED3);

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and intitalization of public global variables]=============

char keypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

char buffer[32];

doorState_t mainDoorState;

struct tm t;

time_t seconds;

int codeDigits[] = {'1','4','7'};

//=====[Declarations (prototypes) of public functions]=========================

void uartTask();
void availableCommands();
void mainDoorUpdate();
void mainDoorInit();
int keypadScan();
void keypadInit();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    mainDoorInit();
    keypadInit();
    while (true) {
        mainDoorUpdate();
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
            uartUsb.printf("Enter the current year (YYYY): ");
            uartUsb.scanf("%d",&aux);
            t.tm_year = aux - 1900;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Enter the current month (1-12): ");
            uartUsb.scanf("%d",&aux);
            t.tm_mon = aux - 1;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Enter the current day (1-31): ");
            uartUsb.scanf("%d",&t.tm_mday);
            uartUsb.printf("%d\r\n",t.tm_mday);

            uartUsb.printf("Enter the current hour (0-24): ");
            uartUsb.scanf("%d",&t.tm_hour);
            uartUsb.printf("%d\r\n",t.tm_hour);

            uartUsb.printf("Enter the current minutes (0-59): ");
            uartUsb.scanf("%d",&t.tm_min);
            uartUsb.printf("%d\r\n",t.tm_min);

            uartUsb.printf("Enter the current seconds (0-59): ");
            uartUsb.scanf("%d",&t.tm_sec);
            uartUsb.printf("%d\r\n",t.tm_sec);

            while ( uartUsb.readable() ) {
                uartUsb.getc();
            }

            t.tm_isdst = -1;

            set_time( mktime( &t ) );
            break;

        case 't':
        case 'T':
            seconds = time(NULL);
            uartUsb.printf("Date and Time = %s", ctime(&seconds));
            strftime(buffer, 32, "%I:%M:%S %p\n", localtime(&seconds));
            uartUsb.printf("Time as a custom formatted string = %s", buffer);
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

void mainDoorInit()
{
    doorUnlockedLed = OFF;
    doorLockedLed = ON;
    incorrectCodeLed = OFF;
    mainDoorState = DOOR_CLOSED;
}

void mainDoorUpdate()
{
    bool incorrectCode;
    int keyPressed;
    struct tm * timeinfo;
    int prevKeyPressed;
    int i;

    switch( mainDoorState ) {
    case DOOR_CLOSED:
        if ( keypadScan() == 'A' ) {
            seconds = time(NULL);
            timeinfo = localtime ( &seconds );
            keyPressed = 'A';

            if ( ( timeinfo->tm_hour >= START_HOUR ) &&
                 ( timeinfo->tm_hour <= END_HOUR ) ) {
                incorrectCode = false;
                prevKeyPressed = 'A';

                for ( i = 0; i < CODE_DIGITS; i++) {
                    while ( ( keyPressed == 0 ) ||
                            ( keyPressed == prevKeyPressed ) ) {

                        keyPressed = keypadScan();
                    }
                    prevKeyPressed = keyPressed;
                    if ( keyPressed != codeDigits[i] ) {
                        incorrectCode = true;
                    }
                }

                if ( incorrectCode ) {
                    incorrectCodeLed = ON;
                    delay (1000);
                    incorrectCodeLed = OFF;
                } else {
                    mainDoorState = DOOR_UNLOCKED;
                    doorLockedLed = OFF;
                    doorUnlockedLed = ON;
                }
            }
        }
        break;

    case DOOR_UNLOCKED:
        if ( mainDoorHandle ) {
            doorUnlockedLed = OFF;
            mainDoorState = DOOR_OPEN;
        }
        break;

    case DOOR_OPEN:
        if ( !mainDoorHandle ) {
            doorLockedLed = ON;
            mainDoorState = DOOR_CLOSED;
        }
        break;

    default:
        mainDoorInit();
        break;
    }
}                                                                              

void keypadInit()
{
    int pinIndex = 0;

    for( pinIndex=0; pinIndex<KEYPAD_AMOUNT_OF_ROWS; pinIndex++ ) {
        keypadRowPins[pinIndex] = ON;
    }

    for( pinIndex=0; pinIndex<KEYPAD_AMOUNT_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
}                                                                              

int keypadScan()
{
    int r = 0;
    int c = 0;
    int i = 0;

    for( r=0; r<KEYPAD_AMOUNT_OF_ROWS; r++ ) {

        for( i=0; i<KEYPAD_AMOUNT_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[r] = OFF;

        for( c=0; c<KEYPAD_AMOUNT_OF_COLS; c++ ) {

            if( keypadColPins[c] == OFF ) {
                return keypadIndexToCharArray[r*KEYPAD_AMOUNT_OF_ROWS + c];
            }
        }
    }
    return 0;
}                                                                              