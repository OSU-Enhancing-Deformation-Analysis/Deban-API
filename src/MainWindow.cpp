#include "MainWindow.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QString>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QMessageBox>
#include <QDoubleValidator>

namespace
{
    constexpr double kGuiMinLoad = 0.0;
    constexpr double kGuiMaxLoad = 25.0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      dllPathEdit(new QLineEdit(this)),
      targetLoadEdit(new QLineEdit(this)),
      overallStatusLabel(new QLabel("Overall: Not ready", this)),
      dllStatusLabel(new QLabel("DLL: Not loaded", this)),
      softwareStatusLabel(new QLabel("Software Link: Not connected", this)),
      hardwareStatusLabel(new QLabel("Hardware Data: Unavailable", this)),
      forceLabel(new QLabel("Force: unavailable", this)),
      browseDllButton(new QPushButton("Browse...", this)),
      loadDllButton(new QPushButton("Load DLL", this)),
      connectButton(new QPushButton("Connect", this)),
      sendLoadButton(new QPushButton("Send Load", this)),
      stopButton(new QPushButton("Emergency Stop", this)),
      logOutput(new QTextEdit(this)),
      forceTimer(new QTimer(this)),
      loadValidator(new QDoubleValidator(kGuiMinLoad, kGuiMaxLoad, 3, this)),
      dllLoaded(false),
      softwareConnected(false),
      hardwareReady(false)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    setWindowTitle("Deben API Control Panel");
    resize(950, 650);

    QString rememberedPath = loadLastDllPath();
    if (rememberedPath.isEmpty())
    {
        rememberedPath = "src/External Control DLL/DebenMT64.dll";
    }
    dllPathEdit->setText(rememberedPath);

    loadValidator->setNotation(QDoubleValidator::StandardNotation);
    targetLoadEdit->setValidator(loadValidator);
    targetLoadEdit->setPlaceholderText("Enter target load in N (0 to 25)");

    logOutput->setReadOnly(true);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    QGroupBox *statusBox = new QGroupBox("System Status", this);
    QGridLayout *statusLayout = new QGridLayout();
    statusLayout->addWidget(overallStatusLabel, 0, 0, 1, 2);
    statusLayout->addWidget(dllStatusLabel, 1, 0);
    statusLayout->addWidget(softwareStatusLabel, 1, 1);
    statusLayout->addWidget(hardwareStatusLabel, 2, 0);
    statusLayout->addWidget(forceLabel, 2, 1);
    statusBox->setLayout(statusLayout);

    QGroupBox *dllBox = new QGroupBox("DLL Setup", this);
    QHBoxLayout *dllLayout = new QHBoxLayout();
    dllLayout->addWidget(new QLabel("DLL Path:", this));
    dllLayout->addWidget(dllPathEdit);
    dllLayout->addWidget(browseDllButton);
    dllLayout->addWidget(loadDllButton);
    dllBox->setLayout(dllLayout);

    QGroupBox *controlBox = new QGroupBox("Manual Control", this);
    QGridLayout *controlLayout = new QGridLayout();
    controlLayout->addWidget(new QLabel("Target Load:", this), 0, 0);
    controlLayout->addWidget(targetLoadEdit, 0, 1);
    controlLayout->addWidget(sendLoadButton, 0, 2);
    controlLayout->addWidget(connectButton, 1, 0, 1, 2);
    controlLayout->addWidget(stopButton, 1, 2);
    controlBox->setLayout(controlLayout);

    mainLayout->addWidget(statusBox);
    mainLayout->addWidget(dllBox);
    mainLayout->addWidget(controlBox);
    mainLayout->addWidget(new QLabel("Event Log:", this));
    mainLayout->addWidget(logOutput);

    central->setLayout(mainLayout);

    connect(browseDllButton, &QPushButton::clicked, this, &MainWindow::onBrowseDll);
    connect(loadDllButton, &QPushButton::clicked, this, &MainWindow::onLoadDll);
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(sendLoadButton, &QPushButton::clicked, this, &MainWindow::onSendLoad);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(forceTimer, &QTimer::timeout, this, &MainWindow::updateForceDisplay);

    forceTimer->start(1000);

    appendLog("GUI initialized.");
    appendLog("Tip: use Browse to select DebenMT64.dll if the default path is wrong.");

    refreshSystemState();
    updateButtonStates();
}

MainWindow::~MainWindow() = default;

void MainWindow::appendLog(const QString &message)
{
    logOutput->append(message);
}

void MainWindow::onBrowseDll()
{
    QString selected = QFileDialog::getOpenFileName(
        this,
        "Select DebenMT64.dll",
        dllPathEdit->text(),
        "DLL Files (*.dll)"
    );

    if (!selected.isEmpty())
    {
        dllPathEdit->setText(QDir::toNativeSeparators(selected));
        appendLog("DLL path selected from file browser.");
    }
}

QString MainWindow::normalizeDllPath(const QString &path) const
{
    return QDir::toNativeSeparators(QDir::cleanPath(path));
}

QString MainWindow::resolveDllPath(const QString &userInput) const
{
    QString trimmed = userInput.trimmed();
    QString appDir = QCoreApplication::applicationDirPath();

    QStringList candidates;

    if (!trimmed.isEmpty())
    {
        QFileInfo info(trimmed);
        if (info.isAbsolute())
        {
            candidates << info.absoluteFilePath();
        }
        else
        {
            candidates << QDir::current().absoluteFilePath(trimmed);
            candidates << QDir(appDir).absoluteFilePath(trimmed);
            candidates << QDir(appDir).absoluteFilePath("../" + trimmed);
        }
    }

    candidates << QDir(appDir).absoluteFilePath("src/External Control DLL/DebenMT64.dll");
    candidates << QDir(appDir).absoluteFilePath("../src/External Control DLL/DebenMT64.dll");
    candidates << QDir(QDir::currentPath()).absoluteFilePath("src/External Control DLL/DebenMT64.dll");
    candidates << QDir(QDir::currentPath()).absoluteFilePath("External Control DLL/DebenMT64.dll");

    for (const QString &candidate : candidates)
    {
        QFileInfo candidateInfo(candidate);
        if (candidateInfo.exists() && candidateInfo.isFile())
        {
            return normalizeDllPath(candidateInfo.absoluteFilePath());
        }
    }

    return QString();
}

