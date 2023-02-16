#include "HologramRender.h"
#include <graphics/sys.h>
#include <graphics/gl.h>
#include <graphics/RenderEnv.h>
#include <QtCore/QDir>
#include <QtGui/QImage.h>
#include <fstream>

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

using namespace model_kernel;

HologramRender::HologramRender() : model_(0), character_(0), holo_clients_(), encoded_hologram_()
{
    reduce_source_ = 0;
    reduce_min_ = 0;
    reduce_max_ = 0;
    add_A_ = 0;
    add_B_ = 0;

    holo_encoded_GPU_ = 0;
    holo_encoded_GPU_temp_ = 0;
    holo_normalized_GPU_ = 0;
    normalized_hologram_ = 0;


	save_src_file_  = true;
	save_output_file_  = true;
	do_recon_  = false;

	hologram_ = 0;
	src_depth_ = 0;
	src_color_ = 0;
	scene_ = 0;

	buffer_ = 0;
	buffer_size_ = 0;

	camera_ = 0;

	timer_.start();
	
	is_GPU_ = true;
	hologram_ = new HologramGenerator();
}

void HologramRender::readConfig()
{
	//ppx_ = 0.0000043;
	//ppy_ = 0.0000043;

	//pnx_ = 400;
	//pny_ = 400;

	////offset_ = 0.01806;
	//wave_ = 5.32e-7;


	std::string inputFileName_ = "config_hologram.txt";
	
	LOG("Reading....%s\n", inputFileName_.c_str());

	std::ifstream inFile(inputFileName_.c_str());

	if (!inFile.is_open()) {
		LOG("file not found.\n");
		return;
	}

	// skip 7 lines
	std::string temp;

	inFile >> temp;					
	
	QString hdir = QString("").fromStdString(temp);
	getline(inFile, temp, '\n');

	inFile >> wave_;				getline(inFile, temp, '\n');
	inFile >> pnx_;					getline(inFile, temp, '\n');
	inFile >> pny_;					getline(inFile, temp, '\n');
	inFile >> ppx_;					getline(inFile, temp, '\n');
	inFile >> ppy_;					getline(inFile, temp, '\n');
	inFile >> offset_;				getline(inFile, temp, '\n');
	inFile >> save_src_file_;		getline(inFile, temp, '\n');
	inFile >> save_output_file_;	getline(inFile, temp, '\n');
	

	inFile.close();

	LOG("done\n");

	//=====================================================================================
	QDir dir(hdir);
	if (!dir.exists())
	{
		QDir d;
		d.mkpath(hdir);
	}

	depthfname_ = hdir + "/PointCloud_Src_Depth.jpg";
	colorfname_ = hdir + "/PointCloud_Src_Color.jpg";
	cghfname_ = hdir + "/PointCloud_cgh.jpg";
	cghrealfname_ = hdir + "/PointCloud_cgh_real.jpg";
	cghimgfname_ = hdir + "/PointCloud_cgh_img.jpg";
	reconfname_ = hdir + "/PointCloud_recon.jpg";

	// if offset == -1 than use the auto-generated value- kBaseLen * ppx_ * kDefaultDistance + (pnx_ * ppx_) / 2.0
	if (offset_ == -1 )
		offset_ = kBaseLen * ppx_ * kDefaultDistance + (pnx_ * ppx_) / 2.0;
	normalized_hologram_ = (unsigned char*)malloc(pnx_*pny_);

    // now read server config

    FILE* fp = fopen("config_servers.txt", "rt");
    if (!fp) return;
    int n_server;
    fscanf(fp, "%d\n", &n_server);
    for (int i = 0; i < n_server; i++) {
        char server_name[1000];
        fscanf(fp, "%s\n", server_name);
        client_thread* new_th = new client_thread(QString(server_name));
        holo_clients_.push_back(new_th);
        //new_th->set_hostname(QString(server_name));
        encoded_hologram_.push_back((float*)malloc(pnx_*pny_ * sizeof(float)));
    }
    fclose(fp);
}

