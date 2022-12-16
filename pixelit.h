#ifndef PIXELIT_H
#define PIXELIT_H

#include <QMqttClient>
#include <QtGui>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>


class Pixelit: public QObject
{
    Q_OBJECT
public:
    Pixelit();
    int init(QString hostname, unsigned port);
    void setMasterTopic(QString topic){ masterTopic = topic; };
    void setText();
    void setClock();
    void setColor(QColor);
    void setPosition(int x, int y);
    void setBrightness(int b);
    void setSlots(std::function<void()> stateUI);

public slots:
    void MQTTstateChanged(QMqttClient::ClientState state);
    void MQTTmessageReceived(const QByteArray &msg, const QMqttTopicName &topic);
    void MQTTsubscribe();

signals:
    void statusChanged(QString str);
    void newMessage(QString str);

private:
    QMqttClient *m_client;
    QTimer *timer = new QTimer(this);
    QString masterTopic;
    bool newValue;

    QJsonObject json;
    QJsonObject jcolor;
    QJsonObject jposition;

    QString text;

private:
    void timerEv();
};

#endif // PIXELIT_H
