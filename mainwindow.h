#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>

#include <QTextCodec>
#include <QMessageBox>

#include "pushmessage.h"
#include "writelog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString configFile = nullptr, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    _CONFIGURE cfg;
    WriteLog *log;

    _CRITERIA criteria;
    void readCriteria();

    void readCFG();

    void openDB();
    // About Database & table
    QSqlDatabase criteriaDB;
    QSqlQueryModel *criteriaModel;

    QTextCodec *codec;

private slots:
    void savePBClicked(bool);
    void cancelPBClicked(bool);
};

#endif // MAINWINDOW_H