void HologramRender::initialize()
{
	if (!scene_) {
		scene_ = new TextureBuffer();
		scene_->GenerateObjects();
	}
	scene_->Resize(pnx_, pny_);

	if (!buffer_ || buffer_size_ != pnx_*pny_ * 4) {
		buffer_size_ = pnx_*pny_* 4;
		free(buffer_);
		buffer_ = (unsigned char*)malloc(pnx_*pny_ * 4);
	}

	//hologram_->setMode(!is_GPU_);

	graphics::vec2 pp = graphics::vec2(ppx_, ppy_);
	graphics::ivec2 pn = graphics::ivec2(pnx_, pny_);

    cudaSetDevice(0);
    if (!reduce_source_) reduce_source_ = new GTensor(1, 1, pnx_, pny_);
    if (!add_A_) add_A_ = new GTensor(1, 1, pnx_, pny_);
    if (!add_B_) add_B_ = new GTensor(1, 1, pnx_, pny_);
    if (!reduce_min_) reduce_min_ = new GTensor(1, 1, 1, 1);
    if (!reduce_max_) reduce_max_ = new GTensor(1, 1, 1, 1);
    if (!holo_encoded_GPU_) cudaMalloc(&holo_encoded_GPU_, pnx_ * pny_* sizeof(float));
    if (!holo_encoded_GPU_temp_) cudaMalloc(&holo_encoded_GPU_temp_, pnx_ * pny_ * sizeof(float));
    if (!holo_normalized_GPU_) cudaMalloc(&holo_normalized_GPU_, pnx_ * pny_);

	//hologram_->setConfig_PointCloud(offset_, pp, pn, wave_);
}

void HologramRender::render()
{
	gl_stat stat;
	stat.save_stat();

	if (!kOglRenderShader) {
		kOglRenderShader = new OglRenderShader();
		kOglRenderShader->Initialize();
	}

	scene_->BeginDraw();
	beginDraw();
	draw();

	read_color(colorfname_);
	read_depth(depthfname_);

	endDraw();
	scene_->EndDraw();
	
	gen_hologram();

	stat.restore_stat();

}

void HologramRender::draw()
{
	if (character_ && character_->hasBody()) {
		character_vbo_ = character_->getVBO();
		std::vector<AnimVertexBufferData> data;
		float t = timer_.getElapsedTimeInSec();

		character_->getAnimVertexBufferData(t, data, character_materials_, character_range_);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, character_vbo_);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(AnimVertexBufferData) * data.size(), &data[0]);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	float projMat[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projMat);

	float viewMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, viewMat);

	kOglRenderShader->setProjandViewMat(projMat, viewMat);

	setFrontLight();

	user::Matrix4 aa = user::Matrix4::identity();
	kOglRenderShader->setModelMat(aa.array());

	//aa.rotateX(radian(rotx_));
	//aa.scale(user::Vector3(scale_, scale_, scale_));
	//kOglRenderShader->setModelMat(aa.array());
	//kOglRenderShader->setModelMat(render_instances[i].transform().array());
	   
	drawOnePass(true);
	drawOnePass(false);
}

