//#include "HologramRender.h"
#include <graphics/sys.h>
#include <direct.h>
//#include <graphics/gl.h>
#include <graphics/RenderEnv.h>
#include <QtCore/QDir>
#include <QtCore/qmath.h>
#include <QtGui/QImage.h>
#include <fstream>
#include <QtGui/QPainter.h>

#include "model/MemManager.h"
#include "model/sync_gpu_memory.h"
#include "model/rel_mem_manager.h"
#include <graphics/RenderEnv.h>
#include "material/GMaterial.h"
#include "material/GTexture.h"
#include "graphics/gl_stat.h"
#include "graphics/fvec.h"
#include <holo_client/holo_client.h>
#include <Hologram/HologramGenerator_GPU.h>

std::vector<client_thread*> holo_clients_;
double				ppx_ = 0.00000048;
double				ppy_ = 0.00000048;
int					pnx_ = 1000;
int					pny_ = 1000;
double				offset_ = -1;
double				wave_ = 0.000000532;
//std::vector<float*> encoded_hologram_;
std::vector<float*> encoded_hologram_R_;
std::vector<float*> encoded_hologram_G_;
std::vector<float*> encoded_hologram_B_;
float*	holo_encoded_GPU_;
float*  holo_encoded_GPU_temp_;

GTensor *					reduce_source_;
GTensor *					reduce_min_;
GTensor *					reduce_max_;
GTensor *                   add_A_;
GTensor *                   add_B_;

unsigned char* normalized_hologram_;
unsigned char* holo_normalized_GPU_;

unsigned char*	buffer_;			// have the hologram output - real part
int buffer_size_;

void init();
void zeroinit();
void gen_hologram();
void loadData(std::vector<fvec3>& point_data);
void ClientConnect(QString IP);

//BJJ
int					CAMX = 60;
int					CAMY = 60;
int					SLMX = 2;
int					SLMY = 2;
int					ResX = 40;
int					ResY = 25;
float					Near = 0.01;
float					Far = 100;
//float					Near = 0.00001;
//float					Far = 10;
float					FOV = 28.42719453;

int xC, yC;
int UVx=0, UVy=0;
int SLMW=4096, SLMH=2160;
QImage simgR(SLMW, SLMH, QImage::Format::Format_Grayscale8);
QPainter sptrR(&simgR);
QImage simgG(SLMW, SLMH, QImage::Format::Format_Grayscale8);
QPainter sptrG(&simgG);
QImage simgB(SLMW, SLMH, QImage::Format::Format_Grayscale8);
QPainter sptrB(&simgB);

void main()
{
	init();
	sptrR.fillRect(simgR.rect(), Qt::black);
	sptrG.fillRect(simgG.rect(), Qt::black);
	sptrB.fillRect(simgB.rect(), Qt::black);
	//normalized_hologram_ = (unsigned char*)malloc(pnx_*pny_);
	//int n_server = 3;
	//char servername[100] = "192.168.10.4";
	//fscanf(,);
	//QString qqq1 = "LocalHost";
	//QString qqq1 = "192.168.10.188";

	//ClientConnect("192.168.0.167");
	//ClientConnect("192.168.0.187");
	ClientConnect("192.168.0.188");
	//ClientConnect("192.168.0.189");
	//ClientConnect("192.168.0.190");

	//ClientConnect("LocalHost");

	//_mkdir("C:/AuthHogel");
	//_mkdir("C:/AuthHogel/RED");
	//_mkdir("C:/AuthHogel/GREEN");
	//_mkdir("C:/AuthHogel/BLUE");

	//for (yC = 0; yC < CAMY; yC++)//one by one
	//{
	//	for (xC = 0; xC < CAMX; xC++)
	//	{
	//		//if (first)
	//		//{
	//		//	xS = 40;
	//		//	first = false;
	//		//}
	//		gen_hologram();
	//	}
	//}

	for (int yS = 0; yS < CAMY / SLMY; yS++)
	{
		for (int xS = 0; xS < CAMX / SLMX; xS++)
		{
			for (yC = yS * SLMY; yC < (yS + 1) * SLMY; yC++)
			{
				for (xC = xS * SLMX; xC < (xS + 1) * SLMX; xC++)
				{
					gen_hologram();
				}
			}
		}
	}
}

