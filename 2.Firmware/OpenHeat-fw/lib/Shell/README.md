# GeekFactory Shell Library #

This library allows us to **control our devices with simple text commands**. The experience is similar to the terminal in Mac OS or linux and the Windows command line. The commands can be transmitted to the microcontroller using a serial port, a TCP/IP socket or a USB (CDC) connection.
**This library is written in pure C in order to allow it to work in other microcontrollers/compilers that do not support C++**.

## Basic library usage ##

In order to use the library we need to call the initialization function that will tell the library the functions that it shall use to read / write the communication media (serial, TCP/IP, etc.). The following code will prepare the library and instruct it to use shell_reader() function to read bytes from the serial port and shell_writter() function to write bytes on the serial port.

```c
// Initialize command line interface (CLI)
// We pass the function pointers to the read and write functions that we implement below
// We can also pass a char pointer to display a custom start message
shell_init(shell_reader, shell_writer, 0);
```

The next step is to register the commands. This will associate the functions that will run when the command name (string) is found in the user input.

```c
 // Add commands to the shell
shell_register(command_mycommand, "mycommand");
shell_register(command_othercommand, "othercommand");
```

Finally, inside the main loop of the application, the programmer should call the **shell_task()** function. Inside this function the library takes care of the character reception, string matching and also this function will call the appropriate command when needed.

```c
 // This should always be called to process user input
shell_task();
```

The programs (functions) that implement the functionality of each command should be written using the standard C prototype and receive their parameters in the exact same way it happens on a desktop PC when we write a command line tool.

```c
int command_func(int argc, char ** argv);
```

## Basic library example using arduino ##

```cpp
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
```

## Project Objectives ##

Our library should fulfill the following goals:

* Code should be portable to other platforms
* Library should be compact
* It should be easy to add new commands


## Supported devices ##

This library was developed/tested on the following boards:

* Arduino UNO R3
* Arduino Mega 2560 R3
* Basic testing on ESP8266 boards

The library was also tested / compiled on the following PIC microcontrollers using MPLAB X:

* PIC24FJ64GA002
* PIC24HJ128GA504

The library is meant to work in other CPU architectures where a C compiler is available, please tell us about your experience if you try it in other platforms.

## Contact me ##

* Feel free to write for any inquiry: ruben at geekfactory.mx
* Check our website: https://www.geekfactory.mx



# Librería Shell de GeekFactory #

Esta librería permite **controlar nuestros programas y dispositivos con simples comandos de texto** de manera similar a la terminal en Mac OSX o linux y la linea de comandos de Windows. Los comandos pueden transmitirse al microcontrolador mediante un puerto serie, un socket TCP/IP o una conexión USB CDC. **Esta librería está escrita en C puro para permitir que funcione en microcontroladores/compiladores que no soportan C++**.

## Uso básico de la librería ##

Para utilizar la librería debemos llamar a la función de inicialización que le dice a la librería que funciones debe usar para leer y escribir sobre el medio (serial, TCP/IP, etc.). El siguiente código preparará la librería para su uso y le indica que use las funciones shell_reader() para leer bytes desde el puerto serie y la función shell_writter() para escribir bytes al serial.

```c
// Initialize command line interface (CLI)
// We pass the function pointers to the read and write functions that we implement below
// We can also pass a char pointer to display a custom start message
shell_init(shell_reader, shell_writer, 0);
```

Luego, el programador debe registrar los comandos y las funciones a las que estará asociada cada una de las cadenas de texto válido.

```c
 // Add commands to the shell
shell_register(command_mycommand, "mycommand");
shell_register(command_othercommand, "othercommand");
```

Finalmente, dentro del ciclo principal de cada aplicación, el programador debe asegurarse de llamar a la función shell_task(), ya que dentro de esta se responde a los comandos introducidos y es la encargada de ejecutar las funciones que asociamos a cada cadena.

```c
 // This should always be called to process user input
shell_task();
```

Los programas (funciones) que implementan cada uno de los comandos se deben escribir con el prototipo estándar de C y reciben los parámetros para realizar su función de manera similar a una utilidad de linea de comandos programada para una PC.

```c
int command_func(int argc, char ** argv);
```

## Ejemplo básico de la librería utilizando arduino ##

```cpp
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
```

## Objetivos del proyecto ##

Nuestra librería debe cumplir los siguientes objetivos:

* El código debe ser fácilmente portable a otras plataformas
* La liobrería debe ser compacta
* Debe ser fácil agregar nuevos comandos


## Dispositivos soportados ##

La librería fue desarrollada/probada en las siguientes tarjetas:

* Arduino UNO R3
* Arduino Mega 2560 R3
* Pruebas básicas en tarjetas con ESP8266

La librería fue probada en los siguientes microcontroladores PIC usando MPLAB X:

* PIC24FJ64GA002
* PIC24HJ128GA504

La librería GeekFactory Shell esta diseñada para trabajar en otras arquitecturas de CPU donde exista un compilador de C, por favor comentanos sobre tu experiencia si la pruebas en otras plataformas.

## Contáctame ##

* Contacto para cualquier tema referente a esta librería: ruben en geekfactory.mx
* Revisa nuestro sitio web: https://www.geekfactory.mx
