#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QtNetwork/qtcpsocket.h>

class QDate;
class QTime;


class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
    ClientSocket(QObject *parent = 0);

private slots:
    void readClient();
	void clear();

private:
  
    quint32 nextBlockSize;  

	QObject* parent_;


};

#endif