static void draw_by_shader(GMaterial* material, bool opaque_only, RelativeMemoryManager* ret) {

	if (opaque_only) {
		if (material->IsTransparent()) return;
	}
	else { // draw only_transparent
		if (!material->IsTransparent()) return;
	}

	bool hasDiffuseTexture = material->HasDiffuseTexture();
	bool isTransparent = material->IsTransparent();
	bool hasBumpmapping = material->HasBumpmappingTexture();
	bool hasReflectiveTexture = material->HasReflectiveTexture();
	bool hasTexture = false;
	if (hasDiffuseTexture || hasReflectiveTexture || hasBumpmapping)
		hasTexture = true;

	color diffuse_c = material->GetDiffuse();
	color ambient_c = material->GetAmbient();
	color specular_c = material->GetSpecular();
	color emission_c = material->GetEmission();
	float shininess = material->GetShininess();

	float diffuse[3], ambient[3], specular[3], emission[3];

	for (int i = 0; i < 3; i++)
	{
		diffuse[i] = diffuse_c[i];
		ambient[i] = ambient_c[i];
		specular[i] = specular_c[i];
		emission[i] = emission_c[i];
	}

	// tranparency ==================================================
	if (isTransparent) {

		color blend_color(1);
		color transparent = material->GetTransparent();
		real  transparency = material->GetTransparency();
		blend_color[0] = transparency * transparent[0];
		blend_color[1] = transparency * transparent[1];
		blend_color[2] = transparency * transparent[2];
		blend_color[3] = transparency * (transparent[0] * 0.212671 +
			transparent[1] * 0.715160 +
			transparent[2] * 0.072169);

		glEnable(GL_BLEND);
		glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
		glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);

	}
	else {
		glDisable(GL_BLEND);
	}

	kOglRenderShader->setMaterialProperties(hasDiffuseTexture, isTransparent, hasBumpmapping, hasReflectiveTexture, hasTexture,
		emission, ambient, diffuse, specular, shininess);

	if (hasDiffuseTexture)
		kOglRenderShader->SetDiffuseColorTexture(material->GetUserTextureDiffuse()->GetTextureId());

	// Bump Mapping
	if (hasBumpmapping)
		kOglRenderShader->SetBumpMapTexture(material->GetUserTextureBumpmapping()->GetTextureId());


	kOglRenderShader->BeginShader();

	//glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	int r = ret->getRange();
	glDrawElements(GL_TRIANGLES, r * 3, GL_UNSIGNED_INT, BUFFER_OFFSET(ret->baseAddr() * EBO_ELEM_SIZE));

	kOglRenderShader->EndShader();

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLenum err = glGetError();
}

