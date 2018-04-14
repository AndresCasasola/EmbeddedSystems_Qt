#ifndef QTIVARPC_H
#define QTIVARPC_H

#include <QObject>
#include <QSerialPort>      // Comunicacion por el puerto serie
#include <QSerialPortInfo>  // Comunicacion por el puerto serie


#include<stdint.h>      // Cabecera para usar tipos de enteros con tamaño
#include<stdbool.h>     // Cabecera para usar booleanos




class QTivaRPC : public QObject
{

    Q_OBJECT
public:
    explicit QTivaRPC(QObject *parent = 0);

    //Define una serie de etiqueta para los errores y estados notificados por la señal statusChanged(...)
    enum TivaStatus {TivaConnected,
                     TivaDisconnected,
                     OpenPortError,
                     BaudRateError,
                     DataBitError,
                     ParityError,
                     StopError,
                     FlowControlError,
                     UnexpectedPacketError,
                     FragmentedPacketError,
                     CRCorStuffError
                    };
    Q_ENUM(TivaStatus)

    //Metodo publico
    QString getLastErrorMessage();

signals:
    void statusChanged(int status, QString message); //Esta señal se genera al realizar la conexión/desconexion o cuando se produce un error de comunicacion
    void pingReceivedFromTiva(void); //Esta señal se genera al recibir una respuesta de PING de la TIVA
    void commandRejectedFromTiva(int16_t code); //Esta señal se genera al recibir una respuesta de Comando Rechazado desde la TIVA
public slots:
    void startRPCClient(QString puerto); //Este Slot arranca la comunicacion
    void ping(void); //Este Slot provoca el envio del comando PING
    void LEDGpio(bool red, bool green, bool blue); //Este Slot provoca el envio del comando LED
    void LEDPwmBrightness(double value); //Este Slot provoca el envio del comando Brightness

private slots:
    void processIncommingSerialData(); //Este Slot se conecta a la señal readyRead(..) del puerto serie. Se encarga de procesar y decodificar los mensajes que llegan de la TIVA y
                        //generar señales para algunos de ellos.

private:
    QSerialPort serial;
    QString LastError;
    bool connected;
    QByteArray incommingDataBuffer;

};

#endif // QTIVARPC_H