void init()
{
	if (!buffer_ || buffer_size_ != pnx_ * pny_ * 4) {
		buffer_size_ = pnx_ * pny_ * 4;
		free(buffer_);
		buffer_ = (unsigned char*)malloc(pnx_*pny_ * 4);
	}
	cudaSetDevice(0);
	if (!reduce_source_) reduce_source_ = new GTensor(1, 1, pnx_, pny_);
	if (!add_A_) add_A_ = new GTensor(1, 1, pnx_, pny_);
	if (!add_B_) add_B_ = new GTensor(1, 1, pnx_, pny_);
	if (!reduce_min_) reduce_min_ = new GTensor(1, 1, 1, 1);
	if (!reduce_max_) reduce_max_ = new GTensor(1, 1, 1, 1);
	if (!holo_encoded_GPU_) cudaMalloc(&holo_encoded_GPU_, pnx_ * pny_ * sizeof(float));
	if (!holo_encoded_GPU_temp_) cudaMalloc(&holo_encoded_GPU_temp_, pnx_ * pny_ * sizeof(float));
	if (!holo_normalized_GPU_) cudaMalloc(&holo_normalized_GPU_, pnx_ * pny_);

}

void zeroinit()
{
	reduce_source_ = 0;
	reduce_min_ = 0;
	reduce_max_ = 0;
	add_A_ = 0;
	add_B_ = 0;

	holo_encoded_GPU_ = 0;
	holo_encoded_GPU_temp_ = 0;
	holo_normalized_GPU_ = 0;
	//normalized_hologram_ = 0;

	buffer_ = 0;
	buffer_size_ = 0;
}