static void draw_char_by_shader(GMaterial* material, bool opaque_only, ivec2 ret) {

	if (opaque_only) {
		if (material->IsTransparent()) return;
	}
	else { // draw only_transparent
		if (!material->IsTransparent()) return;
	}


	bool hasDiffuseTexture = material->HasDiffuseTexture();
	bool isTransparent = material->IsTransparent();
	bool hasBumpmapping = material->HasBumpmappingTexture();
	bool hasReflectiveTexture = material->HasReflectiveTexture();
	bool hasTexture = false;
	if (hasDiffuseTexture || hasReflectiveTexture || hasBumpmapping)
		hasTexture = true;

	color diffuse_c = material->GetDiffuse();
	color ambient_c = material->GetAmbient();
	color specular_c = material->GetSpecular();
	color emission_c = material->GetEmission();
	float shininess = material->GetShininess();

	float diffuse[3], ambient[3], specular[3], emission[3];

	for (int i = 0; i < 3; i++)
	{
		diffuse[i] = diffuse_c[i];
		ambient[i] = ambient_c[i];
		specular[i] = specular_c[i];
		emission[i] = emission_c[i];
	}

	// tranparency ==================================================
	if (isTransparent) {

		color blend_color(1);
		color transparent = material->GetTransparent();
		real  transparency = material->GetTransparency();
		blend_color[0] = transparency * transparent[0];
		blend_color[1] = transparency * transparent[1];
		blend_color[2] = transparency * transparent[2];
		blend_color[3] = transparency * (transparent[0] * 0.212671 +
			transparent[1] * 0.715160 +
			transparent[2] * 0.072169);

		glEnable(GL_BLEND);
		glBlendColor(blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
		glBlendFunc(GL_ONE_MINUS_CONSTANT_COLOR, GL_CONSTANT_COLOR);

	}
	else {
		glDisable(GL_BLEND);
	}

	kOglRenderShader->setMaterialProperties(hasDiffuseTexture, isTransparent, hasBumpmapping, hasReflectiveTexture, hasTexture,
		emission, ambient, diffuse, specular, shininess);

	if (hasDiffuseTexture)
		kOglRenderShader->SetDiffuseColorTexture(material->GetUserTextureDiffuse()->GetTextureId());

	// Bump Mapping
	if (hasBumpmapping)
		kOglRenderShader->SetBumpMapTexture(material->GetUserTextureBumpmapping()->GetTextureId());

	kOglRenderShader->BeginShader();

	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDrawArrays(GL_TRIANGLES, ret[0], ret[1] - ret[0] + 1);
	kOglRenderShader->EndShader();

	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLenum err = glGetError();

}


void HologramRender::drawOnePass(bool opaque)
{
	std::vector<RelativeMemoryManager*> ret = model_->getAllRelativeMemoryManager();
	std::vector<GMaterial*> materials = model_->getAllMaterial();

	GLenum err = glGetError();

	GLuint	arrays;
	glGenVertexArrays(1, &arrays);
	glBindVertexArray(arrays);

	int position_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "vPosition");
	int normal_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "vNormal");
	int textcoord_loc = glGetAttribLocation(kOglRenderShader->getShaderId(), "in_tex_coord");

	glBindBuffer(GL_ARRAY_BUFFER, kSyncGpuMemory->vbo());
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, VBO_ELEM_SIZE, 0);
	glEnableVertexAttribArray(position_loc);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, VBO_ELEM_SIZE, NORMAL_OFFSET);
	glEnableVertexAttribArray(normal_loc);
	glVertexAttribPointer(textcoord_loc, 2, GL_FLOAT, GL_TRUE, VBO_ELEM_SIZE, TEXTURE_OFFSET);
	glEnableVertexAttribArray(textcoord_loc);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kSyncGpuMemory->ebo());

	for (size_t i = 0; i < ret.size(); i++) {

		draw_by_shader(materials[i], opaque, ret[i]);

	}

	err = glGetError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDeleteVertexArrays(1, &arrays);

	if (character_ && character_->hasBody()) {

		glBindVertexArray(arrays);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, character_vbo_);
		glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 32, 0);
		glEnableVertexAttribArray(position_loc);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));
		glEnableVertexAttribArray(normal_loc);
		glVertexAttribPointer(textcoord_loc, 2, GL_FLOAT, GL_TRUE, 32, BUFFER_OFFSET(24));
		glEnableVertexAttribArray(textcoord_loc);

		//kOglRenderShader->setModelMat(character_render_instances[i].transform().array());

		for (size_t j = 0; j < character_materials_.size(); j++) {

			draw_char_by_shader(character_materials_[j],
				opaque,
				character_range_[j]);

		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glDeleteVertexArrays(1, &arrays);
}

