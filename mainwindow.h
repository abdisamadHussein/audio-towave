#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString scrDirectoryPath;
    QString distDirectoryPath;
    QString lastOpenedDirectory;

protected:
    // Declarations for drag-and-drop events
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_chooseDirectory_clicked();

    void on_SaveToDirectory_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
