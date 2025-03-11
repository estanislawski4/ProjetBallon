#ifndef INTERFACE_H
#define INTERFACE_H

#include <QWidget>

namespace Ui {
class Interface;
}

class SerialPortManager;
class APRSISClient;
class AX25Converter;
class KISSHandler;
class WebSocketServer;
class MySQLManager;

class Interface : public QWidget
{
    Q_OBJECT

public:
    explicit Interface(QWidget *parent = nullptr);
    ~Interface();

private slots:
    void fillPortsComboBox();
    void onStartButtonClicked();
    void onSendButtonClicked();

private:
    Ui::Interface *ui;
    SerialPortManager *m_serialManager;
    APRSISClient     *m_aprsClient;
    AX25Converter    *m_converter;
    KISSHandler      *m_kissHandler;
    MySQLManager     *m_dbManager;
    QString buildLoRaFrame();
    QString buildAprsFrame();
    bool storeLoRaTrame(const QString &source, const QString &destination,
                        const QString &fullTrame, const QString &message);
};

#endif // INTERFACE_H