void HologramRender::gen_hologram()
{
	std::vector<fvec3> points_data;
	loadData(points_data);
    if (points_data.size() > 100) {
		int data_size = points_data.size() / 2;
		int split_size = data_size / holo_clients_.size();
        
        for (int i = 0; i < holo_clients_.size(); i++) {
            std::vector<fvec3> points;
            int s = i * split_size;
            int e = (i + 1)*split_size;
            if (e > data_size) e = data_size;
            for (int a = s; a < e; a++)
                points.push_back(points_data[a]);
			for (int a = data_size+s; a < data_size+e; a++)
				points.push_back(points_data[a]);
            //LOG("send point %f %f %f\n", points[0][0], points[0][1], points[0][2]);
            //LOG("%d set params begin\n", i);
//            holo_clients_[i]->set_params(pnx_, pny_, ppx_, wave_, offset_, points, encoded_hologram_[i]);
            //LOG("%d set params end\n", i);
        }
        for (int i = 0; i < holo_clients_.size(); i++) {
            //LOG("%d start working begin\n", i);
            holo_clients_[i]->startWorking();
            //LOG("%d start working end\n", i);
        }
        cudaMemset(holo_encoded_GPU_, 0, pnx_*pny_ * sizeof(float));
        for (int i = 0; i < holo_clients_.size(); i++) {
            //LOG("%d wait begin\n", i);
            holo_clients_[i]->waitForCompletion();
            //LOG("%d wait end\n", i);
            //LOG("%f %f\n", encoded_hologram_[i][0], encoded_hologram_[i][1]);
        }

        
        for (int i = 0; i < holo_clients_.size(); i++) {
            // each hologram generated from server, will be fed into temp
            cudaMemcpy(holo_encoded_GPU_temp_, encoded_hologram_[i], pnx_*pny_ * sizeof(float), cudaMemcpyKind::cudaMemcpyHostToDevice);

            add_A_->set_mem(holo_encoded_GPU_temp_);
            add_B_->set_mem(holo_encoded_GPU_);
            add_B_->add(0, add_B_, 0.0, add_A_, 1.0, 1.0);
        }

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
        cudaFree(min_v_mem);
        cudaFree(max_v_mem);
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

void HologramRender::loadData(std::vector<fvec3>& point_data)
{
	double zNear = camera_->get_near();
	double zFar = camera_->get_far();

	graphics::vec2 pp(ppx_, ppy_);
	graphics::ivec2 pn(pnx_, pny_);

	std::vector<graphics::fvec3>	positions;
	std::vector<graphics::fvec3>	intensities;

	float epsilon_t = (zFar - zNear) / 100.0;

	float maxZ = _MINFLOAT;
	float minZ = _MAXFLOAT;

	frame cam_pose = camera_->getCameraPose();

	int intv = pnx_ / kSamplingNum;

	for (int x = 0; x < pnx_; x+= intv) {
		for (int y = 0; y < pny_; y+= intv) {

			float d = src_depth_[x + y * pnx_];

			if (apx_equal((double)d, zFar, (double)epsilon_t) || d > zFar) continue;

			graphics::line ray;
			camera_->getViewRayLocal(vec2(x, y), ray);
			//vec3 dir = cam_pose.to_model_normal(ray.get_direction());
			vec3 dir = ray.get_direction();
			vec3 p = (d / dir[2])*dir;
			fvec3 fp(p[0], p[1], p[2]);
			positions.push_back(fp);
			maxZ = p(2) > maxZ ? p(2) : maxZ;
			minZ = p(2) < minZ ? p(2) : minZ;

			graphics::vec3 v = src_color_[x + y * pnx_];
			fvec3 c_val = graphics::fvec3((float)v(0) / 255.0, (float)v(1) / 255.0, (float)v(2) / 255.0);
			intensities.push_back(c_val);
		}
	}

	recreate_inputdata(maxZ, minZ, positions);

	/*int npoints = positions.size();
	HologramData_PointCloud* data = hologram_->getPCData();
	data->n_channels = 3;
	data->n_points = npoints;

	if (data->ObjPosition)	delete[] data->ObjPosition;
	data->ObjPosition = new float[3 * npoints];

	if (data->ObjIntensity)	delete[] data->ObjIntensity;
	data->ObjIntensity = new float[3 * npoints];
	
	for (int i = 0; i < npoints; i++)
	{
		data->ObjPosition[3 * i + 0] = positions[i].v[0];
		data->ObjPosition[3 * i + 1] = positions[i].v[1];
		data->ObjPosition[3 * i + 2] = positions[i].v[2] + kBaseLen * pp[0] * kDefaultDistance;

		data->ObjIntensity[3 * i + 0] = intensities[i].v[0];
		data->ObjIntensity[3 * i + 1] = intensities[i].v[1];
		data->ObjIntensity[3 * i + 2] = intensities[i].v[2];

	}
	*/
	for (auto& i : positions) {
		point_data.push_back(graphics::fvec3(i.v[0], i.v[1], i.v[2] + kBaseLen*pp[0] * kDefaultDistance));
		if (i.v[0] != 0.0) {
			int kkk = 0;
		}
	}
	for (auto& i : intensities) {
		point_data.push_back(i);
	}
}

void HologramRender::recreate_inputdata(float maxZ, float minZ, std::vector<graphics::fvec3>& positions)
{
	
	graphics::vec2 pp(ppx_, ppy_);
	float midZ = (maxZ + minZ) / 2.0;

	graphics::line ray;
	box3 bound;
	frame& cam_frame = camera_->getCameraPose();

	camera_->getViewRayLocal(vec2(0, 0), ray);
	vec3 dir = ray.get_direction();
	//dir = cam_frame.to_model_normal(dir);
	vec3 p1 = (midZ / dir[2])*dir;
	vec3 p2 = (minZ / dir[2])*dir;
	bound.extend(p1);
	bound.extend(p2);

	camera_->getViewRayLocal(vec2(pnx_- 1, pny_- 1), ray);
	dir = ray.get_direction();
	//dir = cam_frame.to_model_normal(dir);
	p1 = (midZ / dir[2])*dir;
	p2 = (minZ / dir[2])*dir;
	bound.extend(p1);
	bound.extend(p2);

	float sizeX = bound.maximum[0] - bound.minimum[0];
	float scaleF = pnx_ * pp[0] / sizeX;

	for (int i = 0; i < positions.size(); i++)
	{
		fvec3 pos = positions[i];
		pos[0] = pos[0] * scaleF;
		pos[1] = pos[1] * scaleF;
		pos[2] = (pos[2] - minZ) * scaleF;
		positions[i] = pos;
	}
}

void HologramRender::writefloatimage(const char* fileName, int nx, int ny, int ch, float* intensity, int up)
{
	const int n = nx * ny * ch;

	double min_val, max_val;
	min_val = intensity[0];
	max_val = intensity[0];

	for (int i = 0; i < n; ++i)
	{
		if (min_val > intensity[i])
			min_val = intensity[i];
		else if (max_val < intensity[i])
			max_val = intensity[i];
	}

	char fname[100];
	strcpy(fname, fileName);
	//strcat(fname, ".bmp");

	//LOG("minval %f, max val %f\n", min_val, max_val);
	unsigned char* cgh = (unsigned char*)malloc(sizeof(unsigned char)*n);

	for (int i = 0; i < n; ++i) {
		double val = 255.0 * ((intensity[i] - min_val) / (max_val - min_val));
		cgh[i] = val;
	}

	QImage img(cgh, nx, ny, nx*ch, QImage::Format::Format_RGB888);
	if (up)
		img = img.mirrored(false, true);
	img.save(QString(fname), "jpg");

	free(cgh);
}

void HologramRender::read_depth(QString fname) {

	if (src_depth_)
		free(src_depth_);

	src_depth_ = (float*)malloc(pny_*pnx_ * sizeof(float));
	memset(src_depth_, 0.0, pny_*pnx_ * sizeof(float));

	scene_->ReadDepth(src_depth_);

	scene_->getLinearDepth(pnx_, pny_, src_depth_);

	//if (save_src_file_)
	//{
		float zNear = camera_->get_near();
		float zFar = camera_->get_far();

		unsigned char* depth = (unsigned char*)malloc(pny_*pnx_ * sizeof(unsigned char));
		memset(depth, 0, pny_*pnx_ * sizeof(unsigned char));

		for (int y = 0; y < pny_; ++y) {
			for (int x = 0; x < pnx_; ++x) {
				float val = src_depth_[x + y * pnx_];
				float nval = (val - zNear) / (zFar - zNear);
				depth[x + y * pnx_] = (uchar)(nval * 255.0);
				//depth[x + y * pnx_] = (uchar)(src_depth_[x + y * pnx_] * 255.0);
			}
		}

		QImage image(depth, pnx_, pny_, pnx_, QImage::Format::Format_Grayscale8);
		image = image.mirrored(false, true);
		image.save(fname, "jpg");

		free(depth);
	//}

}

void HologramRender::read_color(QString fname) {

	unsigned char* buffer = (unsigned char*)malloc(pnx_*pny_ * 4 * sizeof(unsigned char));
	scene_->ReadData(buffer);

	QImage image(buffer, pnx_, pny_, pnx_ * 4, QImage::Format::Format_RGBA8888);
	image = image.convertToFormat(QImage::Format_Grayscale8);

	if (save_src_file_) {
		QImage img = image.mirrored(false, true);
		int ret = img.save(fname, "jpg");
	}

	if (src_color_)	free(src_color_);
	src_color_ = (uchar*)malloc(pny_*pnx_ * sizeof(uchar));
	memset(src_color_, 0, pny_*pnx_);
	memcpy(src_color_, image.bits(), pny_*pnx_ * sizeof(uchar));

	free(buffer);
}

void HologramRender::setFrontLight()
{
	int vport[4];
	double model[16];
	double proj[16];
	glGetIntegerv(GL_VIEWPORT, vport);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	double p[3];
	gluUnProject(vport[2] / 2, vport[3] / 2, 0, model, proj, vport, &p[0], &p[1], &p[2]);

	float position[4];
	position[0] = p[0];
	position[1] = p[1];
	position[2] = p[2];
	position[3] = 0.0;

	float diffuse_color[4] = { 0.8, 0.8, 0.8, 1.0 };
	float ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	float specular_color[4] = { 0.3, 0.3, 0.3, 1.0 };
	float global_ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
	float tmp[3] = { 0.0, 0.0, 0.0 };

	kOglRenderShader->SetNumberofLights(1);
	kOglRenderShader->setLightsProperties(0, false, false, false,
		ambient, diffuse_color, specular_color, position, tmp, 0, 0, 1, 0, 0, global_ambient);

}

void HologramRender::setCamera(const graphics::Camera& cam)
{
	double angle = cam.get_angle()/2.0;
	double focal_len = (cam.GetHeight() / 2.0) / tan(double(radian(angle)));
	angle = graphics::degree(atan((pny_ / 2.0) / focal_len) * 2.0);
	
	if (!camera_)	camera_ = new Camera(pnx_, pny_);
	camera_->set_angle(angle);
	camera_->set_scale(cam.scale());
	camera_->SetCameraPose(cam.getCameraPose());
	camera_->set_center_of_look(cam.center_of_look());

	camera_->set_near(cam.get_near());
	camera_->set_far(1000);

}

void HologramRender::beginDraw()
{	
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	real deg = camera_->get_angle();
	float zNear = camera_->get_near();
	float zFar = camera_->get_far();
	
	//LOG("Angle=%.3f, Near=%.3f, Far=%.3f\n", deg, zNear, zFar);
		
	glViewport(0, 0, pnx_, pny_);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	gluPerspective(deg, (float)pnx_ / (float)pny_, zNear, zFar);

	frame f = camera_->getCameraPose();
	vec3 org = f.get_origin();
	vec3 ref = org + f.basis[2];
	vec3 up = f.basis[1];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(org[0], org[1], org[2], ref[0], ref[1], ref[2], up[0], up[1], up[2]);
	
	glClearColor(0, 0, 0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_MULTISAMPLE);
	
}

void HologramRender::endDraw()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glPopAttrib();
	glFinish();

}

void HologramRender::reconstruct_fromCGH()
{
	if (!hologram_)	return;

	if (!hologram_->isCPUMode())
		hologram_->getCGHfromGPU();
	
	graphics::ivec2 Nxy = hologram_->getPixelNumber();
	Complex* out = new Complex[Nxy[0] * Nxy[1]];
	memset(out, 0, sizeof(Complex) * Nxy[0] * Nxy[1]);

	double* src = new double[Nxy[0] * Nxy[1]];
	memset(src, 0.0, sizeof(double)*Nxy[0] * Nxy[1]);

	uchar* dst = new uchar[Nxy[0] * Nxy[1]];
	memset(dst, 0.0, sizeof(uchar)*Nxy[0] * Nxy[1]);

	hologram_->fresnelPropagation(hologram_->getComplexField(), out, hologram_->getOffsetDepth());
	   
	hologram_->encoding_amplitude(out, src, Nxy[0], Nxy[1]);

	hologram_->normalize_cpu();

	hologram_->save(reconfname_.toStdString().c_str(), 8, 0, Nxy[0], Nxy[1], 1);

	delete[] out;
	delete[] src, dst;

}



