#pragma once

#include <graphics/sys.h>
#include <graphics/texture_buffer.h>
#include <algorithm>
#include <chrono>
#include <QtGui/QImage.h>

#include "Hologram/HologramGenerator.h"
#include "model/ImportedModel.h"
#include "model/AnimCharacter.h"
#include "material/GMaterial.h"
#include "OglRenderShader.h"
#include "graphics/Timer.h"
#include "graphics/camera.h"
#include "graphics/fvec.h"
#include <holo_client/holo_client.h>
const float kDefaultDistance = 10.0;
const float kBaseLen = 300.0;
const int kSamplingNum = 300;

class HologramRender
{

public:

	HologramRender();
	~HologramRender() {}

	static void writefloatimage(const char* fileName, int nx, int ny, int ch, float* intensity, int upsidedown = 0);

	int getWidth() { return pnx_;  }
	int getHeight() { return pny_; }
	uchar* getBuffer() { return buffer_;  }

	void setCamera(const graphics::Camera& cam);

	void setModel(model_kernel::ImportedModel* m) { model_ = m; }
	void setCharacter(model_kernel::AnimCharacter* c) { character_ = c; }
	void readConfig();
	void initialize();

	void render();
	void reconstruct_fromCGH();

protected:

	void gen_hologram();
	void loadData(std::vector<fvec3>& point_data);

	void recreate_inputdata(float maxZ, float minZ, std::vector<graphics::fvec3>& positions);

	void beginDraw();
	void draw();
	void endDraw();
	void drawOnePass(bool opaque);
	void setFrontLight();

	void read_depth(QString fname);
	void read_color(QString fname);

protected:

    // TCP Connections to each server
    std::vector<client_thread*> holo_clients_;
	//HoloClient holo_client_;
	unsigned char*	buffer_;			// have the hologram output - real part
	int buffer_size_;
	unsigned char* normalized_hologram_;
    std::vector<float*> encoded_hologram_;
    float*	holo_encoded_GPU_;
    float*  holo_encoded_GPU_temp_;
    unsigned char* holo_normalized_GPU_;

	graphics::TextureBuffer* scene_;
	
	model_kernel::ImportedModel* model_;
	model_kernel::AnimCharacter* character_;
	graphics::Camera* camera_;

	unsigned int character_vbo_;
	std::vector<model_kernel::GMaterial*> character_materials_;
	std::vector<ivec2> character_range_;

	graphics::Timer timer_;

	HologramGenerator*	hologram_;
	bool				is_GPU_;

	float*				src_depth_;			
	unsigned char*		src_color_;

	QString				depthfname_;
	QString				colorfname_;
	QString				cghfname_;
	QString				cghrealfname_;
	QString				cghimgfname_;
	QString				reconfname_;

	bool				save_src_file_;
	bool				save_output_file_;
	bool				do_recon_;


	double				ppx_;
	double				ppy_;
	int					pnx_;
	int					pny_;
	double				offset_;
	double				wave_;

    GTensor *					reduce_source_;
    GTensor *					reduce_min_;
    GTensor *					reduce_max_;
    GTensor *                   add_A_;
    GTensor *                   add_B_;

};
