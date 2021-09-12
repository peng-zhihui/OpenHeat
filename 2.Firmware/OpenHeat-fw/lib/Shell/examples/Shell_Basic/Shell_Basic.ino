/**
 * GeekFactory - "Construye tu propia tecnologia"
 * Distribucion de materiales para el desarrollo e innovacion tecnologica
 * www.geekfactory.mx
 *
 * Ejemplo de libreria Shell. Este ejemplo representa la configuracion minima
 * requerida para implementar la funcionalidad de interfaz de comandos. En esta
 * ocasion registramos 2 comandos y enviamos texto como respuesta a cada uno.
 *
 * Example for Shell library. This example shows the minimum setup needed to
 * implement a command line interface. This time we register 2 commands and we
 * send text as response for each command.
 */

#include <Shell.h>

void setup()
{
  // Prepare serial communication
  Serial.begin(9600);
  // Wait after reset or power on...
  delay(1000);

  // Initialize command line interface (CLI)
  // We pass the function pointers to the read and write functions that we implement below
  // We can also pass a char pointer to display a custom start message
  shell_init(shell_reader, shell_writer, 0);

  // Add commands to the shell
  shell_register(command_mycommand, PSTR("mycommand"));
  shell_register(command_othercommand, PSTR("othercommand"));
}

void loop()
{
  // This should always be called to process user input
  shell_task();
}

/**
 * Test commands: The commands should always use this function prototype.
 * They receive 2 parameters: The total count of arguments (argc) and a pointer 
 * to the begining of each one of the null-terminated argument strings.
 *
 * In this example we ignore the parameters passed to the functions
 */
int command_mycommand(int argc, char** argv)
{
  shell_println("Running \"mycommand\" now");
  shell_println("Exit...");
  return SHELL_RET_SUCCESS;
}

int command_othercommand(int argc, char** argv)
{
  shell_println("Running \"othercommand\" now");
  shell_println("Exit...");
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
