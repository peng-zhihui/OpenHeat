/**
 * GeekFactory - "Construye tu propia tecnologia"
 * Distribucion de materiales para el desarrollo e innovacion tecnologica
 * www.geekfactory.mx
 *
 * Ejemplo de libreria Shell. En este ejemplo se ilustra la recepcion y manejo
 * de los parametros recibidos por el comando. Se imprimen cada uno de los
 * argumentos recibidos así como la longitud de cada una de las cadenas.
 *
 * Example for Shell library. This example shows the reception and handling of
 * arguments received by command programs. The arguments are printed along with
 * the string lenghts.
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
  shell_register(command_test, PSTR("test"));
}

void loop()
{
  // This should always be called to process user input
  shell_task();
}

int command_test(int argc, char** argv)
{
  int i;

  shell_println("-----------------------------------------------");
  shell_println("SHELL DEBUG / TEST UTILITY");
  shell_println("-----------------------------------------------");
  shell_println("");
  shell_printf("Received %d arguments for test command\r\n",argc);

  // Print each argument with string lenghts
  for(i=0; i<argc; i++)
  {
    // Print formatted text to terminal
    shell_printf("%d - \"%s\" [len:%d]\r\n", i, argv[i], strlen(argv[i]) );
  }

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
