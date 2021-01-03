//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Defines]===============================================================

#define KEYPAD_AMOUNT_OF_ROWS   4
#define KEYPAD_AMOUNT_OF_COLS   4

//=====[Declaration of public data types]======================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut keypadRowPins[KEYPAD_AMOUNT_OF_ROWS] = {PB_3, PB_5, PC_7, PA_15};
DigitalIn keypadColPins[KEYPAD_AMOUNT_OF_COLS]  = {PB_12, PB_13, PB_15, PC_6};

Serial uartUsb(USBTX, USBRX);

//=====[Declaration and initialization of public global variables]=============

char keypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

char buffer[32];

struct tm RTCTime;

time_t seconds;

bool showKeypadInUart = false;

//=====[Declarations (prototypes) of public functions]=========================

void uartTask();
void availableCommands();
void keypadToUart();
int keypadScan();
void keypadInit();

//=====[Main function, the program entry point after power on or reset]========

int main()
{
    keypadInit();
    while (true) {
        keypadToUart();
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

        case 'k':
        case 'K':
            uartUsb.printf("\r\nButtons pressed at the matrix keypad\r\n");
            showKeypadInUart = true;
            break;

        case 'q':
        case 'Q':
            uartUsb.printf("\r\nQuiting of command k\r\n");
            showKeypadInUart = false;
            break;

        case 's':
        case 'S':
            uartUsb.printf("\r\nEnter the current year (YYYY): ");
            uartUsb.scanf("%d",&aux);
            RTCTime.tm_year = aux - 1900;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Enter the current month (1-12): ");
            uartUsb.scanf("%d",&aux);
            RTCTime.tm_mon = aux - 1;
            uartUsb.printf("%d\r\n",aux);

            uartUsb.printf("Enter the current day (1-31): ");
            uartUsb.scanf("%d",&RTCTime.tm_mday);
            uartUsb.printf("%d\r\n",RTCTime.tm_mday);

            uartUsb.printf("Enter the current hour (0-24): ");
            uartUsb.scanf("%d",&RTCTime.tm_hour);
            uartUsb.printf("%d\r\n",RTCTime.tm_hour);

            uartUsb.printf("Enter the current minutes (0-59): ");
            uartUsb.scanf("%d",&RTCTime.tm_min);
            uartUsb.printf("%d\r\n",RTCTime.tm_min);

            uartUsb.printf("Enter the current seconds (0-59): ");
            uartUsb.scanf("%d",&RTCTime.tm_sec);
            uartUsb.printf("%d\r\n",RTCTime.tm_sec);

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
    uartUsb.printf( "Press 'k' or 'K' to show the buttons pressed at the " );
    uartUsb.printf( "matrix keypad\r\n" );
    uartUsb.printf( "Press 'q' or 'Q' to quit the k command\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the time\r\n\r\n" );
}

void keypadToUart()
{
    bool incorrectCode;
    int keyPressed;
    struct tm * timeinfo;
    static int prevKeyPressed;
    int i;

    if ( showKeypadInUart ) {
        keyPressed = keypadScan();
        if ( ( keyPressed != 0 ) && ( keyPressed != prevKeyPressed ) ) {
            uartUsb.printf("%c",keyPressed);
            prevKeyPressed = keyPressed;
        }
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