#include <Arduino.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "HMC5883L.h"
#include "math.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;
HMC5883L magnetometro;
int16_t mx, my, mz;
int16_t magnitud2 = 0;
uint8_t estado;
bool salida;
void setup() {
    
    Serial.begin(9600);
    Serial.println("Inicializando Magnetometro...");
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
    //Inicializamos la comunicación I2C y el magnetómetro
    Wire.begin();
    magnetometro.initialize();
}

void loop() {
    //Obtenemos del magnetometro las componentes del campo magnético
    magnetometro.getHeading(&mx, &my, &mz);
    int16_t magnitud = sqrt( pow(mx,2) + pow(my,2) + pow(mz,2) );
    /*
    Serial.print("mx:");
    Serial.print(mx); 
    Serial.print("\tmy:");
    Serial.print(my);
    Serial.print("\tmz:");
    Serial.print(mz);
    Serial.print("\tmagnitud: ");
    */
    
    //Serial.println(magnitud);
    if (SerialBT.available())
    {      
        estado = SerialBT.read();
        Serial.println(estado);
    }
    switch (estado)
    {
    case 100:
        Serial.println(magnitud);
        SerialBT.println(magnitud);
        break;

    case 97:
        Serial.println(magnitud-magnitud2);
        SerialBT.println(magnitud-magnitud2);
        break;

    case 120:
        Serial.print(mx);
        SerialBT.println(mx);
        break;
    case 121:
        Serial.print(my);
        SerialBT.println(my);
        break;
    case 122:
        Serial.print(mz);
        SerialBT.println(mz);
        break;
    default:
        break;
    }
    magnitud2=magnitud;
    delay(300);

}
