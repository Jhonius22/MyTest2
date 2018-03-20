
/*--------------------------------------------------------------
  Program:      PZEM-004 & ESP8266 12E Modbus TCP/IP Slave

  Description:  Connection and communication Serial power meter Active PZEM-004 Peacefair
                with Module ESP8266 per port Serial0 and visualization in matrix led 

                Conexion y comunicacion Serial  medidor de potencia Activa PZEM-004 
                Peacefair  con Modulo ESP8266 por puerto Serial0 y visualizacion en matrix led 
  
  Hardware:     ESP8266 12E NodeMCU Lolin .
                Matrix led X4 MAX7219
                PZEM-004
                
  Software:     Arduino IDE v1.8.3
  
  Date:         19 March 2018
   
  Modified or Created:       PDAControl   http://pdacontroles.com   http://pdacontrolen.com

  Complete Tutorial English:
  
  Tutorial Completo Español

  Based: MAX7219_Dot_Matrix library for Nick Gammon          https://github.com/nickgammon/MAX7219_Dot_Matrix

         PZEM004T library for olehs                          https://github.com/olehs/PZEM004T

         SoftwareSerial for esp8266                          https://github.com/plerup/espsoftwareserial

 
--------------------------------------------------------------*/


///#include <WiFi.h>

#include <ESP8266WiFi.h>


#include <Ticker.h>    ///  https://github.com/esp8266/Arduino/tree/master/libraries/Ticker

const char* ssid       = "1503523";
const char* password   = "D2E7D32DBC883";
int ModbusTCP_port     = 502;
unsigned long timeout_connection= 5000;

////////  Required for Modbus TCP / IP /// Requerido para Modbus TCP/IP  /////////
#define maxInputRegister    20
#define maxHoldingRegister    20

#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3              //implemented
#define MB_FC_WRITE_REGISTER 6              //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16   //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID          0
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7
#define MB_TCP_REGISTER_START         8
#define MB_TCP_REGISTER_NUMBER         10

byte ByteArray[260];
unsigned int  MBHoldingRegister[maxHoldingRegister];

//////////////////////////////////////////////////////////////////////////

WiFiServer MBServer(ModbusTCP_port);

///https://www.prosoft-technology.com/kb/assets/intro_modbustcp.pdf
///http://www.modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf
///http://esp8266.github.io/Arduino/versions/2.0.0/doc/libraries.html


#define led_control  12  //D6    //led verde // green led   ----




Ticker event; /////LED Control

void setup() {

 pinMode(led_control, OUTPUT);    
 Serial.begin(115200);

 delay(100) ;
 WiFi.begin(ssid, password);
 delay(100) ;
 Serial.println(".");
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  MBServer.begin();
  Serial.println("Connected ");
  Serial.print("ESP8266 Slave Modbus TCP/IP ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(String(ModbusTCP_port));
  Serial.println("Modbus TCP/IP Online");
  
}

void loop() {

  // Check if a client has connected  // Modbus TCP/IP
  WiFiClient client = MBServer.available();
  if (!client) {
   return;
  }
 

    boolean flagClientConnected = 0;
    byte byteFN = MB_FC_NONE;
    int Start;
    int WordDataLength;
    int ByteDataLength;
    int MessageLength;
    
    // Modbus TCP/IP
    while (client.connected()) {

     /////////////////// timeout 5 sec - sin respuesta de servidor.. //////////////////////
                 unsigned long timeout = millis();
                while (client.available() == 0) {
                  if (millis() - timeout > timeout_connection) {
                    event.attach(0.3, ledcontrol);   
                   
                    Serial.println(">>> Client Timeout !");
                    client.stop();
                    return;
                  }
                   digitalWrite(led_control, LOW);
                 ///  Serial.println(">>> Client Connect !");
                   }
                   
      ////////////////////////////////////////////////////////////////////////////////////            

                  
    if(client.available())
        
    {
        flagClientConnected = 1;
        int i = 0;
        while(client.available())
        {
            ByteArray[i] = client.read();
            i++;
        }
         client.flush();    
       

          ///////// Holding Register [0]  A [9]   = 10 Holding Registers Escritura
          ///////// Holding Register [0]  A [9]   = 10 Holding Registers Writing
                  
                  MBHoldingRegister[0] = random(0,12);
                  MBHoldingRegister[1] = analogRead(A0);
                  MBHoldingRegister[2] = random(0,12);
                  MBHoldingRegister[3] = random(0,12);
                  MBHoldingRegister[4] = random(0,12);
                  MBHoldingRegister[5] = random(0,12);
                  MBHoldingRegister[6] = random(0,12);
                  MBHoldingRegister[7] = random(0,12);
                  MBHoldingRegister[8] = random(0,12);
                  MBHoldingRegister[9] = random(0,12);     

                
       
           
   
          




///////// Holding Register [10]  A [19]  =  10 Holding  Registers Lectura ///////////////
/////     Holding Register [10] A [19]   =  10 Holding Registers Reading 
        
              
                
                int Temporal[10];
                
                Temporal[0] = MBHoldingRegister[10];                
                Temporal[1] = MBHoldingRegister[11];                    
                Temporal[2] = MBHoldingRegister[12];
                Temporal[3] = MBHoldingRegister[13];
                Temporal[4] = MBHoldingRegister[14];
                Temporal[5] = MBHoldingRegister[15];
                Temporal[6] = MBHoldingRegister[16];
                Temporal[7] = MBHoldingRegister[17];
                Temporal[8] = MBHoldingRegister[18];
                Temporal[9] = MBHoldingRegister[19];


                  


           for (int i = 0; i < 10; i++) {
 
             Serial.print("[");
             Serial.print(i);
             Serial.print("] ");
             Serial.print(Temporal[i]);
             
             }
             Serial.println("");        

              

        ////  rutine Modbus TCP
        byteFN = ByteArray[MB_TCP_FUNC];
        Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
        WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
     }
    
    // Handle request

    switch(byteFN) {
        case MB_FC_NONE:
            break;
            
          
        
        case MB_FC_READ_REGISTERS:  // 03 Read Holding Registers
            ByteDataLength = WordDataLength * 2;
            ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
            ByteArray[8] = ByteDataLength;     //Number of bytes after this one (or number of bytes of data).
            for(int i = 0; i < WordDataLength; i++)
            {
                ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
                ByteArray[10 + i * 2] =  lowByte(MBHoldingRegister[Start + i]);
            }
            MessageLength = ByteDataLength + 9;
            client.write((const uint8_t *)ByteArray,MessageLength);
      
            byteFN = MB_FC_NONE;
   
            break;
            
             
        case MB_FC_WRITE_REGISTER:  // 06 Write Holding Register
            MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
            ByteArray[5] = 6; //Number of bytes after this one.
            MessageLength = 12;
            client.write((const uint8_t *)ByteArray,MessageLength);
            byteFN = MB_FC_NONE;
            break;
            
        case MB_FC_WRITE_MULTIPLE_REGISTERS:    //16 Write Holding Registers
            ByteDataLength = WordDataLength * 2;
            ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
            for(int i = 0; i < WordDataLength; i++)
            {
                MBHoldingRegister[Start + i] =  word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
            }
            MessageLength = 12;
            client.write((const uint8_t *)ByteArray,MessageLength);    
            byteFN = MB_FC_NONE;
     
            break;
    }
    }


    

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////blinked led control ////////////////
void ledcontrol () {
   int state = digitalRead(led_control);  // get the current state of GPIO1 pin
  digitalWrite(led_control, !state);     // set pin to the opposite state          

}

