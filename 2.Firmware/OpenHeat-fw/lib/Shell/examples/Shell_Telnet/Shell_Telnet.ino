/**
 * GeekFactory - "Construye tu propia tecnologia"
 * Distribucion de materiales para el desarrollo e innovacion tecnologica
 * www.geekfactory.mx
 *
 * Ejemplo de libreria Shell. Este ejemplo permite controlar el color mostrado
 * en un led RGB conectado a los pines PWM mediante comandos de texto. El programa
 * admite 3 parametros de 0 a 255 que representan la intensidad de cada led (RGB).
 * Esta version del programa permite el control remoto a traves de un shield Ethernet
 * y demuestra la capacidad de la libreria para redirigir la entrada y salida de datos
 * desde y hacia cualquier medio.
 *
 * Example for Shell library. This example controls the color displayed on the RGB
 * led connected to PWM pins using text commands. The program accepts 3 parameters
 * in a range from 0 to 255 that represent the intensity of each led.
 * This version of the program allows the remote control through the Ethernet Shield
 * and shows the capacity of the library to redirect data input and output to any
 * physical medium.
 */

#include <Shell.h>
#include <SPI.h>
#include <Ethernet.h>

// Ethernet and TCP/IP protocol configuration
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  

byte ip[] = { 
  192, 168, 1, 177 };    

byte gateway[] = { 
  192, 168, 1, 254 };

byte subnet[] = { 
  255, 255, 255, 0 };

// Create the server instance on port 23 (default port for telnet protocol)
EthernetServer server = EthernetServer(23);
// The client that is willing to connect to server
EthernetClient client;


void setup()
{
  // Prepare serial communication
  Serial.begin(9600);
  // Setup ethernet shield
  Ethernet.begin(mac, ip, gateway, subnet);
  // Start TCP Server on port 23
  server.begin();
  // Wait after reset or power on...
  delay(1000);

  // Pins to connect RGB led (PWM outputs)
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);

  // Initialize command line interface (CLI)
  // We pass the function pointers to the read and write functions that we implement below
  // We can also pass a char pointer to display a custom start message
  shell_init(shell_reader, shell_writer, 0);

  // Add commands to the shell
  shell_register(command_rgbled, PSTR("rgbled"));

}

void loop()
{
  // Check if a client is willing to connect and get client object
  client = server.available();
  // This should always be called to process user input
  shell_task();
}

/**
 * This function implements the rgbled command and parses the required args
 * Type: rgbled 255 255 255 to set led to maximum brigtness
 * Type: rgbled 0 0 0 to set led to minimum brightess
 */
int command_rgbled(int argc, char** argv)
{
  // Variables that store parameter values
  uint8_t val_R, val_G, val_B;

  // Parse RGB values and write to PWM pins
  val_R = strtol(argv[1],NULL,0);
  analogWrite(3, val_R);

  val_G = strtol(argv[2],NULL,0);
  analogWrite(5, val_G);

  val_B = strtol(argv[3],NULL,0);
  analogWrite(6, val_B);

  // Print the received falues to the terminal screen
  shell_printf("#RGB-COMPONENTS:%d,%d,%d\r\n", val_R, val_G, val_B);

  // Return success code
  return SHELL_RET_SUCCESS;
}

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
int shell_reader(char * data)
{
  int ret = 0;
  // If a client is available and it´s connected
  if (client==true && client.connected()) {
    // Client has data?
    if (client.available()) {
      // Read data from client
      *data = client.read();
      ret = 1;
    }
  } 
  else
  {
    client.stop();
  }
  return ret;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data)
{
  // If we can write to a client
  if(client==true && client.connected())
  {
    // Wrapper for Serial.write() method
    client.write(data);
  }
}


