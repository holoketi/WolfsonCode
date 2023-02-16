
#include	"Hologram/HologramGenerator.h"
#include    "graphics/sys.h"
#include	<omp.h>

fftw_plan fft_plan_fwd_;
fftw_plan fft_plan_bwd_;

void HologramGenerator::genCghPointCloudCPU()
{
	// Output Image Size
	graphics::ivec2 pn;
	pn[0] = config_PC_->pixel_number[0];
	pn[1] = config_PC_->pixel_number[1];

	// Wave Number (2 * PI / lambda(wavelength))
	double k = (2 * M_PI) / config_PC_->wave_length;

	// Pixel pitch at eyepiece lens plane (by simple magnification) ==> SLM pitch
	graphics::vec2 pp;
	pp[0] = config_PC_->pixel_pitch[0];
	pp[1] = config_PC_->pixel_pitch[1];

	// Length (Width) of complex field at eyepiece plane (by simple magnification)
	graphics::vec2 ss;
	ss[0] = pn[0] * pp[0];
	ss[1] = pn[1] * pp[1];
	const int points = data_PC_->n_points;

	int j; // private variable for Multi Threading
#ifdef _OPENMP
	int num_threads = 0;
#pragma omp parallel
	{
		num_threads = omp_get_num_threads(); // get number of Multi Threading
#pragma omp for private(j)
#endif
		for (j = 0; j < points; ++j) { //Create Fringe Pattern
			uint idx = 3 * j;
			uint color_idx = data_PC_->n_channels * j;
			double pcx = data_PC_->ObjPosition[idx + 0];
			double pcy = data_PC_->ObjPosition[idx + 1];
			double pcz = data_PC_->ObjPosition[idx + 2];
			double amplitude = data_PC_->ObjIntensity[color_idx];

			diffractNotEncodedRS(pn, pp, ss, vec3(pcx, pcy, pcz), k, amplitude, config_PC_->wave_length);
			//break;
		}
#ifdef _OPENMP
	}
	std::cout << ">>> All " << num_threads << " threads" << std::endl;
#endif
}

void HologramGenerator::diffractNotEncodedRS(ivec2 pn, vec2 pp, vec2 ss, vec3 pc, double k, double amplitude, double lambda)
{
	// for performance
	double tx = lambda / (2 * pp[0]);
	double ty = lambda / (2 * pp[1]);
	double half_ss_X = (pp[0] * pn[0]) / 2.0;
	double half_ss_Y = (pp[1] * pn[1]) / 2.0;

	double abs_txy_pcz = abs(tx * pc[2]);
	double _xbound[2] = {
		pc[0] + abs_txy_pcz,
		pc[0] - abs_txy_pcz
	};

	abs_txy_pcz = abs(ty * pc[2]);
	double _ybound[2] = {
		pc[1] + abs_txy_pcz,
		pc[1] - abs_txy_pcz
	};

	double Xbound[2] = {
		floor((_xbound[0] + half_ss_X) / pp[0]) + 1,
		floor((_xbound[1] + half_ss_X) / pp[0]) + 1
	};

	double Ybound[2] = {
		floor((_ybound[0] + half_ss_Y) / pp[1]),
		floor((_ybound[1] + half_ss_Y) / pp[1])
	};

	if (Xbound[0] > pn[0])	Xbound[0] = pn[0];
	if (Xbound[1] < 0)		Xbound[1] = 0;
	if (Ybound[0] > pn[1]) Ybound[0] = pn[1];
	if (Ybound[1] < 0)		Ybound[1] = 0;


	for (int yytr = Ybound[1]; yytr < Ybound[0]; ++yytr)
	{
		for (int xxtr = Xbound[1]; xxtr < Xbound[0]; ++xxtr)
		{
			double x = xxtr * pp[0] - 0.5*pn[0]*pp[0];
			double y = yytr * pp[1] - 0.5*pn[1]*pp[1];
			double z = 0.0;

			double oz = (int)(pc[2] / lambda) * (lambda);
			double rx = x - pc[0];
			double ry = y - pc[1];
			double rz = z - oz;

			const float l = sqrtf(rx*rx + ry * ry + rz * rz);
			double val = amplitude / (lambda * rz);
			double sval = (2 * M_PI) / lambda * l;
			double cos_v = cos(sval);
			double sin_v = sin(sval);

			int index = yytr * pn[0] + xxtr;

#ifdef _OPENMP
#pragma omp atomic
			complex_H_[index].a += val * cos_v;
#pragma omp atomic
			complex_H_[index].b += -val * sin_v;

#endif
		}
	}
}

