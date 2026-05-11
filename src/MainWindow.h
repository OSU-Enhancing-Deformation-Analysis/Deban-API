#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QLineEdit;
class QLabel;
class QTextEdit;
class QPushButton;
class QTimer;
class QDoubleValidator;

#include "DebenController.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowseDll();
    void onLoadDll();
    void onConnect();
    void onSendLoad();
    void onStop();
    void updateForceDisplay();

private:
    void appendLog(const QString &message);
    void refreshSystemState();
    void updateButtonStates();

    bool hasValidForce(double force) const;
    QString resolveDllPath(const QString &userInput) const;
    QString normalizeDllPath(const QString &path) const;
    void saveLastDllPath(const QString &path);
    QString loadLastDllPath() const;

    DebenController controller;

    QLineEdit *dllPathEdit;
    QLineEdit *targetLoadEdit;

    QLabel *overallStatusLabel;
    QLabel *dllStatusLabel;
    QLabel *softwareStatusLabel;
    QLabel *hardwareStatusLabel;
    QLabel *forceLabel;

    QPushButton *browseDllButton;
    QPushButton *loadDllButton;
    QPushButton *connectButton;
    QPushButton *sendLoadButton;
    QPushButton *stopButton;

    QTextEdit *logOutput;
    QTimer *forceTimer;
    QDoubleValidator *loadValidator;

    bool dllLoaded;
    bool softwareConnected;
    bool hardwareReady;
};

#endif