void MainWindow::saveLastDllPath(const QString &path)
{
    QSettings settings("OSU", "DebenAPI");
    settings.setValue("lastDllPath", path);
}

QString MainWindow::loadLastDllPath() const
{
    QSettings settings("OSU", "DebenAPI");
    return settings.value("lastDllPath").toString();
}

void MainWindow::onLoadDll()
{
    QString resolvedPath = resolveDllPath(dllPathEdit->text());

    if (resolvedPath.isEmpty())
    {
        dllLoaded = false;
        appendLog("DLL load failed.");
        appendLog("DebenMT64.dll could not be found. Check the path or use Browse.");
        refreshSystemState();
        updateButtonStates();
        return;
    }

    dllPathEdit->setText(resolvedPath);

    std::wstring dllPath = resolvedPath.toStdWString();

    if (controller.loadLibrary(dllPath.c_str()))
    {
        dllLoaded = true;
        saveLastDllPath(resolvedPath);
        appendLog("DLL loaded successfully.");
        appendLog("Resolved path: " + resolvedPath);
    }
    else
    {
        dllLoaded = false;
        appendLog("DLL load failed.");
        appendLog(QString::fromStdString(controller.getLastErrorMessage()));
    }

    refreshSystemState();
    updateButtonStates();
}

void MainWindow::onConnect()
{
    if (!dllLoaded)
    {
        appendLog("Connection blocked: DLL is not loaded.");
        refreshSystemState();
        updateButtonStates();
        return;
    }

    if (controller.connectDevice())
    {
        softwareConnected = true;
        appendLog("Software side connection established.");
        updateForceDisplay();
    }
    else
    {
        softwareConnected = false;
        appendLog("Connection failed.");
        appendLog(QString::fromStdString(controller.getLastErrorMessage()));
    }

    refreshSystemState();
    updateButtonStates();
}

void MainWindow::onSendLoad()
{
    if (!dllLoaded)
    {
        appendLog("Load command blocked: DLL not loaded.");
        return;
    }

    if (!softwareConnected)
    {
        appendLog("Load command blocked: software link not connected.");
        return;
    }

    QString rawInput = targetLoadEdit->text().trimmed();
    if (rawInput.isEmpty())
    {
        appendLog("Load command blocked: target load is empty.");
        return;
    }

    bool ok = false;
    double targetLoad = rawInput.toDouble(&ok);

    if (!ok)
    {
        appendLog("Load command blocked: invalid numeric value.");
        return;
    }

    if (targetLoad < kGuiMinLoad || targetLoad > kGuiMaxLoad)
    {
        appendLog(QString("Load command blocked: %1 N is outside GUI safe range [%2, %3] N.")
                  .arg(targetLoad)
                  .arg(kGuiMinLoad)
                  .arg(kGuiMaxLoad));
        return;
    }

    if (!hardwareReady)
    {
        QMessageBox::StandardButton response = QMessageBox::warning(
            this,
            "Hardware Data Unavailable",
            "The software link is connected, but hardware force data is currently unavailable.\n\n"
            "Do you still want to send this command?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (response != QMessageBox::Yes)
        {
            appendLog("Load command cancelled because hardware data is unavailable.");
            return;
        }
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

    refreshSystemState();
    updateButtonStates();
}

void MainWindow::onStop()
{
    if (!dllLoaded)
    {
        appendLog("Emergency stop ignored: DLL not loaded.");
        return;
    }

    if (!softwareConnected)
    {
        appendLog("Emergency stop ignored: software link not connected.");
        return;
    }

    controller.stopMotor();
    appendLog("Emergency stop requested.");

    refreshSystemState();
    updateButtonStates();
}

bool MainWindow::hasValidForce(double force) const
{
    return force >= 0.0;
}

void MainWindow::updateForceDisplay()
{
    double force = controller.getForce();

    hardwareReady = softwareConnected && hasValidForce(force);

    if (hardwareReady)
    {
        forceLabel->setText(QString("Force: %1 N").arg(force, 0, 'f', 3));
    }
    else
    {
        forceLabel->setText("Force: unavailable");
    }

    refreshSystemState();
    updateButtonStates();
}

void MainWindow::refreshSystemState()
{
    dllStatusLabel->setText(dllLoaded ? "DLL: Loaded" : "DLL: Not loaded");
    softwareStatusLabel->setText(softwareConnected ? "Software Link: Connected" : "Software Link: Not connected");
    hardwareStatusLabel->setText(hardwareReady ? "Hardware Data: Ready" : "Hardware Data: Unavailable");

    if (!dllLoaded)
    {
        overallStatusLabel->setText("Overall: Waiting for DLL");
    }
    else if (!softwareConnected)
    {
        overallStatusLabel->setText("Overall: DLL loaded, not connected");
    }
    else if (!hardwareReady)
    {
        overallStatusLabel->setText("Overall: Software connected, hardware data unavailable");
    }
    else
    {
        overallStatusLabel->setText("Overall: System ready");
    }
}

void MainWindow::updateButtonStates()
{
    connectButton->setEnabled(dllLoaded);
    sendLoadButton->setEnabled(dllLoaded && softwareConnected);
    stopButton->setEnabled(dllLoaded && softwareConnected);
}