void HologramGenerator::fresnelPropagation(Complex* in, Complex* out, double distance, uint width, uint height) {

	int Nx = config_PC_->pixel_number[0];
	int Ny = config_PC_->pixel_number[1];
	if (width)		Nx = width;
	if (height)		Ny = height;

	Complex* in2x = (Complex*)malloc(sizeof(Complex)* Nx * Ny * 4);
	memset(in2x, 0.0, sizeof(Complex)*Nx*Ny * 4);

	uint idxIn = 0;

	for (int idxNy = Ny / 2; idxNy < Ny + (Ny / 2); idxNy++) {
		for (int idxNx = Nx / 2; idxNx < Nx + (Nx / 2); idxNx++) {

			in2x[idxNy*Nx * 2 + idxNx] = in[idxIn];
			idxIn++;
		}
	}

	//writeIntensity_gray8_bmp("in2x", Nx*2, Ny*2, in2x);

	Complex* temp1 = (Complex*)malloc(sizeof(Complex)* Nx * Ny * 4);

	fftw_complex *fftwin, *fftwout;
	fft_plan_fwd_ = fftw_plan_dft_2d(Ny * 2, Nx * 2, fftwin, fftwout, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwShift(in2x, temp1, Nx * 2, Ny * 2, FFTW_FORWARD, false);
	fftw_destroy_plan(fft_plan_fwd_);
	fftw_cleanup();
	fft_plan_fwd_ = 0;

	//writeIntensity_gray8_bmp("temp1", Nx * 2, Ny * 2, temp1);

	double* fx = new double[Nx*Ny * 4];
	double* fy = new double[Nx*Ny * 4];

	uint i = 0;
	for (int idxFy = -Ny; idxFy < Ny; idxFy++) {
		for (int idxFx = -Nx; idxFx < Nx; idxFx++) {
			fx[i] = idxFx / (2 * Nx*config_PC_->pixel_pitch[0]);
			fy[i] = idxFy / (2 * Ny*config_PC_->pixel_pitch[1]);
			i++;
		}
	}

	Complex* prop = (Complex*)malloc(sizeof(Complex)* Nx * Ny * 4);
	memset(prop, 0.0, sizeof(Complex)*Nx*Ny * 4);

	double sqrtPart;

	Complex* temp2 = (Complex*)malloc(sizeof(Complex)* Nx * Ny * 4);

	for (int i = 0; i < Nx*Ny * 4; i++) {
		sqrtPart = sqrt(1 / (config_PC_->wave_length * config_PC_->wave_length) - fx[i] * fx[i] - fy[i] * fy[i]);
		prop[i].b = 2 * M_PI * distance;
		prop[i].b *= sqrtPart;
		exponent_complex(&prop[i]);
		temp2[i] = temp1[i] * prop[i];
	}

	//writeIntensity_gray8_bmp("temp2", Nx * 2, Ny * 2, temp2);

	Complex* temp3 = (Complex*)malloc(sizeof(Complex)* Nx * Ny * 4);

	fft_plan_bwd_ = fftw_plan_dft_2d(Ny * 2, Nx * 2, fftwin, fftwout, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwShift(temp2, temp3, Nx * 2, Ny * 2, FFTW_BACKWARD, false);
	fftw_destroy_plan(fft_plan_bwd_);
	fftw_cleanup();
	fft_plan_bwd_ = 0;

	//writeIntensity_gray8_bmp("temp3", Nx * 2, Ny * 2, temp3);

	uint idxOut = 0;
	for (int idxNy = Ny / 2; idxNy < Ny + (Ny / 2); idxNy++) {
		for (int idxNx = Nx / 2; idxNx < Nx + (Nx / 2); idxNx++) {

			out[idxOut] = temp3[idxNy*Nx * 2 + idxNx];
			idxOut++;
		}
	}

	free(in2x);
	free(temp1);
	free(prop);
	free(temp2);
	free(temp3);
	delete[] fx;
	delete[] fy;


}

void HologramGenerator::exponent_complex(Complex* val)
{
	double realv = val->a;
	double imgv = val->b;
	val->a = exp(realv)*cos(imgv);
	val->b = exp(realv)*sin(imgv);

}

void HologramGenerator::fftShift(int nx, int ny, Complex* input, Complex* output)
{
	for (int i = 0; i < nx; i++)
	{
		for (int j = 0; j < ny; j++)
		{
			int ti = i - nx / 2; if (ti < 0) ti += nx;
			int tj = j - ny / 2; if (tj < 0) tj += ny;

			output[ti + tj * nx] = input[i + j * nx];
		}
	}
}


void HologramGenerator::fftwShift(Complex* src, Complex* dst, int nx, int ny, int type, bool bNomarlized)
{
	Complex* tmp = (Complex*)malloc(sizeof(Complex)*nx*ny);
	memset(tmp, 0.0, sizeof(Complex)*nx*ny);
	fftShift(nx, ny, src, tmp);

	fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * nx * ny);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * nx * ny);

	for (int i = 0; i < nx*ny; i++)
	{
		in[i][0] = tmp[i].a;
		in[i][1] = tmp[i].b;
	}

	if (type == FFTW_FORWARD)
		fftw_execute_dft(fft_plan_fwd_, in, out);
	else
		fftw_execute_dft(fft_plan_bwd_, in, out);

	int normalF = 1;
	if (bNomarlized) normalF = nx * ny;
	memset(tmp, 0.0, sizeof(Complex)*nx*ny);

	for (int k = 0; k < nx*ny; k++) {
		tmp[k].a = out[k][0] / normalF;
		tmp[k].b = out[k][1] / normalF;
	}


	memset(dst, 0.0, sizeof(Complex)*nx*ny);
	fftShift(nx, ny, tmp, dst);

	fftw_free(in);
	fftw_free(out);
	free(tmp);

}

