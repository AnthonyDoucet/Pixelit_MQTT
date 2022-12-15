/** @file mainwindow.cpp
 *  @brief Gestion de la fenetre principale
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMqttClient>
#include <QColorDialog>
#include <QColor>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);

    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    timer->start(100);

    ui->setupUi(this);

    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::MQTTstateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::MQTTmessageReceived);

    m_client->setHostname(ui->lineEditHostname->text());
    m_client->setPort(1883) ;
    m_client->connectToHost();


    unsigned brightness = 50; //afficheur.getBrightness();
    ui->labelBrightness->setText(QString("Brightness: %1").arg(brightness));
    ui->sliderBrightness->setValue(brightness);
    jposition["x"] = ui->spinBoxPosition_X->value();
    jposition["y"] = ui->spinBoxPosition_Y->value();
    jcolor["r"] = 0;
    jcolor["g"] = 255;
    jcolor["b"] = 0;
}


MainWindow::~MainWindow(){ delete ui; }

void MainWindow::positionChanged(){
    jposition["x"] = ui->spinBoxPosition_X->value();
    jposition["y"] = ui->spinBoxPosition_Y->value();
    updateText();
}
void MainWindow::colorBtnClicked(){

    QColor color = QColorDialog().getColor();

    QPalette pal = ui->btnColor->palette();
    pal.setColor(QPalette::Button, color);
    ui->btnColor->setAutoFillBackground(true);
    ui->btnColor->setPalette(pal);
    ui->btnColor->update();

    jcolor["r"] = color.red();
    jcolor["g"] = color.green();
    jcolor["b"] = color.blue();

    if(ui->checkBoxShowClock->isChecked()){
        sendCommand(2);
    }
    else{
        sendCommand(1);
    }
}

/* ENVOI */
void MainWindow::brightnessChanged(){ sendCommand(0); }
void MainWindow::updateText(){ sendCommand(1); }
void MainWindow::updateClock(){ sendCommand(2); }

void MainWindow::sendCommand(unsigned choix){
    QJsonObject jtext, jclock;
    switch(choix){
        case 0: //Brightness
            ui->labelBrightness->setText(QString("Brightness: %1").arg(ui->sliderBrightness->value()));
            //json.insert("brightness", ui->sliderBrightness->value());
            json["brightness"] = ui->sliderBrightness->value();
            break;
        case 1: //Text
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
            break;
        case 2: //Clock
            jclock["show"] = ui->checkBoxShowClock->isChecked();
            jclock["switchAktiv"] = ui->checkBoxAktiv->isChecked();
            jclock["withSeconds"] = ui->checkBoxSeconds->isChecked();
            jclock["switchSec"] = ui->spinBoxSwitchSec->value();
            jclock["drawWeekDays"] = ui->checkBoxWeek->isChecked();
            jclock["color"] = jcolor;
            //jtext["hexColor"] = "#FFFFFF";

            json["text"] = "";
            json["clock"] = jclock;
            break;
        default:
            break;
    }
    newValue = true;
}
void MainWindow::update(){
    if(newValue){
        newValue = false;
        //qDebug() << json;
        auto time = QDateTime::currentDateTime().time().toString();
        qDebug() << time << json;
        auto j = QJsonDocument(json).toJson();
        if (m_client->publish(ui->lineEditTopic->text()+"/pixelitsetScreen", QString(j).toUtf8(), 0, false) == -1)
                QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
    }
}

/* RECEPTION */
void MainWindow::MQTTsubscribe(){
    auto state = m_client->subscribe(ui->lineEditTopic->text() + "/pixelitmatrixinfo", 0);
    if (!state) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
}
void MainWindow::MQTTstateChanged(QMqttClient::ClientState state){
    switch (state) {
        case QMqttClient::ClientState::Disconnected:
            ui->labelStatus->setText(QLatin1String("Disconnected"));
            break;
        case QMqttClient::ClientState::Connecting:
            ui->labelStatus->setText(QLatin1String("Connecting"));
            break;
        case QMqttClient::ClientState::Connected:
            ui->labelStatus->setText(QLatin1String("Connected"));
            break;
        default:
            ui->labelStatus->setText(QLatin1String("--Unknown--"));
            break;
    }
}
void MainWindow::MQTTmessageReceived(const QByteArray &msg, const QMqttTopicName &topic){
    //qDebug() << "Topic: " << topic;
    //qDebug() << "Msg: " << msg;
    //ui->textBrowserMatrixInfo->setText(msg);
    QJsonObject rootObject = QJsonDocument::fromJson(msg).object();

    ui->textBrowserMatrixInfo->clear();
    ui->textBrowserMatrixInfo->append("Hostname: "      + rootObject["hostname"].toString());
    ui->textBrowserMatrixInfo->append("Version: "       + rootObject["pixelitVersion"].toString());
    ui->textBrowserMatrixInfo->append("SSID: "          + rootObject["wifiSSID"].toString() );
    ui->textBrowserMatrixInfo->append("Wifi RSSI: "     + QString::number(rootObject["wifiRSSI"].toInt()) );
    ui->textBrowserMatrixInfo->append("Wifi Quality: "  + QString::number(rootObject["wifiQuality"].toInt()) );
    ui->textBrowserMatrixInfo->append("IP: "            + rootObject["ipAddress"].toString() );
    ui->textBrowserMatrixInfo->append("Brightness: "    + QString::number(rootObject["currentMatrixBrightness"].toInt()) );

}
