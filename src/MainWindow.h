#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QLabel;
class QTextEdit;
class QPushButton;
class QTimer;

#include "DebenController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadDll();
    void onConnect();
    void onSendLoad();
    void onStop();
    void updateForceDisplay();

private:
    void appendLog(const QString &message);

    DebenController controller;

    QLineEdit *dllPathEdit;
    QLineEdit *targetLoadEdit;

    QLabel *statusLabel;
    QLabel *forceLabel;

    QPushButton *loadDllButton;
    QPushButton *connectButton;
    QPushButton *sendLoadButton;
    QPushButton *stopButton;

    QTextEdit *logOutput;
    QTimer *forceTimer;
};

#endif