void gen_hologram()
{
	std::vector<fvec3> points_data;
	loadData(points_data);
	if (points_data.size() > 0) {
		int data_size = points_data.size() / 2;
		int split_size = data_size / holo_clients_.size();

		for (int i = 0; i < holo_clients_.size(); i++) {
			std::vector<fvec3> points;
			int s = i * split_size;
			int e = (i + 1)*split_size;
			if (e > data_size) e = data_size;
			for (int a = s; a < e; a++)
				points.push_back(points_data[a]);
			for (int a = data_size + s; a < data_size + e; a++)
				points.push_back(points_data[a]);
			//LOG("send point %f %f %f\n", points[0][0], points[0][1], points[0][2]);
			//LOG("%d set params begin\n", i);
			holo_clients_[i]->set_params(pnx_, pny_, ppx_, wave_, offset_, points, encoded_hologram_R_[i], encoded_hologram_G_[i], encoded_hologram_B_[i]);
			//LOG("%d set params end\n", i);
		}
		for (int i = 0; i < holo_clients_.size(); i++) {
			//LOG("%d start working begin\n", i);
			holo_clients_[i]->startWorking();
			//LOG("%d start working end\n", i);
		}
		for (int i = 0; i < holo_clients_.size(); i++) {
			//LOG("%d wait begin\n", i);
			holo_clients_[i]->waitForCompletion();
			//LOG("%d wait end\n", i);
			//LOG("%f %f\n", encoded_hologram_[i][0], encoded_hologram_[i][1]);
		}


		for (int c = 0; c < 3; c++)
		{
			//zeroinit();
			//init();
			cudaMemset(holo_encoded_GPU_, 0, pnx_*pny_ * sizeof(float));
			for (int i = 0; i < holo_clients_.size(); i++) {
				// each hologram generated from server, will be fed into temp
				if (c == 0)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_R_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);
				else if (c == 1)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_G_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);
				else if (c == 2)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_B_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);

				add_A_->set_mem(holo_encoded_GPU_temp_);
				add_B_->set_mem(holo_encoded_GPU_);
				add_B_->add(0, add_B_, 0.0, add_A_, 1.0, 1.0);
			}
			normalized_hologram_ = (unsigned char*)malloc(pnx_*pny_);

			int N = pnx_ * pny_;

			reduce_source_->set_mem(holo_encoded_GPU_);

			float* min_v_mem;
			float* max_v_mem;
			cudaMalloc(&min_v_mem, sizeof(float));
			cudaMalloc(&max_v_mem, sizeof(float));
			reduce_min_->set_mem(min_v_mem);
			reduce_max_->set_mem(max_v_mem);
			reduce_source_->min_reduce(0, reduce_min_, 1.0);
			reduce_source_->max_reduce(0, reduce_max_, 1.0);

			grey_normalize_api(holo_encoded_GPU_, holo_normalized_GPU_, min_v_mem, max_v_mem, pnx_, pny_);
			cudaMemcpy(normalized_hologram_, holo_normalized_GPU_, N * sizeof(uchar), cudaMemcpyDeviceToHost);

			QImage timg(normalized_hologram_, pnx_, pny_, QImage::Format::Format_Grayscale8);
			if (c == 0)
			{
				//sptrR.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * UVy, timg);//Z좌우반전 정순
				sptrR.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * (1-UVy), timg);//Z역순
				if (UVx == 1 && UVy == 1)
				{
					sptrR.end();
					//simgR.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/%1/CGH_%2_%3.bmp").arg(c).arg(yC / 2).arg(xC / 2), "BMP");
					//simgR.save(QString("D:/NewWolfsonImage/RED/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					simgR.save(QString("C:/Hogel/RED/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					sptrR.begin(&simgR);
				}
			}
			else if (c == 1)
			{
				//sptrG.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * UVy, timg);
				sptrG.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * (1 - UVy), timg);
				if (UVx == 1 && UVy == 1)
				{
					sptrG.end();
					//simgG.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/%1/CGH_%2_%3.bmp").arg(c).arg(yC / 2).arg(xC / 2), "BMP");
					//simgG.save(QString("D:/NewWolfsonImage/GREEN/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					simgG.save(QString("C:/Hogel/GREEN/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					sptrG.begin(&simgG);
				}
			}
			else if (c == 2)
			{
				//sptrB.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * UVy, timg);
				sptrB.drawImage(SLMW / 2 - pnx_ * UVx, 80 + pny_ * (1 - UVy), timg);
				if (UVx == 1 && UVy == 1)
				{
					sptrB.end();
					//simgB.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/%1/CGH_%2_%3.bmp").arg(c).arg(yC / 2).arg(xC / 2), "BMP");
					//simgB.save(QString("D:/NewWolfsonImage/BLUE/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					simgB.save(QString("C:/Hogel/BLUE/CGH_%1_%2.bmp").arg(yC / SLMY).arg(xC / SLMX), "BMP");
					sptrB.begin(&simgB);
				}
			}
			//QImage simg(normalized_hologram_, pnx_, pny_, QImage::Format::Format_Grayscale8);
			////simg.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/CGH_%1_%2.bmp").arg(yC).arg(xC), "BMP");
			//simg.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/%1/CGH_%2_%3.bmp").arg(c).arg(yC).arg(xC), "BMP");

			cudaFree(min_v_mem);
			cudaFree(max_v_mem);

			cudaFree(normalized_hologram_);
		}
	}
	if (UVx == 0 && UVy == 0)
	{
		UVx = 1;
	}
	else if (UVx == 1 && UVy == 0)
	{
		UVx = 0;
		UVy = 1;
	}
	else if (UVx == 0 && UVy == 1)
	{
		UVx = 1;
	}
	else if (UVx == 1 && UVy == 1)
	{
		UVx = 0;
		UVy = 0;
	}

	memset(buffer_, 0.0, pnx_*pny_ * 4);
	for (int k = 0; k < pnx_*pny_; k++) {

		uchar val = normalized_hologram_[k];
		buffer_[4 * k + 0] = val;
		buffer_[4 * k + 1] = val;
		buffer_[4 * k + 2] = val;
		buffer_[4 * k + 3] = 1;
	}
}
/*
void gen_hologram() //one by one
{
	std::vector<fvec3> points_data;
	loadData(points_data);
	if (points_data.size() > 0) {
		int data_size = points_data.size() / 2;
		int split_size = data_size / holo_clients_.size();

		for (int i = 0; i < holo_clients_.size(); i++) {
			std::vector<fvec3> points;
			int s = i * split_size;
			int e = (i + 1)*split_size;
			if (e > data_size) e = data_size;
			for (int a = s; a < e; a++)
				points.push_back(points_data[a]);
			for (int a = data_size + s; a < data_size + e; a++)
				points.push_back(points_data[a]);
			//LOG("send point %f %f %f\n", points[0][0], points[0][1], points[0][2]);
			//LOG("%d set params begin\n", i);
			holo_clients_[i]->set_params(pnx_, pny_, ppx_, wave_, offset_, points, encoded_hologram_R_[i], encoded_hologram_G_[i], encoded_hologram_B_[i]);
			//LOG("%d set params end\n", i);
		}
		for (int i = 0; i < holo_clients_.size(); i++) {
			//LOG("%d start working begin\n", i);
			holo_clients_[i]->startWorking();
			//LOG("%d start working end\n", i);
		}
		for (int i = 0; i < holo_clients_.size(); i++) {
			//LOG("%d wait begin\n", i);
			holo_clients_[i]->waitForCompletion();
			//LOG("%d wait end\n", i);
			//LOG("%f %f\n", encoded_hologram_[i][0], encoded_hologram_[i][1]);
		}


		for (int c = 0; c < 3; c++)
		{
			//zeroinit();
			//init();
			cudaMemset(holo_encoded_GPU_, 0, pnx_*pny_ * sizeof(float));
			for (int i = 0; i < holo_clients_.size(); i++) {
				// each hologram generated from server, will be fed into temp
				if (c == 0)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_R_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);
				else if (c == 1)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_G_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);
				else if (c == 2)
					cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_B_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);

				add_A_->set_mem(holo_encoded_GPU_temp_);
				add_B_->set_mem(holo_encoded_GPU_);
				add_B_->add(0, add_B_, 0.0, add_A_, 1.0, 1.0);
			}
			normalized_hologram_ = (unsigned char*)malloc(pnx_*pny_);

			int N = pnx_ * pny_;

			reduce_source_->set_mem(holo_encoded_GPU_);

			float* min_v_mem;
			float* max_v_mem;
			cudaMalloc(&min_v_mem, sizeof(float));
			cudaMalloc(&max_v_mem, sizeof(float));
			reduce_min_->set_mem(min_v_mem);
			reduce_max_->set_mem(max_v_mem);
			reduce_source_->min_reduce(0, reduce_min_, 1.0);
			reduce_source_->max_reduce(0, reduce_max_, 1.0);

			grey_normalize_api(holo_encoded_GPU_, holo_normalized_GPU_, min_v_mem, max_v_mem, pnx_, pny_);
			cudaMemcpy(normalized_hologram_, holo_normalized_GPU_, N * sizeof(uchar), cudaMemcpyDeviceToHost);

			QImage timg(normalized_hologram_, pnx_, pny_, QImage::Format::Format_Grayscale8);
			if (c == 0)
			{
				timg.save(QString("C:/AuthHogel/RED/%1.bmp").arg(yC*CAMX + xC), "BMP");
			}
			else if (c == 1)
			{
				timg.save(QString("C:/AuthHogel/GREEN/%1.bmp").arg(yC*CAMX + xC), "BMP");
			}
			else if (c == 2)
			{
				timg.save(QString("C:/AuthHogel/BLUE/%1.bmp").arg(yC*CAMX + xC), "BMP");
			}
			//QImage simg(normalized_hologram_, pnx_, pny_, QImage::Format::Format_Grayscale8);
			////simg.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/CGH_%1_%2.bmp").arg(yC).arg(xC), "BMP");
			//simg.save(QString("C:/Users/KETI-Sparrow/Documents/WolfsenLab_Point/MultiNode3/HologramRealTime_MultiNode/Test/%1/CGH_%2_%3.bmp").arg(c).arg(yC).arg(xC), "BMP");

			cudaFree(min_v_mem);
			cudaFree(max_v_mem);

			cudaFree(normalized_hologram_);
		}
	}
	memset(buffer_, 0.0, pnx_*pny_ * 4);
	for (int k = 0; k < pnx_*pny_; k++) {

		uchar val = normalized_hologram_[k];
		buffer_[4 * k + 0] = val;
		buffer_[4 * k + 1] = val;
		buffer_[4 * k + 2] = val;
		buffer_[4 * k + 3] = 1;
	}
}*/

