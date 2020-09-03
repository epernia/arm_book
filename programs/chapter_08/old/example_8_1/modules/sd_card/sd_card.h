//=====[#include guards - begin]===============================================

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

//=====[Libraries]=============================================================
/*

{
    "target_overrides": {
        "*": {
            "platform.stdio-convert-newlines": 1,
            "target.features_add": ["STORAGE"],
            "target.components_add": ["SD"],
            "sd.SPI_MOSI": "D11",
            "sd.SPI_MISO": "D12",
            "sd.SPI_CLK": "D13",
            "sd.SPI_CS": "D7"
        }
    }
}

{
    "target_overrides": {
        "*": {
            "platform.stdio-convert-newlines": 1,
            "target.features_add": ["STORAGE"],
            "target.components_add": ["SD"],
            "sd.SPI_MOSI": "PB_5",
            "sd.SPI_MISO": "PB_4",
            "sd.SPI_CLK": "PB_3",
            "sd.SPI_CS": "D9"
        }
    }
}

{
    "target_overrides": {
        "*": {
            "platform.stdio-convert-newlines": 1,
            "target.features_add": ["STORAGE"],
            "target.components_add": ["SD"],
        }
    }
}
 */


//=====[Declaration of public defines]=======================================

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void sdMount();
void sdWrite();
void sdRead();
void sdDir();

//=====[#include guards - end]=================================================

#endif // _SD_CARD_H_
