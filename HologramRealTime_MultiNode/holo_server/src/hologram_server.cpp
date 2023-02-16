

#include "holo_server/clientsocket.h"
#include "holo_server/hologram_server.h"

HologramServer::HologramServer(QObject *parent)
    : QTcpServer(parent)
{
	hologram_ = new HologramGenerator();
}

void HologramServer::incomingConnection(qintptr handle)
{
    ClientSocket *socket = new ClientSocket(this);
    socket->setSocketDescriptor(handle);
}

void HologramServer::generateHologram(char* d, float* cghR, float* cghG, float* cghB)
{
	hologram_->setConfig_PointCloud(cmd_.offset_, 
		graphics::vec2(cmd_.pixel_pitch_x, cmd_.pixel_pitch_y), 
		graphics::ivec2(cmd_.pnx, cmd_.pny), 
		cmd_.wave_len_);
	HologramData_PointCloud* data = hologram_->getPCData();
	// now generate hologram with data
	data->n_points = cmd_.point_cnt;
	data->n_channels = cmd_.channel;
	if (data->ObjPosition)	delete[] data->ObjPosition;
	data->ObjPosition = new float[3 * cmd_.point_cnt];

	if (data->ObjIntensity)	delete[] data->ObjIntensity;
	data->ObjIntensity = new float[3 * cmd_.point_cnt];

	memcpy(data->ObjPosition, d, data->n_points * sizeof(float) * 3);
	memcpy(data->ObjIntensity, d + data->n_points * sizeof(float) * 3, data->n_points * sizeof(float) * 3);
	hologram_->generateHologram();

	hologram_->GetReal();

	//hologram_->encoding_ssb();

    //hologram_->readback_ssb();

	//hologram_->normalize_gpu();
	//uchar* cgh_a = hologram_->getNormalizedCGH();
	//memcpy(cgh, cgh_a, cmd_.pnx*cmd_.pny);
	hologram_->readback_ssbR();
	memcpy(cghR, hologram_->getEncodedCGH(), cmd_.pnx*cmd_.pny * sizeof(float));
	hologram_->readback_ssbG();
	memcpy(cghG, hologram_->getEncodedCGH(), cmd_.pnx*cmd_.pny * sizeof(float));
	hologram_->readback_ssbB();
	memcpy(cghB, hologram_->getEncodedCGH(), cmd_.pnx*cmd_.pny * sizeof(float));
   

}