#ifndef __graphics_data_read_h
#define __graphics_data_read_h


namespace graphics {

# define COLOR_MAX 1000
# define COR3_MAX 200000
# define FACE_MAX 200000
# define LINE_MAX_LEN 256
# define LEVEL_MAX 10
# define LINES_MAX 100000
# define MATERIAL_MAX 100
# define ORDER_MAX 10
# define TEXTURE_MAX 100

extern float*	kVertex[3];
extern int		kVertexMaterial[COR3_MAX];
extern float	kVertexNormal[3][COR3_MAX];
extern int		kVertexCount;
extern float	kVertexTexUv[3][COR3_MAX];

extern int*		kFace[ORDER_MAX];
extern float	kFaceArea[FACE_MAX];
extern int		kFaceFlags[FACE_MAX];
extern int		kFaceMaterial[FACE_MAX];
extern float	kFaceNormal[3][FACE_MAX];
extern int		kFaceCount;
extern int		kFaceObject[FACE_MAX];
extern int		kFaceOrder[FACE_MAX];

void	init_program_data ( );
void	data_init ( );
int		stla_read ( FILE *filein );
int		stlb_read ( FILE *filein );
int		obj_read ( FILE *filein );
bool	data_post_process ( );
int     get_face_count();
int	    get_vertex_count();

};

#endif