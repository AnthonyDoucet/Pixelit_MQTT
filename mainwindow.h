#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>
#include <QtGui>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ///@brief Constructeur
    MainWindow(QWidget *parent = nullptr);
    ///@brief Destructeur
    ~MainWindow();

public slots:
    void MQTTstateChanged(QMqttClient::ClientState state);
    void MQTTmessageReceived(const QByteArray &msg, const QMqttTopicName &topic);
    void MQTTsubscribe();

private slots:
    ///@brief Slot positionChanged
    void positionChanged();
    ///@brief Slot changement de couleur
    void colorBtnClicked();
    ///@brief Slot changemenet de brightness, Envoie la commande 0
    void brightnessChanged();
    ///@brief Slot changement de texte, Envoie la commande 1
    void updateText();
    void updateClock();



private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    QTimer *timer = new QTimer(this);

    bool newValue;

    QJsonObject json;
    QJsonObject jcolor;
    QJsonObject jposition;

    QString text;
    void sendCommand(unsigned);
    void update();
};


#endif // MAINWINDOW_H
