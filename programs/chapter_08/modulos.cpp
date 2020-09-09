 EVENT LOG    user interface
    |          |
    v          v       
      sd CARD
      
      
En módulo sd CARD
      
    // En caso de error o no poder completar la opercion devuelven false
    // Si esta todo bien devuelven true

    // Inicializa el puerto SPI de la tarjeta SD.
    // Monta el sistema e archivos.
    bool sdCardInit();

    // En fileName va el String con nombre de archivo a leer desde la SD.
    // En el array readBuffer me graba el conteido del archivo a leer. Yo le paso el tamaño máximo de buffer de lectura en bytes en *readBufferSize y en la mimsma variable me devueve cuantos bytes ocupo del buffer al leer el archivo
    bool sdCardReadFile( char const * fileName, 
                        uint8_t* readBuffer,
                        int* readBufferSize );
                   
    // En fileName va el String con nombre de archivo a escribir en la SD.
    // En el array writeBuffer le paso los datos a grabar en la sd. 
    // Yo le paso el tamaño máximo de buffer de a copiar a la SD en bytes en *writeBufferSize y en la mimsma variable me devueve cuantos bytes pudo grabar en ese archivo
    bool sdCardWriteFile( char const * fileName, 
                         uint8_t* writeBuffer,
                         int* writeBufferSize );
    // Siempre abre for append

    // En el array fileNamesBuffer me graba los nombres de archivo que encuentra. Yo le paso el tamaño máximo de buffer en bytes en *fileNamesBufferSize y en la mimsma variable me devueve cuantos bytes ocupo del buffer
    bool sdCardListFiles( uint8_t* fileNamesBuffer,
                         int* fileNamesBufferSize );
     
En módulo EVENT LOG 
     
     bool eventLogSaveToSdCard()

    accion 1 - loguear evento ( eventLogUpdate(), lo guarda en RAM )

    accion 2 - grabas ese log en un lugar, por
    ahora los manda por uart USB y BLE


En módulo pc serial com

    // Usa a la SD para leer el archivo y lo muestra por uart
    commandShowSdCardFileContent( fileName )
    commandShowSdCardFileList()
    
En módulo userInterface

    userInterfaceReadBitmapFromSdCard()
    
    
     