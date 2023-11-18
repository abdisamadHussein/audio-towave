#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QProcess>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEditpath->setStyleSheet("QLineEdit {  border: 0px;}");
    ui->progressBar->setVisible(false);

//    // Set fixed window size
    this->setFixedSize(400, 400); // Adjust the size to your desired width and height
//    // Set the window to not resizable
//    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1) {
        // Check if the dropped item is a directory
        QFileInfo fileInfo(event->mimeData()->urls().first().toLocalFile());
        if (fileInfo.isDir()) {
            event->acceptProposedAction();
        }
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls() && mimeData->urls().size() == 1) {
        QString droppedPath = mimeData->urls().first().toLocalFile();

        // Check if the dropped item is a directory
        QFileInfo fileInfo(droppedPath);
        if (fileInfo.isDir()) {
            scrDirectoryPath = droppedPath;
            ui->lineEditpath->setText(scrDirectoryPath);
        }
    }
}

void MainWindow::on_chooseDirectory_clicked()
{
    scrDirectoryPath = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), QDir::homePath());

    if (!scrDirectoryPath.isEmpty()) {

        ui->lineEditpath->setText(scrDirectoryPath);

    }

}

QString sanitizeFilename(const QString &filename) {
    // Replace special characters and symbols with spaces
    QString sanitizedName = filename;
    sanitizedName.replace(QRegularExpression("[^A-Za-z0-9 ]"), " ");
    // Remove leading and trailing spaces
    sanitizedName = sanitizedName.trimmed();
    // Replace consecutive spaces with a single space
    sanitizedName.replace(QRegularExpression("\\s+"), " ");
    return sanitizedName;
}


void MainWindow::on_SaveToDirectory_clicked()
{
    QString initialDirectory = lastOpenedDirectory.isEmpty() ? QDir::homePath() : lastOpenedDirectory;

    distDirectoryPath = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), initialDirectory);

    lastOpenedDirectory = distDirectoryPath;

    bool shouldSanitizeFilename = ui->checkBoxRemove->isChecked();

    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QString ffmpegPath = QDir(applicationDirPath).filePath("ffmpeg/ffmpeg");

    QDir inputDir(scrDirectoryPath);
    QStringList filter;
    filter << ui->comboBoxType->currentText();
    qDebug() << filter;
    QStringList fileList = inputDir.entryList(filter, QDir::Files);

    ui->progressBar->setVisible(true);

    for (const QString &filename : fileList) {
        QString inputFilePath = scrDirectoryPath + "/" + filename;
        QString outputFilename = QFileInfo(filename).baseName();

        if (shouldSanitizeFilename) {
            outputFilename = sanitizeFilename(outputFilename);
        }

        QString outputFilePath = distDirectoryPath + "/" + outputFilename + ".wav";

        qDebug() << inputFilePath;
        qDebug() << outputFilePath;
        qDebug() << filter;
         qDebug() << ffmpegPath;

        // Construct the FFmpeg command
        QStringList command;
        command  << "-i" << inputFilePath << "-ac" << "1" << "-acodec" << "pcm_s16le" << "-ar" << "8000" << outputFilePath;
        // Execute the command
        QProcess ffmpegProcess;
        // Connect the readyReadStandardError signal to a slot
        QObject::connect(&ffmpegProcess, &QProcess::readyReadStandardError, [&]() {
            // Read the output from FFmpeg and parse progress information
            QByteArray output = ffmpegProcess.readAllStandardError();
            // Implement your progress parsing logic here
            // Update the progress bar accordingly

            // For example, if FFmpeg output contains lines like "time=00:01:23.45", you can extract the time
            // and update the progress bar based on the total duration of the video.
            // ...

            // Print the FFmpeg output to debug (optional)
            qDebug() << output;
        });

        // Wait for the process to finish
        ffmpegProcess.waitForFinished();

        // Update the progress bar after each process finishes
        ui->progressBar->setValue((fileList.indexOf(filename) + 1) * 100 / fileList.size());

        ffmpegProcess.start(ffmpegPath, command);
        ffmpegProcess.waitForFinished();

    }

}

