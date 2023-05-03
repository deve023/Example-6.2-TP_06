// FALTA DECLARAR E INICIALIZAR MATRIZ Y PUNTERO.
// PONER EN TODOS LOS PROTOTIPOS DE LAS FUNCIONES LOS NUEVOS PARAMETROS.

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

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000
#define DISPLAY_ROWS 4 // UM 
#define DISPLAY_COLS 20 // UM 

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];
char matrixDisplay[DISPLAY_ROWS][DISPLAY_COLS] = {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}}; // UM - Matrix Dispaly delacration and initialization.
char* matrixDisplayPointer = &matrixDisplay[0][0]; // UM - Matrix display pointer declaration and initialization.

//=====[Declaration and initialization of private global variables]============

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

static void printMatrixDisplay();

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

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_CONNECTION_GPIO_4BITS );
     
    displayCharPositionWrite ( 0,0, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
    displayStringWrite( "Temperature:", matrixDisplayPointer ); // UM - Adding new parameters.

    displayCharPositionWrite ( 0,1, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
    displayStringWrite( "Gas:" ); // UM - Adding new parameters.
    
    displayCharPositionWrite ( 0,2, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
    displayStringWrite( "Alarm:", matrixDisplayPointer ); // UM - Adding new parameters.
}

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
    char temperatureString[3] = "";
    
    if( accumulatedDisplayTime >=
        DISPLAY_REFRESH_TIME_MS ) {

        accumulatedDisplayTime = 0;

        sprintf(temperatureString, "%.0f", temperatureSensorReadCelsius());
        displayCharPositionWrite ( 12,0, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
        displayStringWrite( temperatureString, matrixDisplayPointer ); // UM - Adding new parameters.
        displayCharPositionWrite ( 14,0, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
        displayStringWrite( "'C", matrixDisplayPointer ); // UM - Adding new parameters.

        displayCharPositionWrite ( 4,1, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.

        if ( gasDetectorStateRead() ) {
            displayStringWrite( "Detected    ", matrixDisplayPointer ); // UM - Adding new parameters.
        } else {
            displayStringWrite( "Not Detected", matrixDisplayPointer ); // UM - Adding new parameters.
        }

        displayCharPositionWrite ( 6,2, matrixDisplay, matrixDisplayPointer ); // UM - Adding new parameters.
        
        if ( sirenStateRead() ) {
            displayStringWrite( "ON ", matrixDisplayPointer ); // UM - Adding new parameters.
        } else {
            displayStringWrite( "OFF", matrixDisplayPointer ); // UM - Adding new parameters.
        }

    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;        
    } 
    printMatrixDisplay();
}

static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}


static void printMatrixDisplay() // UM - Function that prints matrix display.
{
    for(int i=0 ; i<DISPLAY_ROWS ; i++){
        for(int j=0 ; i<DISPLAY_COLS ; j++){
            printf(%c, matrixDisplay[i][j]);
        }
        printf('\n');
    }
}

