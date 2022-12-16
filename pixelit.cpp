#include "pixelit.h"
#include <QMqttClient>
#include <QColorDialog>
#include <QColor>

Pixelit::Pixelit()
{
    connect(timer, &QTimer::timeout, this, &Pixelit::timerEv);
    timer->start(100);

    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::stateChanged, this, &Pixelit::MQTTstateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &Pixelit::MQTTmessageReceived);

    jposition["x"] = 0;
    jposition["y"] = 0;
    jcolor["r"] = 0;
    jcolor["g"] = 255;
    jcolor["b"] = 0;
}

int Pixelit::init(QString hostname, unsigned port)
{
    m_client->setHostname(hostname);
    m_client->setPort(port) ;
    m_client->connectToHost();
}

void Pixelit::setText(){
     QJsonObject jtext;
    /*
    jtext["textString"] = ui->lineEditText->text();
    jtext["bigFont"] = ui->checkBoxBigFont->isChecked();
    jtext["scrollText"] = ui->checkBoxScroll->isChecked();
    jtext["scrollTextDelay"] = ui->spinBoxScrollDelay->value();
    jtext["centerText"] = ui->checkBoxCenter->isChecked();
    jtext["position"] = jposition;
    jtext["color"] = jcolor;
    //jtext["hexColor"] = "#FFFFFF";
    json["clock"] = "";
    json["text"] = jtext;
    */
    newValue = true;
}

void Pixelit::setClock(){
     QJsonObject jclock;
    /*
    jclock["show"] = ui->checkBoxShowClock->isChecked();
    jclock["switchAktiv"] = ui->checkBoxAktiv->isChecked();
    jclock["withSeconds"] = ui->checkBoxSeconds->isChecked();
    jclock["switchSec"] = ui->spinBoxSwitchSec->value();
    jclock["drawWeekDays"] = ui->checkBoxWeek->isChecked();
    jclock["color"] = jcolor;
    //jtext["hexColor"] = "#FFFFFF";

    json["text"] = "";
    json["clock"] = jclock;
    */
    newValue = true;
}

void Pixelit::setColor(QColor color){
    jcolor["r"] = color.red();
    jcolor["g"] = color.green();
    jcolor["b"] = color.blue();
}

void Pixelit::setPosition(int x, int y){
    jposition["x"] = x;
    jposition["y"] = y;
}

void Pixelit::setBrightness(int b){
    json.insert("brightness", b);
    //json["brightness"] = b;
}

void Pixelit::MQTTstateChanged(QMqttClient::ClientState state)
{
    switch (state) {
        case QMqttClient::ClientState::Disconnected:
            emit statusChanged("Disconnected");
            break;
        case QMqttClient::ClientState::Connecting:
            emit statusChanged("Connecting");
            break;
        case QMqttClient::ClientState::Connected:
            emit statusChanged("Connected");
            break;
        default:
            emit statusChanged("--Unknown--");
            break;
    }
}

void Pixelit::MQTTmessageReceived(const QByteArray &msg, const QMqttTopicName &topic)
{
    qDebug() << "New Message from topic: " << topic;
    //qDebug() << "Msg: " << msg;

    QJsonObject rootObject = QJsonDocument::fromJson(msg).object();

    QString str;
    str.append("Hostname: "      + rootObject["hostname"].toString() ); str.append('\n');
    str.append("Version: "       + rootObject["pixelitVersion"].toString() );   str.append('\n');
    str.append("SSID: "          + rootObject["wifiSSID"].toString() ); str.append('\n');
    str.append("Wifi RSSI: "     + QString::number(rootObject["wifiRSSI"].toInt()) );   str.append('\n');
    str.append("Wifi Quality: "  + QString::number(rootObject["wifiQuality"].toInt()) );    str.append('\n');
    str.append("IP: "            + rootObject["ipAddress"].toString() );    str.append('\n');
    str.append("Brightness: "    + QString::number(rootObject["currentMatrixBrightness"].toInt()) );    str.append('\n');

    //qDebug() << str;
    emit newMessage(str);
}

void Pixelit::MQTTsubscribe()
{
    auto state = m_client->subscribe(masterTopic + "/pixelitmatrixinfo", 0);
    if (!state) {
        qDebug() << "Subscribe Error";
        return;
    }
    qDebug() << "Subscribe to /pixelitmatrixinfo ok";
}

void Pixelit::timerEv()
{
    if(newValue){
        newValue = false;

        auto time = QDateTime::currentDateTime().time().toString();
        qDebug() << time << json;

         if (m_client->publish(QMqttTopicName(masterTopic + "/pixelitsetScreen"), QString(QJsonDocument(json).toJson()).toUtf8(), 0, false) == -1)
                qDebug() << "publish Error";
    }
}
