
#define NUMBER_OF_BITS 12  // En este caso son 12 bits, porque así está el ejemplo.

DigitalOut startOfConversion(LED1);  // Se prende solamente al principio una vez para indicar que empieza.
DigitalOut stepOfConversion(LED2);  // Se prende cada vez que se completa uno de los doce pasos.
DigitalOut endOfConversion(LED3);  // Se prende solamente una vez al final para indicar que terminó.

DigitalIn nextStepButton(Button1);  // Con este botón vamos haciendo la pausa entre cada paso (12 pasos)

AnalogIn potentiometer(A0); // Esto se usar para ingresar las lecturas de analogInput

float analogInput; //Ver por favor la figura del Under the hood para entender qué es esta señal
float digitalOutputScaledIntoRange0to1 = 0; //Esto lo usamos para dar la salida en el rando 0 a 1

int conversionStep = 0;  // Con esta variable llevaremos la cuenta del paso en que vamos (0 a 11)

bool comparatorOutput = 0;   // Ver por favor la figura del Under the hood para entender qué es esta señal
bool DACInput[NUMBER_OF_BITS]; //Acá vamos a ir guardando los 12 bits de la conversión (ver la fig.)
bool digitalOutput[NUMBER_OF_BITS]; //Este es el resultado de la convesión (ver la fig.)

bool analogComparator (input1, input2)  // Esta función representa al bloque Analog Comparator (ver fig.)

float digitalToAnalogConverter (array of bool); // Esta función representa al bloque DAC (ver fig.)

void resetIterativeConversionController (array of bool) // Esta función se usa al inicio para resetar DACInput

void showConversionStatus (conversionStep, analogInput, dacOutput, dacInput)
// Esta función se usa para ir mostrando el estado de la conversión en cada paso.

void main () // Lo que está acá adentro se debería repetir indefinidamente.
{

    startOfConversion (); // Esta función espera a que se presione el botón de nextStepButton
    // Puede poner en la PC un cartel alusivo a que está esperando
    // Cuando se presiona el botón debería hacer analogInput = potentiometer.read();
    // Debería encender por un segundo el LED startOfConversion

    resetIterativeConversionController ();  // Se resetea cada bit, se enciende 1 seg startOfConversion,
    //se envía un showConversionStatus a la PC.

    for (conversionStep = 1 to <= NUMBER_OF_BITS) {

        DACInput [NUMBER_OF_BITS - conversionStep]  =
            iterativeConversionControllerStep (conversionStep, comparatorOutput, DACOutput);

        DACOutput = digitalToAnalogConverter(DACInput); //falta escribirla

        comparatorOutput = analogComparator (analogInput, DACOutput); //falta escribirla

        showConversionStatus (conversionStep, analogInput, dacOutput, dacInput);

    }

    endOfConversion     // Esta función avisa con un cartel en la PC que se terminó la conversión
    // Ese cartel debería indicar expresamente el valor de digitalOutput
    // Debería mostrar además digitalOutputScaledIntoRange0to1 (que sería entre 0 y 1)
    // También debería encender por un segundo el LED endOfConversion
    // Debería dejar comparatorOutput = 0 así ya empieza bien la próxima vez.

}


// Declaración de las funciones

showConversionStatus (conversionStep, analogInput, dacOutput, dacInput)
{
    printf (conversionStep, analogInput, dacOutput, dacInput)
    delay (1000);
    while (!nextStepButton);
}

bool analogComparator (analogInput1, analogInput2)
{
    //Completa Pablin
}

bool iterativeConversionControllerStep (currentConversionStep,comparisonResult, DACStatus)
{
    if (comparatorOutput = 1) {
        DACOutput[i] = 1;
    } else {
        DACOutput[i] = 0;
    }
}
