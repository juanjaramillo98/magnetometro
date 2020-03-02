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
int16_t valorEstable;
int16_t valorEstableOcupado;
int16_t margen = 25;
int16_t ultimosValores[5];
int16_t recorrido = 0;
int16_t magnitud2 = 0;
bool estado;
uint8_t imprecion;

bool salida;
void setup() {
    
    Serial.begin(9600);
    pinMode(0,OUTPUT);
    digitalWrite(0,LOW);
    Serial.println("Inicializando Magnetometro...");
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
    //Inicializamos la comunicación I2C y el magnetómetro
    Wire.begin();
    magnetometro.initialize();
    delay(5000);
    valorEstable = magnetometro.getHeadingZ();
    for (size_t i = 0; i < sizeof(ultimosValores) - 1; i++)
    {
        ultimosValores[i] = valorEstable;
    }
    
    
}

int promedio(int16_t a[]){
    int suma;
    for (size_t i = 0; i < sizeof(a); i++)
    {
        suma += a[i];
    }
    return suma/sizeof(a);
}

void loop() {
    //Obtenemos del magnetometro las componentes del campo magnético
    magnetometro.getHeading(&mx, &my, &mz);
    int16_t magnitud = sqrt( pow(mx,2) + pow(my,2) + pow(mz,2) );
    //Serial.println(magnitud);
    if (SerialBT.available())
    {      
        imprecion = SerialBT.read();
        Serial.println(imprecion);
    }
    switch (imprecion)
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
    ultimosValores[recorrido] = mz;
    recorrido = (recorrido+1)%sizeof(ultimosValores);

    if ((promedio(ultimosValores) < valorEstable-margen) && !estado)
    {
        delay(2000);
        if (magnetometro.getHeadingZ < valorEstable-margen)
        {
            estado = true;
            digitalWrite(0,HIGH);
            delay(2000);
            valorEstableOcupado = magnetometro.getHeadingZ;

        }
        
    }
    if (estado &&  (promedio(ultimosValores) > valorEstableOcupado + margen))
    {
        delay(1000);
        if ((magnetometro.getHeadingZ < valorEstable+margen) && (magnetometro.getHeadingZ > valorEstable-margen))
        {
            estado = false;
            digitalWrite(0,LOW);
        }
        
    }
    
    






}
