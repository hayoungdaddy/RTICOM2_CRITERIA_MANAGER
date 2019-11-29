#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDebug>
#include <QDateTime>

#define MAX_QSCD_CNT 100
#define MAX_NUM_NETWORK 100
#define MAX_NUM_STATION 1000
#define MAX_NUM_EVENTINITSTA 5

typedef struct _message
{
    int time;
    char MType;
    int value;
} _MESSAGE;

typedef struct _configure
{
    QString configFileName;
    QString logDir;
    QString amq_ip, amq_port, amq_user, amq_passwd, amq_topic;
    QString db_ip, db_name, db_user, db_passwd;
} _CONFIGURE;

typedef struct _criteria
{
    int insec;
    int numsta;
    double thresholdG;
    int dist;
    double thresholdM;
} _CRITERIA;

#endif // COMMON_H
