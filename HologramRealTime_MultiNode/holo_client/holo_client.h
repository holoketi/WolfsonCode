#ifndef NDISPLAYCONTROLLER_H
#define NDISPLAYCONTROLLER_H

#include	<QtNetwork/qtcpsocket.h>
#include	<QtNetwork/qhostaddress.h>
#include	<QtCore/qobject.h>
#include	<qtgui/qcolor.h>
#include	<qtgui/qimage.h>
#include	<graphics/fvec.h>

#include	<conio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
typedef struct {
	int			channel;
	int			pnx;
	int			pny;
	float       pixel_pitch_x;
	float		pixel_pitch_y;
	float		wave_len_;
	float		offset_;
	int			point_cnt;

}  Host_Command;



class client_thread {

public:
    client_thread(QString& name);

    virtual ~client_thread();

    void set_hostname(QString& name) { host_name_ = name; }

    void set_params(int pnx, int pny,
        float pixel_pitch, float wave_len, float offset_, 
        std::vector<graphics::fvec3>& points_intensity,
		float* outR,
		float* outG,
		float* outB);

    void startWorking();
    void waitForCompletion();
    void stop();
private:
    client_thread() {}
    void run();

    QTcpSocket*	tcpSocket_;
    std::thread thread_;
    QString host_name_;
    Host_Command  cmd_;
	float* output_buffer_R_;
	float* output_buffer_G_;
	float* output_buffer_B_;
    std::vector<graphics::fvec3> points_intensities_;
    std::mutex mutex_;
    std::condition_variable cond_;
    volatile bool invoked_;
    volatile bool stopped_;
};


#endif // NDISPLAY_CONTROLLER_H
