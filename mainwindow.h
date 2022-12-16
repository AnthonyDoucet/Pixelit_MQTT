#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pixelit.h>

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
    void setStateUI(QString str);
    void setTextBrowserUI(QString str);

private slots:
    void btnSubscribe();
    ///@brief Slot positionChanged
    void positionChanged();
    ///@brief Slot changement de couleur
    void colorBtnClicked();
    ///@brief Slot changemenet de brightness
    void brightnessChanged();
    ///@brief Slot changement de texte
    void updateText();
    ///@brief Slot changement horloge
    void updateClock();

private:
    Ui::MainWindow *ui;
    Pixelit pixelit;
};


#endif // MAINWINDOW_H
