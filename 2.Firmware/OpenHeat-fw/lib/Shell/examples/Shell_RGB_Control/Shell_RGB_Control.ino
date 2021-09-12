/**
 * GeekFactory - "Construye tu propia tecnologia"
 * Distribucion de materiales para el desarrollo e innovacion tecnologica
 * www.geekfactory.mx
 *
 * Ejemplo de libreria Shell. Este ejemplo permite controlar el color mostrado
 * en un led RGB conectado a los pines PWM mediante comandos de texto. El programa
 * admite 3 parametros de 0 a 255 que representan la intensidad de cada led (RGB).
 *
 * Example for Shell library. This example controls the color displayed on the RGB
 * led connected to PWM pins using text commands. The program accepts 3 parameters
 * in a range from 0 to 255 that represent the intensity of each led.
 */

#include <Shell.h>

void setup()
{
  // Prepare serial communication
  Serial.begin(9600);
  // Wait after reset or power on...
  delay(1000);

  // Pins to connect RGB led (PWM outputs)
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);

  // Initialize command line interface (CLI)
  // We pass the function pointers to the read and write functions that we implement below
  // We can also pass a char pointer to display a custom start message
  shell_init(shell_reader, shell_writer, 0);

  // Add commands to the shell
  shell_register(command_rgbled, PSTR("rgbled"));
}

void loop()
{
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
  
  val_B = strtol(argv[4],NULL,0);
  analogWrite(9, val_B);

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
  // Wrapper for Serial.read() method
  if (Serial.available()) {
    *data = Serial.read();
    return 1;
  }
  return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data)
{
  // Wrapper for Serial.write() method
  Serial.write(data);
}


