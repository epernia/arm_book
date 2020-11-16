Cambios:
--------

alarmLed --> sirenLed


Examples Ch 10
    Antes se usa en modo single en la section. 
    El pir Queda 3 segundos "muerto"
    luego de que termina el pulso de detección.

    Example 10-1 Cuando detecto movimiento, activo la alarma.
        intruder_alarm.cpp
        pir.cpp --> Cambiar a motion_sensor.cpp
                 Se agrega pir.cpp/.h (interrupciones) y se agrega al eventLog, 
                 manda por UART: PIR_DET_ON, PIR_DET_OFF.                 
                 Se setea muktiple trigger y pone bien angosto el pulso para
                 que el tiempo del pulso refleje el tiempo que detecta intruso.

        bool motionSensorRead();

    Example 10-2 Implementar un porton electrico automatico que puedo abrirlo,
                 cerrarlo, o pedirle el estado: abriendo, cerrando, abierto o cerrado.
                 Funciona con un motor y pines por interrupción.
        Motor, 4 botones
        DIR1 --> DIR_L y DIR2 --> DIR_R direccion
        
        contacto es sentido de giro
        contacto on-off del motor
        
        B3 y B4 finales de carrera (hablar de switches fin de carrera, podemos dar ejemplo de I3D)
        
        Separar driver de motor de driver de de fines de carrera en 2.
            
        gate.cpp (portón automático del garage) --> Usa al motor.cpp
        Ver si el nombre del módulo es uno de los siguientes: 
            gate
            automaticGate
            automaticDoor
            garageDoor

            typedef enum {
               GATE_CLOSE,
               GATE_OPEN,
               GATE_OPENING,
               GATE_CLOSING,
            } GateStatus_t;

            static bool gateOpenLimitSwitchRead();
            static bool gateCloseLimitSwitchRead();

            void gateOpen();  // Orden de abrir la puerta
            void gateClose(); // Orden de cerrar la puerta

            void gateLock();   // Orden de no dejar abrir o cerrar
            void gateUnlock(); // Orden de dejar abrir o cerrar

            GateStatus_t gateStatus(); // Leeer el estado actual del porton

        motor.cpp

            typedef enum {
               MOTOR_CLOCK_WISE         = 0, // Sentido por defecto al arrancar
               MOTOR_COUNTER_CLOCK_WISE = 1,
            } MotorSpinDirection_t;

            void motorInit();

            void motorPower( bool_t enable ); // Arranca o para el motor

            MotorSpinDirection_t motorSpinDirectionRead(); // Leer sentido de giro actual
            void motorSpinDirectionWrite( MotorSpinDirection_t spinDirection ); // Setear sentido de giro

            void motorSpinDirectionToggle(); // Opcional

        user_interface.cpp
            Van aca los botones de activar y desactivar el porton automatico.

        Agregar que la UART indique el estado del porton por peticion 
        (como comando en pc_serial_com.cpp, no lo vamosa a gregar al event log).

        Agregar dibujos de los fines de carreras y porton automatico de garage.

    Example 10-3 al detectar el pir se manda a cerrar la puerta
        intruderAlarm.cpp
        Ver nombre:
           - intruderAlarm, u otros?

    Example 10-4 
    
Examples nuevos:

  Subsistema porton automático (más adelante pensar si se abre por BLE desde una app...):
    - Example 10-1: Usar un motor con un boton de activar mientras este pulsado
      y otro paracambiar el sentido de giro cada vez que tocas (sin interrupciones):
       - 1 Motor (no usa interrupciones, usa reles).
    - Example 10-2: Porton automatico con boton de abrir y cerrar:
       - 1 Motor (no usa interrupciones, usa reles).
       - 2 Fin de carrera (usa interrupciones, aca las explicamos).

  Subsistema de alarma antirrobo.
    - Example 10-3: Sensor de movimiento con PIR (usa interrupciones) y
       loguear si hay alguien o no en la habitacion. 
    - Example 10-4: Alarma de robo con PIR (usa interrupciones). 
       - Suena la alarma al detectar movimiento.
       - Acá hay 2 módulos que comparten el recurso sirena, code y display para
         desactivar la alarma.
       - Explicar en el example cual es el problema de tener recursos
         compartidos y ver que cambios surgen.

Conceptos a explicar en CH10:
   - Motor.
   - Relé.
   - Puente H con 2 relés.
   - Fin de carrera.
   - Interrupciones.
   - Introducir otro subsistema que no compite por recursos con el fire_alarm.

   - PIR.
   - Introducir otro subsistema que si compite por recursos con el fire_alarm.
      - Aca se introduce un nuevo modulo "alarm" que hace la OR entrre las diferentes fuentes de alarma.
      - Este modulo alarm lo maneja el modulo "smart_home_system".
      - Un solo code de alarmas desactive todo.
      - armed o dissarmed de la alarma de intruder.
      - Ahora cambian los codigos de titileo de la alarma.
        - para fire_alarm
        - para intruder_alarm
        - ambos a la vez

Conceptos a explicar en CH11:
   - Timer/Counter y sus modos de funcionamiento.
        - Conteo de pulsos hasta overflow del contador.
        - Conteo de pulsos hasta Match ontra un valor de comparación.
        - Generación de señales con timers:
           - PWM.
        - Entrada de captura de pulsos para guardar el estado del contador timer cuando pasa un pulso en un pin.
           - Medir Frecuencia.
           - Medir duty cycle.
   - PWM.
      - Con delay no bloqueante de sAPI.
      - Con Ticker y ticker.attach() (usa timers por debajo, en modo comparacion e interrupción).
        - Probar mas de un ticker en el sistema, suponemos que andan.
      - Con PWM out (usa timers en modo PWM).
   - Dimerizado de luces de una casa con PWM.
   - Como funciona un LED RGB. Que son como 3 leds separadaos puestos fisicamente muy cerca.
   - Como se forman los colores luz sumando componentes R, G y B.

