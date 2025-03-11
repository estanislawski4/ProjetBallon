#ifndef AX25CONVERTER_H
#define AX25CONVERTER_H

// AX25Converter.h
#pragma once

#include <QObject>
#include <QByteArray>
#include <QString>

class AX25Converter : public QObject {
    Q_OBJECT
public:
    explicit AX25Converter(QObject *parent = nullptr);
    QByteArray destAdrToAX25(const QString &indicatif);
    QByteArray convertTNC2ToAX25(const QString &tnc2);
    QString convertAX25ToTNC2(const QByteArray &ax25);
    static QString decodeAX25Address(const QByteArray &addr7);
};

#endif // AX25CONVERTER_H