void HologramGenerator::encodeSideBand_CPU(int cropx1, int cropx2, int cropy1, int cropy2, ivec2 sig_location)
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];

	Complex* h_temp = (Complex*)malloc(sizeof(Complex) * pnx*pny);
	memset(h_temp, 0.0, sizeof(Complex)*pnx*pny);

	fftw_complex *in, *out;
	fft_plan_fwd_ = fftw_plan_dft_2d(pny, pnx, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwShift(complex_H_, h_temp, pnx, pny, FFTW_FORWARD, false);
	fftw_destroy_plan(fft_plan_fwd_);
	fftw_cleanup();
	fft_plan_fwd_ = 0;

	//writeIntensity_gray8_bmp("afterFWD", pnx, pny, h_temp);

	Complex* h_crop = (Complex*)malloc(sizeof(Complex) * pnx*pny);
	memset(h_crop, 0.0, sizeof(Complex)*pnx*pny);

	int p = 0;
#pragma omp parallel for private(p)	
	for (p = 0; p < pnx*pny; p++)
	{
		int x = p % pnx;
		int y = p / pnx;
		if (x >= cropx1 && x <= cropx2 && y >= cropy1 && y <= cropy2)
			h_crop[p] = h_temp[p];
	}

	//writeIntensity_gray8_bmp("afterCrop", pnx, pny, h_crop);

	fft_plan_bwd_ = fftw_plan_dft_2d(pny, pnx, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwShift(h_crop, h_crop, pnx, pny, FFTW_BACKWARD, false);
	fftw_destroy_plan(fft_plan_bwd_);
	fftw_cleanup();
	fft_plan_bwd_ = 0;

	//writeIntensity_gray8_bmp("afterBWD", pnx, pny, h_crop);	

	int i = 0;
#pragma omp parallel for private(i)	
	for (i = 0; i < pnx*pny; i++) {

		Complex shift_phase(1, 0);
		get_shift_phase_value(shift_phase, i, sig_location);
		holo_encoded_[i] = (h_crop[i] * shift_phase).a;

	}

	//writeIntensity_gray8_bmp("afterPhase", pnx, pny, holo_encoded_);

	free(h_crop);
	free(h_temp);
}

void HologramGenerator::get_shift_phase_value(Complex& shift_phase_val, int idx, ivec2 sig_location)
{
	ivec2 pn = config_PC_->pixel_number;
	int pnx = pn[0];
	int pny = pn[1];
	vec2 pp = config_PC_->pixel_pitch;
	double ppx = pp[0];
	double ppy = pp[1];
	double ssx = pnx * ppx;
	double ssy = pny * ppy;

	if (sig_location[1] != 0)
	{
		int r = idx / pnx;
		int c = idx % pnx;
		double yy = (ssy / 2.0) - (ppy)*r - ppy;

		Complex val;
		if (sig_location[1] == 1)
			val.b = 2 * PI * (yy / (4 * ppy));
		else
			val.b = 2 * PI * (-yy / (4 * ppy));

		exponent_complex(&val);
		shift_phase_val *= val;
	}

	if (sig_location[0] != 0)
	{
		int r = idx / pnx;
		int c = idx % pnx;
		double xx = (-ssx / 2.0) - (ppx)*c - ppx;

		Complex val;
		if (sig_location[0] == -1)
			val.b = 2 * PI * (-xx / (4 * ppx));
		else
			val.b = 2 * PI * (xx / (4 * ppx));

		exponent_complex(&val);
		shift_phase_val *= val;
	}

}
