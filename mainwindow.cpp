#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString configFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    activemq::library::ActiveMQCPP::initializeLibrary();

    ui->setupUi(this);

    codec = QTextCodec::codecForName("utf-8");

    cfg.configFileName = configFile;
    readCFG();

    log = new WriteLog();

    criteriaDB = QSqlDatabase::addDatabase("QMYSQL");
    criteriaDB.setHostName(cfg.db_ip);
    criteriaDB.setDatabaseName(cfg.db_name);
    criteriaDB.setUserName(cfg.db_user);
    criteriaDB.setPassword(cfg.db_passwd);

    criteriaModel = new QSqlQueryModel();

    connect(ui->savePB, SIGNAL(clicked(bool)), this, SLOT(savePBClicked(bool)));
    connect(ui->cancelPB, SIGNAL(clicked(bool)), this, SLOT(cancelPBClicked(bool)));
    //connect(ui->cancelPB, SIGNAL(clicked(bool)), this, SLOT(accept()));

    readCriteria();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readCFG()
{
    QFile file(cfg.configFileName);
    if(!file.exists())
    {
        qDebug() << "Failed configuration. Parameter file doesn't exists.";
        exit(1);
    }
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line, _line;

        while(!stream.atEnd())
        {
            line = stream.readLine();
            _line = line.simplified();

            if(_line.startsWith(" ") || _line.startsWith("#"))
                continue;
            else if(_line.startsWith("LOGDIR"))
                cfg.logDir = _line.section("=", 1, 1);
            else if(_line.startsWith("AMQ_IP"))
                cfg.amq_ip = _line.section("=", 1, 1);
            else if(_line.startsWith("AMQ_PORT"))
                cfg.amq_port = _line.section("=", 1, 1);
            else if(_line.startsWith("AMQ_USERNAME"))
                cfg.amq_user = _line.section("=", 1, 1);
            else if(_line.startsWith("AMQ_PASSWD"))
                cfg.amq_passwd = _line.section("=", 1, 1);
            else if(_line.startsWith("AMQ_STA_TOPIC"))
                cfg.amq_topic = _line.section("=", 1, 1);
            else if(_line.startsWith("DB_IP"))
                cfg.db_ip = _line.section("=", 1, 1);
            else if(_line.startsWith("DB_NAME"))
                cfg.db_name = _line.section("=", 1, 1);
            else if(_line.startsWith("DB_USERNAME"))
                cfg.db_user = _line.section("=", 1, 1);
            else if(_line.startsWith("DB_PASSWD"))
                cfg.db_passwd = _line.section("=", 1, 1);
        }
        file.close();
    }
}

void MainWindow::openDB()
{
    if(!criteriaDB.open())
    {
        log->write(cfg.logDir, "Error connecting to DB: " + criteriaDB.lastError().text());
    }
}

void MainWindow::readCriteria()
{
    QString query;
    query = "SELECT * FROM CRITERIA";
    openDB();
    criteriaModel->setQuery(query);

    criteria.insec = criteriaModel->record(0).value("inseconds").toInt();
    criteria.numsta = criteriaModel->record(0).value("numsta").toInt();
    criteria.thresholdG = criteriaModel->record(0).value("thresholdG").toDouble();
    criteria.dist = criteriaModel->record(0).value("distance").toInt();
    criteria.thresholdM = criteriaModel->record(0).value("thresholdM").toDouble();

    //qDebug() << staListVT.count();
    log->write(cfg.logDir, "Read Criteria List from DB");

    ui->insecLE->setText(QString::number(criteria.insec));
    ui->numstaLE->setText(QString::number(criteria.numsta));
    ui->thresholdGLE->setText(QString::number(criteria.thresholdG, 'f', 2));
    ui->distLE->setText(QString::number(criteria.dist));
    ui->thresholdMLE->setText(QString::number(criteria.thresholdM, 'f', 2));
}

void MainWindow::savePBClicked(bool)
{
    if( !QMessageBox::question( this,
                                codec->toUnicode("저장"),
                                codec->toUnicode("Criteria 정보 Database를 변경합니다."),
                                codec->toUnicode("저장"),
                                codec->toUnicode("취소"),
                                QString::null, 1, 1) )
    {
        log->write(cfg.logDir, "============================================================");
        log->write(cfg.logDir, "Changed RTICOM2 Criteria list");

        openDB();
        QString query;

        QString insecS = ui->insecLE->text();
        QString numstaS = ui->numstaLE->text();
        QString thresholdGS = ui->thresholdGLE->text();
        QString distS = ui->distLE->text();
        QString thresholdMS = ui->thresholdMLE->text();

        query = "UPDATE CRITERIA SET "
                "inseconds=" + insecS +
                ", numsta=" + numstaS +
                ", thresholdG=" + thresholdGS +
                ", distance=" + distS +
                ", thresholdM=" + thresholdMS +
                " WHERE inseconds=" + QString::number(criteria.insec) + ";";

        //qDebug() << query;

        criteria.insec = insecS.toInt();
        criteria.numsta = numstaS.toInt();
        criteria.thresholdG = thresholdGS.toDouble();
        criteria.dist = distS.toInt();
        criteria.thresholdM = thresholdMS.toDouble();

        criteriaModel->setQuery(query);

        QString failover = "failover:(tcp://" + cfg.amq_ip + ":" + cfg.amq_port + ")";

        PushMessage *pushmessage = new PushMessage(cfg.logDir, this);
        if(!pushmessage->isRunning())
        {
            pushmessage->setup(failover, cfg.amq_user, cfg.amq_passwd, cfg.amq_topic, true, false);
            pushmessage->start();
        }

        log->write(cfg.logDir, "inseconds : " + insecS);
        log->write(cfg.logDir, "numsta : " + numstaS);
        log->write(cfg.logDir, "thresholdG : " + thresholdGS);
        log->write(cfg.logDir, "distance : " + distS);
        log->write(cfg.logDir, "thresholdM : " + thresholdMS);
        log->write(cfg.logDir, "============================================================");

        QMessageBox msgBox;
        msgBox.setWindowTitle("RTICOM2 - Criteria Manager");
        msgBox.setText(codec->toUnicode("저장이 완료되었습니다."));
        msgBox.exec();
    }
}

void MainWindow::cancelPBClicked(bool)
{
    close();
}
