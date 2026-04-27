#include "MainWindow.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      dllPathEdit(new QLineEdit(this)),
      targetLoadEdit(new QLineEdit(this)),
      statusLabel(new QLabel("Status: Not connected", this)),
      forceLabel(new QLabel("Force: N/A", this)),
      loadDllButton(new QPushButton("Load DLL", this)),
      connectButton(new QPushButton("Connect", this)),
      sendLoadButton(new QPushButton("Send Load", this)),
      stopButton(new QPushButton("Emergency Stop", this)),
      logOutput(new QTextEdit(this)),
      forceTimer(new QTimer(this))
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    setWindowTitle("Deben API Control Panel");
    resize(900, 600);

    dllPathEdit->setText("External control DLL/DebenMT64.dll");
    targetLoadEdit->setPlaceholderText("Enter target load in N");

    logOutput->setReadOnly(true);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(forceLabel);

    QHBoxLayout *dllLayout = new QHBoxLayout();
    dllLayout->addWidget(new QLabel("DLL Path:", this));
    dllLayout->addWidget(dllPathEdit);
    dllLayout->addWidget(loadDllButton);

    QHBoxLayout *connectLayout = new QHBoxLayout();
    connectLayout->addWidget(connectButton);
    connectLayout->addWidget(stopButton);

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(new QLabel("Target Load:", this));
    controlLayout->addWidget(targetLoadEdit);
    controlLayout->addWidget(sendLoadButton);

    mainLayout->addLayout(dllLayout);
    mainLayout->addLayout(connectLayout);
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(new QLabel("Event Log:", this));
    mainLayout->addWidget(logOutput);

    central->setLayout(mainLayout);

    connect(loadDllButton, &QPushButton::clicked, this, &MainWindow::onLoadDll);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(sendLoadButton, &QPushButton::clicked, this, &MainWindow::onSendLoad);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(forceTimer, &QTimer::timeout, this, &MainWindow::updateForceDisplay);

    forceTimer->start(1000);

    appendLog("GUI initialized.");
}

MainWindow::~MainWindow() = default;

void MainWindow::appendLog(const QString &message)
{
    logOutput->append(message);
}

void MainWindow::onLoadDll()
{
    std::wstring dllPath = dllPathEdit->text().toStdWString();

    if (controller.loadLibrary(dllPath.c_str()))
    {
        statusLabel->setText("Status: DLL loaded");
        appendLog("DLL loaded successfully.");
    }
    else
    {
        appendLog("DLL load failed.");
        appendLog(QString::fromStdString(controller.getLastErrorMessage()));
    }
}

void MainWindow::onConnect()
{
    if (controller.connectDevice())
    {
        statusLabel->setText("Status: Connected");
        appendLog("Connected to device.");
        updateForceDisplay();
    }
    else
    {
        appendLog("Connection failed.");
        appendLog(QString::fromStdString(controller.getLastErrorMessage()));
    }
}

void MainWindow::onSendLoad()
{
    bool ok = false;
    double targetLoad = targetLoadEdit->text().toDouble(&ok);

    if (!ok)
    {
        appendLog("Invalid target load.");
        return;
    }

    if (controller.gotoLoad(targetLoad))
    {
        appendLog(QString("Sent load command: %1 N").arg(targetLoad));
        updateForceDisplay();
    }
    else
    {
        appendLog("Load command failed.");
        appendLog(QString::fromStdString(controller.getLastErrorMessage()));
    }
}

void MainWindow::onStop()
{
    controller.stopMotor();
    appendLog("Emergency stop requested.");
}

void MainWindow::updateForceDisplay()
{
    double force = controller.getForce();

    if (force <= -9999.0)
    {
        forceLabel->setText("Force: unavailable");
    }
    else
    {
        forceLabel->setText(QString("Force: %1 N").arg(force));
    }
}