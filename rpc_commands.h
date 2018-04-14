#ifndef RPCCOMMANDS_H
#define RPCCOMMANDS_H

//Códigos de los comandos y definicion de parametros para el protocolo RPC

// El estudiante debe añadir aqui cada nuevo comando que implemente. IMPORTANTE el orden de los comandos
// debe SER EL MISMO aqui, y en el codigo equivalente en la parte del microcontrolador (Code Composer)
typedef enum {
    COMMAND_REJECTED,
    COMMAND_PING,
    COMMAND_LED_GPIO,
    COMMAND_LED_PWM_BRIGHTNESS,
    //etc, etc...
} commandTypes;

//Estructuras relacionadas con los parametros de los comandos. El estuadiante debera crear las
// estructuras adecuadas a los comandos usados, y asegurarse de su compatibilidad en ambos extremos

#pragma pack(1)	//Cambia el alineamiento de datos en memoria a 1 byte.

typedef struct {
    uint8_t command;
} PARAMETERS_COMMAND_REJECTED;

typedef union{
    struct {
         uint8_t padding:1;
         uint8_t red:1;        
         uint8_t blue:1;
         uint8_t green:1;
    } leds;
    uint8_t value;
} PARAMETERS_LED_GPIO;

typedef struct {
    float rIntensity;
} PARAMETERS_LED_PWM_BRIGHTNESS;

#pragma pack()	//...Pero solo para los comandos que voy a intercambiar, no para el resto.

#endif // RPCCOMMANDS_H
