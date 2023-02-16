#ifndef __hologram_server_h
#define __hologram_server_h

#include "renderwidget.h"
#include <QtNetwork/qtcpserver.h>
#include "Hologram/HologramGenerator.h"

typedef struct  {
	int			channel;
	int			pnx;
	int			pny;
	float       pixel_pitch_x;
	float		pixel_pitch_y;
	float		wave_len_;
	float		offset_;
	int			point_cnt;

}  Host_Command;

class HologramServer : public QTcpServer
{
    Q_OBJECT

public:
    HologramServer(QObject *parent = 0);

	Host_Command* getCommand(){ return &cmd_; }

	void generateHologram(char* data, float* cghR, float* cghG, float* cghB);

private:
    void incomingConnection(qintptr handle);

	RenderWidget* render_;

	Host_Command	cmd_;
	HologramGenerator* hologram_;
};

#endif
