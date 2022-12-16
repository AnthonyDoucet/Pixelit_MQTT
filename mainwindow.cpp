/** @file mainwindow.cpp
 *  @brief Gestion de la fenetre principale
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColorDialog>
#include <QColor>


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    pixelit.init(ui->lineEditHostname->text(), 1883);
    pixelit.setMasterTopic(ui->lineEditTopic->text());

    connect(&pixelit, &Pixelit::statusChanged, this, &MainWindow::setStateUI);
    connect(&pixelit, &Pixelit::newMessage, this, &MainWindow::setTextBrowserUI);
}

MainWindow::~MainWindow(){ delete ui; }

void MainWindow::positionChanged(){ pixelit.setPosition(ui->spinBoxPosition_X->value() , ui->spinBoxPosition_Y->value()); }

void MainWindow::btnSubscribe(){ pixelit.MQTTsubscribe(); }

void MainWindow::colorBtnClicked(){

    QColor color = QColorDialog().getColor();

    QPalette pal = ui->btnColor->palette();
    pal.setColor(QPalette::Button, color);
    ui->btnColor->setAutoFillBackground(true);
    ui->btnColor->setPalette(pal);
    ui->btnColor->update();

    pixelit.setColor(color);
}

void MainWindow::brightnessChanged(){
    pixelit.setBrightness(ui->sliderBrightness->value());
    ui->labelBrightness->setText(QString("Brightness: %1").arg(ui->sliderBrightness->value()));
}

void MainWindow::updateText(){ pixelit.setText(); }

void MainWindow::updateClock(){ pixelit.setClock(); }

void MainWindow::setStateUI(QString str){ ui->labelStatus->setText(str); }

void MainWindow::setTextBrowserUI(QString str){ ui->textBrowserMatrixInfo->setText(str); }
