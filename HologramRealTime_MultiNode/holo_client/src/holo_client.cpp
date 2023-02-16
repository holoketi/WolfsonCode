#include "holo_client/holo_client.h"
#include <qtcore/qbuffer.h>



client_thread::client_thread(QString& name)
    :invoked_(false), stopped_(false), mutex_(), cond_(), host_name_(name)
{
    tcpSocket_ = 0;
    //output_buffer_ = 0;
	output_buffer_R_=0;
	output_buffer_G_=0;
	output_buffer_B_=0;

    thread_ = std::thread(std::bind(&client_thread::run, this));
}

client_thread::~client_thread() { thread_.join(); }


void client_thread::run()
{

    while (!stopped_) {
        std::unique_lock<std::mutex> lck(mutex_);


        while (!invoked_) {
            cond_.wait(lck);
        }

        lck.unlock();

        // connect to server
        if (tcpSocket_ == 0) {
            tcpSocket_ = new QTcpSocket;
            if (host_name_.compare("LocalHost") == 0)
                tcpSocket_->connectToHost(QHostAddress::LocalHost, 6178);
            else
                tcpSocket_->connectToHost(host_name_, 6178);
            if (tcpSocket_->state() != QAbstractSocket::ConnectedState)
                tcpSocket_->waitForConnected();
        }
        else if (tcpSocket_->state() == QAbstractSocket::UnconnectedState)
        {
            if (host_name_.compare("LocalHost") == 0)
                tcpSocket_->connectToHost(QHostAddress::LocalHost, 6178);
            else
                tcpSocket_->connectToHost(host_name_, 6178);
            if (tcpSocket_->state() != QAbstractSocket::ConnectedState)
                tcpSocket_->waitForConnected();
        }

        // work_here
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_3);

        out.writeRawData((const char*)&cmd_, sizeof(cmd_));
        //_cprintf("chan %d, pnx %d pny %d pitch %f, wl %f, offset %f, cnt %d\n", 
        //    cmd_.channel, cmd_.pnx, cmd_.pny, cmd_.pixel_pitch_x, cmd_.wave_len_, cmd_.offset_, cmd_.point_cnt);

        int sz = points_intensities_.size() * sizeof(graphics::fvec3);
        out.writeRawData((const char*)points_intensities_.data(), sz);

        tcpSocket_->write(block);
        tcpSocket_->flush();

        while (true) {
            tcpSocket_->waitForReadyRead();
            QDataStream in(tcpSocket_);
            in.setVersion(QDataStream::Qt_5_3);

            if (tcpSocket_->bytesAvailable() < cmd_.pnx*cmd_.pny*sizeof(float)) continue;

			in.readRawData((char*)output_buffer_R_, cmd_.pnx*cmd_.pny * sizeof(float));
			tcpSocket_->flush();
 
            break;
        }

		while (true) {
			tcpSocket_->waitForReadyRead();
			QDataStream in(tcpSocket_);
			in.setVersion(QDataStream::Qt_5_3);

			if (tcpSocket_->bytesAvailable() < cmd_.pnx*cmd_.pny * sizeof(float)) continue;

			in.readRawData((char*)output_buffer_G_, cmd_.pnx*cmd_.pny * sizeof(float));
			tcpSocket_->flush();

			break;
		}

		while (true) {
			tcpSocket_->waitForReadyRead();
			QDataStream in(tcpSocket_);
			in.setVersion(QDataStream::Qt_5_3);

			if (tcpSocket_->bytesAvailable() < cmd_.pnx*cmd_.pny * sizeof(float)) continue;

			in.readRawData((char*)output_buffer_B_, cmd_.pnx*cmd_.pny * sizeof(float));
			tcpSocket_->flush();

			break;
		}
        //_cprintf("%s finish working\n",host_name_.toStdString().c_str());
        //tcpSocket_->disconnectFromHost();
        tcpSocket_->close();
        //if (tcpSocket_->state() == QAbstractSocket::ConnectedState)
        //    tcpSocket_->waitForDisconnected();
        lck.lock();
        invoked_ = false;
        cond_.notify_all();
        lck.unlock();
    }

    tcpSocket_->disconnectFromHost();

    if (tcpSocket_->state() == QAbstractSocket::ConnectedState)
        tcpSocket_->waitForDisconnected();
}
void client_thread::set_params(int pnx, int pny,
    float pixel_pitch, float wave_len, float offset, 
    std::vector<graphics::fvec3>& points_intensity, float* outR, float* outG, float* outB)
{
    std::unique_lock<std::mutex> lck(mutex_);
    while (invoked_) {
        cond_.wait(lck);
    }
    cmd_.channel = 1;
    cmd_.pnx = pnx;
    cmd_.pny = pny;
    cmd_.pixel_pitch_x = cmd_.pixel_pitch_y = pixel_pitch;
    cmd_.wave_len_ = wave_len;
    cmd_.offset_ = offset;
    cmd_.point_cnt = points_intensity.size() / 2;
	output_buffer_R_ = outR;
	output_buffer_G_ = outG;
	output_buffer_B_ = outB;
    if (cmd_.point_cnt == 0) {
		memset(outR, 0, pnx*pny * sizeof(float));
		memset(outG, 0, pnx*pny * sizeof(float));
		memset(outB, 0, pnx*pny * sizeof(float));
    }
    points_intensities_ = points_intensity;
}
void client_thread::startWorking()
{
    std::unique_lock<std::mutex> lck(mutex_);
    while (invoked_) {
        cond_.wait(lck);
    }
    invoked_ = true;
    cond_.notify_all();
}
void client_thread::waitForCompletion()
{
    std::unique_lock<std::mutex> lck(mutex_);
    while (invoked_) {
        cond_.wait(lck);
    }
}
void client_thread::stop()
{
    std::unique_lock<std::mutex> lck(mutex_);
    stopped_ = true;
}
