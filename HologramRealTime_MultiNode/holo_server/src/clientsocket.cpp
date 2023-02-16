#include <QtNetwork/qtcpsocket.h>

#include "holo_server/clientsocket.h"
#include "holo_server/hologram_server.h"
#include <QtOpenGL/QGLWidget>

ClientSocket::ClientSocket(QObject *parent)
    : QTcpSocket(parent), parent_(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(clear()));

    nextBlockSize = 0;

}
//static float* hologram_;
static float* hologram_R_;
static float* hologram_G_;
static float* hologram_B_;
static int hologram_size_;

void ClientSocket::readClient()
{
	QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_3);

	HologramServer* parent = (HologramServer*)parent_;
	Host_Command* cmd = parent->getCommand();

	if (nextBlockSize == 0) 
	{
		int cmd_sz = sizeof(Host_Command);
		if ( bytesAvailable() < cmd_sz )
			return;

		char* cmd_data = (char*) malloc(cmd_sz);
		in.readRawData(cmd_data, cmd_sz);

		Host_Command* cmdin = (Host_Command*)cmd_data;

		*cmd = *cmdin;

        //printf("chan %d, pnx %d pny %d pitch %f, wl %f, offset %f, cnt %d\n",
        //    cmd->channel, cmd->pnx, cmd->pny, cmd->pixel_pitch_x, cmd->wave_len_, cmd->offset_, cmd->point_cnt);

		nextBlockSize = cmd->point_cnt * sizeof(float) * 6;
	}

	int av = bytesAvailable();
	if (/*nextBlockSize == 0 ||*/ bytesAvailable() < nextBlockSize)
        return;


	char* data = (char*) malloc(nextBlockSize);
	int size = in.readRawData(data, nextBlockSize);
	if (hologram_size_ < cmd->pnx * cmd->pny) {
		if (hologram_R_) free(hologram_R_);
		if (hologram_G_) free(hologram_G_);
		if (hologram_B_) free(hologram_B_);
		//if (hologram_) free(hologram_);
		hologram_size_ = cmd->pnx * cmd->pny;
		//hologram_ = (float*)malloc(hologram_size_ * sizeof(float));
		hologram_R_ = (float*)malloc(hologram_size_ * sizeof(float));
		hologram_G_ = (float*)malloc(hologram_size_ * sizeof(float));
		hologram_B_ = (float*)malloc(hologram_size_ * sizeof(float));
	}
	parent->generateHologram(data, hologram_R_, hologram_G_, hologram_B_);

	nextBlockSize = 0; // initialize


	//QString qs("ack");
	//write((char*)qs.data(), 3);
	in.writeRawData((char*)hologram_R_, hologram_size_ * sizeof(float));
	in.writeRawData((char*)hologram_G_, hologram_size_ * sizeof(float));
	in.writeRawData((char*)hologram_B_, hologram_size_ * sizeof(float));
	//QImage im((uchar*)hologram_R_, cmd->pnx, cmd->pny, cmd->pny, QImage::Format::Format_Grayscale8);
	//im.save("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/holoR.bmp");
	//QImage io((uchar*)hologram_G_, cmd->pnx, cmd->pny, cmd->pny, QImage::Format::Format_Grayscale8);
	//io.save("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/holoG.bmp");
	//QImage ip((uchar*)hologram_B_, cmd->pnx, cmd->pny, cmd->pny, QImage::Format::Format_Grayscale8);
	//ip.save("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/holoB.bmp");
    //printf("hologram %f %f\n", hologram_[0], hologram_[1]);
	free(data);
}




void ClientSocket::clear()
{
	nextBlockSize = 0;
}

