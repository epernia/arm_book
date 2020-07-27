//=====[Libraries]=============================================================

#include "pc_serial_communication.h"

//=====[Declaration of private data types]=====================================



//=====[Declaration of private constants]======================================



//=====[Declaration of external public global objects]=========================



//=====[Declaration and intitalization of public global objects]===============



//=====[Declaration and intitalization of private global objects]==============

Serial uartUsb(USBTX, USBRX);

//=====[Declaration of external public global variables]=======================



//=====[Declaration and intitalization of public global variables]=============



//=====[Declaration and intitalization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================



//=====[Implementations of public functions]===================================

void uartTask()
{
    if( uartUsb.readable() ) {
        char receivedChar = uartUsb.getc();
        switch (receivedChar) {
        case '1':
            if ( alarmState ) {
                uartUsb.printf( "The alarmLed is activated\r\n");
            } else {
                uartUsb.printf( "The alarmLed is not activated\r\n");
            }
            break;

        case '2':
            if ( gasDetector ) {
                uartUsb.printf( "Gas is being detected\r\n");
            } else {
                uartUsb.printf( "Gas is not being detected\r\n");
            }
            break;

        case '3':
            if ( overTempDetector ) {
                uartUsb.printf( "Temperature is above the maximum level\r\n");
            } else {
                uartUsb.printf( "Temperature is below the maximum level\r\n");
            }
            break;

        case '4':
            uartUsb.printf( "Please enter the new four digits numeric code " );
            uartUsb.printf( "to deactivate the alarm.\r\n" );

            incorrectCode = false;

            for ( keyBeingCompared = 0;
                  keyBeingCompared < NUMBER_OF_KEYS;
                  keyBeingCompared++) {
                receivedChar = uartUsb.getc();
                uartUsb.printf( "*" );
                if ( codeSequence[keyBeingCompared] != receivedChar ) {
                    incorrectCode = true;
                }
            }

            if ( incorrectCode == false ) {
                uartUsb.printf( "\r\nThe code is correct\r\n\r\n" );
                alarmState = OFF;
                incorrectCodeLed = OFF;
                numberOfIncorrectCodes = 0;
            } else {
                uartUsb.printf( "\r\nThe code is incorrect\r\n\r\n" );
                incorrectCodeLed = ON;
                numberOfIncorrectCodes = numberOfIncorrectCodes + 1;
            }
            break;

        case '5':
            uartUsb.printf( "Please enter the new four digits numeric code" );

            for ( keyBeingCompared = 0;
                  keyBeingCompared < NUMBER_OF_KEYS;
                  keyBeingCompared++) {
                codeSequence[keyBeingCompared] = uartUsb.getc();
                uartUsb.printf( "*" );
            }

            uartUsb.printf( "\r\nNew code configurated\r\n\r\n" );
            break;

        case 'c':
        case 'C':
            uartUsb.printf( "Temperature: %.2f °C\r\n", lm35TempC );
            break;

        case 'f':
        case 'F':
            uartUsb.printf( "Temperature: %.2f °F\r\n",
                            celsiusToFahrenheit( lm35TempC ) );
            break;

        case 's':
        case 'S':
            struct tm rtcTime;
            int uartReceivedYear;
            uartUsb.printf("Enter the current year (YYYY): ");
            uartUsb.scanf("%d",&uartReceivedYear);
            rtcTime.tm_year = uartReceivedYear - 1900;
            uartUsb.printf("%d\r\n",uartReceivedYear);

            int uartReceivedMonth;
            uartUsb.printf("Enter the current month (1-12): ");
            uartUsb.scanf("%d",&uartReceivedMonth);
            rtcTime.tm_mon = uartReceivedMonth - 1;
            uartUsb.printf("%d\r\n",uartReceivedMonth);

            uartUsb.printf("Enter the current day (1-31): ");
            uartUsb.scanf("%d",&rtcTime.tm_mday);
            uartUsb.printf("%d\r\n",rtcTime.tm_mday);

            uartUsb.printf("Enter the current hour (0-24): ");
            uartUsb.scanf("%d",&rtcTime.tm_hour);
            uartUsb.printf("%d\r\n",rtcTime.tm_hour);

            uartUsb.printf("Enter the current minutes (0-59): ");
            uartUsb.scanf("%d",&rtcTime.tm_min);
            uartUsb.printf("%d\r\n",rtcTime.tm_min);

            uartUsb.printf("Enter the current seconds (0-59): ");
            uartUsb.scanf("%d",&rtcTime.tm_sec);
            uartUsb.printf("%d\r\n",rtcTime.tm_sec);

            while ( uartUsb.readable() ) {
                uartUsb.getc();
            }

            rtcTime.tm_isdst = -1;

            set_time( mktime( &rtcTime ) );
            break;

        case 't':
        case 'T':
            time_t epochSeconds;
            epochSeconds = time(NULL);
            uartUsb.printf("Date and Time = %s", ctime(&epochSeconds));
            break;

        case 'e':
        case 'E':
            for (int i = 0; i < eventsIndex; i++) {
                uartUsb.printf("Event = %s\r\n", arrayOfStoredEvents[i].typeOfEvent);
                uartUsb.printf("Date and Time = %s\r\n",
                               ctime(&arrayOfStoredEvents[i].seconds));
                uartUsb.printf("\r\n");
            }
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
    uartUsb.printf( "Press '1' to get the alarm state\r\n" );
    uartUsb.printf( "Press '2' for gas detector state\r\n" );
    uartUsb.printf( "Press '3' for over temperature detector state\r\n" );
    uartUsb.printf( "Press '4' to enter the code sequence\r\n" );
    uartUsb.printf( "Press '5' to enter a new code\r\n" );
    uartUsb.printf( "Press 'f' or 'F' to get lm35 reading in Fahrenheit\r\n" );
    uartUsb.printf( "Press 'c' or 'C' to get lm35 reading in Celsius\r\n" );
    uartUsb.printf( "Press 's' or 'S' to set the date and time\r\n" );
    uartUsb.printf( "Press 't' or 'T' to get the date and time\r\n" );
    uartUsb.printf( "Press 'e' or 'E' to get the stored events\r\n" );
    uartUsb.printf( "\r\n" );
}

//=====[Implementations of private functions]==================================

