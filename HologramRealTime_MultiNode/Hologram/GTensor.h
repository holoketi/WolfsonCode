#pragma once

#include "cudnn/cudnn.h"
#include "cudnn/error_util.h"
#include <vector>


class GTensor {
public:
	GTensor();
	GTensor(int n, int c, int h, int w);
	~GTensor();
	size_t mem_size() const { return N * C*H*W * sizeof(float); }
	size_t float_cnt() const { return N * C * H * W;  }
	void set_mem(float* mem) { mem_ = mem;  }
	float* mem() { return mem_;  }

	// this is C
	void prepare_reduction(int gpu, GTensor* A);
	// C to A reduction
	void reduce(int gpu, GTensor* A, float a, float c = 0); 

	// this is C
	void prepare_min_reduction(int gpu, GTensor* A);
	// C to A reduction
	void min_reduce(int gpu, GTensor* A, float a, float c = 0);

	// this is C
	void prepare_max_reduction(int gpu, GTensor* A);
	// C to A reduction
	void max_reduce(int gpu, GTensor* A, float a, float c = 0);

	// scale this by val: this * val
	void scale(int gpu, float val);

	// aA * bB + cThis = This
	void mul(int gpu, GTensor* A, float a, GTensor* B, float b, float c = 0);
	// aA + bB + cThis = This
	void add(int gpu, GTensor* A, float a, GTensor* B, float b, float c = 0);

	// set the tensor to val
	void set(int gpu, float val);

	void set_tensor(int n, int c, int h, int w);
	cudnnTensorDescriptor_t tensor_desc() { return tensor_desc_;  }

private:

	std::vector<size_t> reduce_workspace_size_;
	cudnnTensorDescriptor_t tensor_desc_;
	cudnnReduceTensorDescriptor_t reduce_tensor_desc_;
	cudnnReduceTensorDescriptor_t max_tensor_desc_;
	cudnnReduceTensorDescriptor_t min_tensor_desc_;
	cudnnOpTensorDescriptor_t mul_tensor_desc_;
	cudnnOpTensorDescriptor_t add_tensor_desc_;
	int N, C, H, W;
	float* mem_;

};
