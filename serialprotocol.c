// Fichero con funciones para la gestion y creacion de tramas segun el protocolo de transmision de datos serializados
// explicado en clase
#include<stdint.h>
#include<stdbool.h>

#include "crc.h"  // Algoritmo de checksum (CRC)
#include "serialprotocol.h"
#include <stdlib.h>
#include <string.h>

//Funcion que realiza el stuffing en una trama.
static int32_t frame_stuffing(uint8_t *origen, uint8_t  *destino,int32_t longitud, int32_t longitud_maxima)
{
    int32_t i,j;
    uint8_t tmp;

    for (i=0,j=0;((i<longitud)&&(j<longitud_maxima));i++,j++)
    {
        tmp=*origen;
        origen++;
        if ((START_FRAME_CHAR==tmp)||(STOP_FRAME_CHAR==tmp)||(ESCAPE_CHAR==tmp))
        {
            *destino=ESCAPE_CHAR;
            destino++;
            j++;
            *destino=tmp^STUFFING_MASK;
            destino++;
        }
        else
        {
            *destino=tmp;
            destino++;
        }
    }


    if (j>longitud_maxima)
        return PROT_ERROR_STUFFED_FRAME_TOO_LONG;
    else
        return j;
}

//Funcion para hacer el "destuffing" de una trama recibida, antes de pasar a procesarla.
static int32_t frame_destuffing(uint8_t *frame,int32_t longitud)
{
    int32_t i,numStuffedBytes;
    uint8_t *auxptr;
    uint8_t escape_seq_detected=0;

    //No hace falta duplicar, se trabaja sobre el propio array, ya que se van eliminando posiciones.
    auxptr=frame;

    for (i=0,numStuffedBytes=0;i<longitud;i++)
    {
        if (*auxptr==ESCAPE_CHAR)
        {
            auxptr++;
            i++;	//Se incremente i
            numStuffedBytes++;
            if ((*auxptr)!=ESCAPE_CHAR)
            {
                *frame=(*auxptr)^STUFFING_MASK;	//Hace el XoR si el siguiente no es tambien ESCAPE_CHAR
                frame++;
                auxptr++;
            }
            else{
                auxptr++;
                escape_seq_detected++;
                //Aqu ira el codigo para tratar una secuencia de escape en caso de querer hacerlo
            }
        }
        else
        {
            *frame=(*auxptr);
            frame++;
            auxptr++;
        }
    }

    longitud-=numStuffedBytes;

    return longitud;
}


//Funcion para calcular el checksum y hacer "stuffing" de un paquete para su envio
//Ojo Asume que le cabe el offset
static int32_t add_checksum_and_stuff(uint8_t *frame,int32_t size, int32_t max_size)
{
    uint16_t checksum;
    uint8_t *ptrtochar;

    checksum=create_checksum(frame,size);	//Calcula el checksum del paquete antes del Stuffing
    frame[size]=(uint8_t)(checksum&0x0FF);
    frame[size+1]=(uint8_t)(checksum>>8);
    size+=CHECKSUM_SIZE;	//Añade el tamaño del checksum

    //Hace una copia temporal del paquete
    ptrtochar=(uint8_t *)malloc(max_size);
    if (!ptrtochar)
    {
        return PROT_ERROR_NOMEM;
    }
    memcpy(ptrtochar,(void *)frame,size);

    size=frame_stuffing(ptrtochar, frame,size, max_size);	//Hace el stuffing
    free(ptrtochar);

    return (size);
}

//Destuffing y chequeo del checksum en un paquete recibido

int32_t destuff_and_check_checksum (uint8_t *frame, int32_t max_size)
{
    uint16_t checksum,checksum_calc;
    int32_t size;


    size=frame_destuffing(frame,max_size);

    checksum_calc=create_checksum(frame,size-CHECKSUM_SIZE);	//Calcula el checksum del paquete despues del deStuffing
    checksum=(((uint16_t)frame[size-(CHECKSUM_SIZE-1)])<<8)|((unsigned short)frame[size-CHECKSUM_SIZE]);

    if (checksum!=checksum_calc)
        return PROT_ERROR_BAD_CHECKSUM;
    else
        return (size);
}


//Funcion que crea un comando y lo introduce en una trama
int32_t create_frame(uint8_t *frame,uint8_t command_type, void * param, int32_t param_size, int32_t max_size)
{
    int32_t min_size;
    uint8_t *auxptr;
    int32_t size;

    auxptr=frame;

    *auxptr=START_FRAME_CHAR;
    auxptr++;


    min_size=(MINIMUN_FRAME_SIZE+param_size);	//1 START  +1 COMANDO + 2 CHECKSUM +1 STOP

    if (min_size>=max_size)
    {
        return PROT_ERROR_COMMAND_TOO_LONG;
    }

    *auxptr=command_type;
    auxptr++;

    if (param_size>0)
    {
        memcpy(auxptr,param,param_size);	//Copia los argumentos
        auxptr+=param_size;
    }

    size=add_checksum_and_stuff((frame+START_SIZE),(min_size-(START_SIZE+END_SIZE+CHECKSUM_SIZE)),max_size-(START_SIZE+END_SIZE));	//No se aplica el chesksum y el stuff a los caracteres de inicio y fin
    auxptr=(frame+size+START_SIZE);
    *auxptr=STOP_FRAME_CHAR;

    return (size+START_SIZE+END_SIZE);
}


//Esta función obtiene el campo "tipo comando" de la trama
uint8_t decode_command_type(uint8_t * buffer)
{
    return buffer[0];
}

//Esta función extrae el parametro de la trama y comprueba que el tamaño sea correcto
//ptrtoparam es el puntero a la zona de memoria donde esta el parametro
//param_size es su tamaño
//param es una estructura por REFERENCIA
//payload es el tamaño de la estructura que se pasa por REFERENCIA
//Devuelve: Tamaño del parametro recibido o un valor negativo si hay error
int32_t check_and_extract_command_param(void *ptrtoparam, int32_t param_size, uint32_t payload,void *param)
{
    if (payload==param_size)
    {
        memcpy(param,ptrtoparam,payload);
        return payload;
    }
    else
    {
        return PROT_ERROR_INCORRECT_PARAM_SIZE;
    }
}


//Esta función obtiene un puntero a la zona de memoria donde está el parámetro dentro de la trama
//buffer es la zona de memoria donde esta almacenada la trama "Desestufada"
//frame_size es su tamaño
//campo es un puntero a void que se pasa por REFERENCIA. Quedará apuntando a la zona de memoria donde está el parametro
//Devuelve: Tamaño del parametro recibido o un valor negativo si hay error
int32_t get_command_param_pointer(uint8_t * buffer, int32_t frame_size, void **campo)
{
    int32_t param_size=frame_size-COMMAND_SIZE-CHECKSUM_SIZE;

    *campo=buffer+COMMAND_SIZE;
    if (param_size<0)
        return PROT_ERROR_BAD_SIZE; //Devuelve un codigo de error
    else
        return param_size;
}

