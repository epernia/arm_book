#include "mbed.h"
#include "arm_book.h"

typedef enum{
   DEBOUNCE_BTN_UP,
   DEBOUNCE_BTN_DOWN,
   DEBOUNCE_BTN_FALLING,
   DEBOUNCE_BTN_RISING
} debounceBtnState_t;

void debounceBtnError( int pin );
void debounceBtnInit( int pin );
void debounceBtnUpdate( int pin );

void enterBtnPressedEvent( void );
void enterBtnReleasedEvent( void );

debounceBtnState_t debounceBtnState;

int main(void){     

   int debounceBtnTimeCounter = 0;  
   debounceBtnInit();
   
   while(true) {
      if( debounceBtnTimeCounter = 4 ){ // Update every 40 ms
         debounceBtnUpdate( ENTER_BTN );
         debounceBtnTimeCounter = 0;
      }
      delay(10);
      debounceBtnTimeCounter++;      
   }
   return 0;
}


void enterBtnPressedEvent( void )
{
   
}

void enterBtnReleasedEvent( void )
{
   
}

void debounceBtnError( int pin )
{
   debounceBtnInit(pin);
}

void debounceBtnInit( int pin )
{
   debounceBtnState = BUTTON_UP;  // Set initial state
}

// FSM Update Sate Function
void debounceBtnUpdate( int pin )
{
   static bool_t flagFalling = FALSE;
   static bool_t flagRising = FALSE;
   
   static uint8_t contFalling = 0;
   static uint8_t contRising = 0;

   switch( debounceBtnState ){

      case DEBOUNCE_BTN_UP: 
         // CHECK TRANSITION CONDITIONS
         if( pin ){
            debounceBtnState = DEBOUNCE_BTN_FALLING;
         }
      break;

      case DEBOUNCE_BTN_DOWN:
         // CHECK TRANSITION CONDITIONS
         if( !pin ){
            debounceBtnState = DEBOUNCE_BTN_RISING;
         }
      break;

      case DEBOUNCE_BTN_FALLING:      
         // ENTRY
         if( flagFalling == FALSE ){
            flagFalling = TRUE;
            break;
         }      
         // CHECK TRANSITION CONDITIONS
         if( pin ){
            debounceBtnState = DEBOUNCE_BTN_DOWN;
            enterBtnPressedEvent();
         } else{
            debounceBtnState = DEBOUNCE_BTN_UP;
         }

         // EXIT
         if( debounceBtnState != DEBOUNCE_BTN_FALLING ){
            flagFalling = FALSE;
         }
      break;

      case DEBOUNCE_BTN_RISING:      
         // ENTRY
         if( flagRising == FALSE ){
            flagRising = TRUE;
            break;
         }    
         // CHECK TRANSITION CONDITIONS
         if( !pin ){
            debounceBtnState = DEBOUNCE_BTN_UP;
            enterBtnReleasedEvent();
         } else{
            debounceBtnState = DEBOUNCE_BTN_DOWN;
         }
         
         // EXIT
         if( debounceBtnState != DEBOUNCE_BTN_RISING ){
            flagRising = FALSE;
         }
      break;

      default:
         debounceBtnError(pin);
      break;
   }
}