void loadData(std::vector<fvec3>& point_data)
{
	//graphics::vec2 pp(ppx_, ppy_);
	//graphics::ivec2 pn(pnx_, pny_);

	std::vector<graphics::fvec3>	positions;
	std::vector<graphics::fvec3>	intensities;

	float aspectRatio = (float)ResX / ResY;
	//for (int yS = 0; yS < CAMY / SLMY; yS++)
	//{
	//	for (int xS = 0; xS < CAMX / SLMX; xS++)
	//	{
	//		for (int y = yS * SLMY; y < (yS + 1) * SLMY; y++)
	//		{
	//			for (int x = xS * SLMX; x < (xS + 1) * SLMX; x++)
	//			{
					//if (!isPointSending)
					//	goto End;
	//QImage colorMap(QString("C:/Users/KETI-Sparrow/Documents/Hogels/RenderRGBD_Chair6/COLOR/Color_%1_%2.png").arg(yC).arg(xC), "PNG");
	//QImage depthMap(QString("C:/Users/KETI-Sparrow/Documents/Hogels/RenderRGBD_Chair6/DEPTH/Depth_%1_%2.png").arg(yC).arg(xC), "PNG");
	QImage colorMap(QString("C:/Hogel/Auth/COLOR/Color_%1_%2.png").arg(yC).arg(xC), "PNG");
	QImage depthMap(QString("C:/Hogel/Auth/DEPTH/Depth_%1_%2.png").arg(yC).arg(xC), "PNG");
	//QImage colorMap(QString("C:/Users/KETI-Sparrow/Documents/Hogels/2021-09-10-150127/COLOR/Color_%1_%2.png").arg(yC).arg(xC), "PNG");
	//QImage depthMap(QString("C:/Users/KETI-Sparrow/Documents/Hogels/2021-09-10-150127/DEPTH/Depth_%1_%2.png").arg(yC).arg(xC), "PNG");

	//int pointNum = 0;

	//std::ofstream csvR, csvG, csvB;
	//std::string path;
	//path<< "C:/csv/RED/CGH_" << yC * CAMX + xC << ".csv" << endl;
	//csvR.open("C:/csv/RED/" + std::to_string(yC * CAMX + xC) + ".csv");
	//csvG.open("C:/csv/GREEN/" + std::to_string(yC * CAMX + xC) + ".csv");
	//csvB.open("C:/csv/BLUE/" + std::to_string(yC * CAMX + xC) + ".csv");
	//csvG.open("C:/csv/GREEN/CGH_%1.csv".arg(yC*CAMX + xC));
	//csvB.open("C:/csv/BLUE/CGH_%1.csv"arg(yC*CAMX + xC));
	//csvR << "x,y,z,intensity\n";
	//csvG << "x,y,z,intensity\n";
	//csvB << "x,y,z,intensity\n";

	for (int yR = 0; yR < ResY; yR++)
	{
		for (int xR = 0; xR < ResX; xR++)
		{
			QColor cp = colorMap.pixel(xR, yR);
			QColor dp = depthMap.pixel(xR, yR);
			if (dp.redF() != 0)// && cp.blackF != 0)
			{
				float zValue = ((0.5 - dp.redF()) * 2 * (Far - Near) + Near) * 0.001;
				fvec3 fp(
					((float)(xR * 2 + 1) / ResX - 1) *qTan(qDegreesToRadians(FOV * aspectRatio / 2)) * zValue,
					-((float)(yR * 2 + 1) / ResY - 1) * qTan(qDegreesToRadians(FOV / 2)) * zValue,
					zValue);
				positions.push_back(fp);
				intensities.push_back(graphics::fvec3(cp.redF(), cp.greenF(), cp.blueF()));
				//csvR << fp[0] << "," << fp[1] << "," << fp[2] << "," << cp.redF() << "\n";
				//csvG << fp[0] << "," << fp[1] << "," << fp[2] << "," << cp.greenF() << "\n";
				//csvB << fp[0] << "," << fp[1] << "," << fp[2] << "," << cp.blueF() << "\n";
			}
		}
	}
	//csvR.close();
	//csvG.close();
	//csvB.close();
	//			}
	//		}
	//	}
	//}

	for (auto& i : positions) {
		point_data.push_back(graphics::fvec3(i.v[0], i.v[1], i.v[2]));// + kBaseLen * pp[0] * kDefaultDistance));
		if (i.v[0] != 0.0) {
			int kkk = 0;
		}
	}
	for (auto& i : intensities) {
		point_data.push_back(i);
	}
}

void ClientConnect(QString IP)
{
	client_thread* new_th2 = new client_thread(IP);
	holo_clients_.push_back(new_th2);
	encoded_hologram_R_.push_back((float*)malloc(pnx_*pny_ * sizeof(float)));
	encoded_hologram_G_.push_back((float*)malloc(pnx_*pny_ * sizeof(float)));
	encoded_hologram_B_.push_back((float*)malloc(pnx_*pny_ * sizeof(float)));
}