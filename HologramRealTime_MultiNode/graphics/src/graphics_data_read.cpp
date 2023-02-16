
// this is gpl licensed!

# include <cstdlib>
# include <cmath>
# include <cstdio>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <cstring>

#include "graphics/graphics_data_read.h"

namespace graphics {


using namespace std;

# define ERROR 1

# define PI 3.141592653589793238462643

# define DEG_TO_RAD   ( PI / 180.0 )
# define RAD_TO_DEG   ( 180.0 / PI )



# define COLOR_MAX 1000
# define COR3_MAX 200000
# define FACE_MAX 200000
# define LINE_MAX_LEN 256
# define LEVEL_MAX 10
# define LINES_MAX 100000
# define MATERIAL_MAX 100
# define ORDER_MAX 10
# define TEXTURE_MAX 100

char anim_name[LINE_MAX_LEN];
float background_rgb[3];
int bad_num;
bool byte_swap;
int bytes_num;
int color_num;
int comment_num;


float* kVertex[3];
int kVertexMaterial[COR3_MAX];
float kVertexNormal[3][COR3_MAX];
int kVertexCount;
float kVertexTexUv[3][COR3_MAX];

float cor3[3][COR3_MAX];

bool debug;

int dup_num;

int face[ORDER_MAX][FACE_MAX];
int*   kFace[ORDER_MAX];
float  kFaceArea[FACE_MAX];
int    kFaceFlags[FACE_MAX];
int    kFaceMaterial[FACE_MAX];
float  kFaceNormal[3][FACE_MAX];
int    kFaceCount;
int    kFaceObject[FACE_MAX];
int    kFaceOrder[FACE_MAX];
int    face_smooth[FACE_MAX];
float  kFaceTexUv[2][FACE_MAX];

char   filein_name[81];
char   fileout_name[81];

int    group_num;

int    i;
char   input[LINE_MAX_LEN];
int    k;
char   level_name[LEVEL_MAX][LINE_MAX_LEN];

int    line_dex[LINES_MAX];
int    line_material[LINES_MAX];
int    line_num;
int    line_prune;

int    list[COR3_MAX];

char   material_binding[80];
char   material_name[MATERIAL_MAX][LINE_MAX_LEN];
int    material_num;
float  material_rgba[4][MATERIAL_MAX];

char   mat_name[81];
int    max_order2;

char   normal_binding[80];
float  normal_temp[3][ORDER_MAX*FACE_MAX];

char   object_name[81];
int    object_num;

float  origin[3];
float  pivot[3];
float  rgbcolor[3][COLOR_MAX];
char   temp_name[81];

int    text_num;

char texture_binding[80];
char texture_name[TEXTURE_MAX][LINE_MAX_LEN];
int texture_num;
float texture_temp[2][ORDER_MAX*FACE_MAX];

float transform_matrix[4][4];

int kFaceVertexMaterial[ORDER_MAX][FACE_MAX];
float  kFaceVertexNormal[3][ORDER_MAX][FACE_MAX];
float  kFaceVertexRgb[3][ORDER_MAX][FACE_MAX];
float  kFaceVertexTexUv[2][ORDER_MAX][FACE_MAX];


int     get_face_count()
{
	return kFaceCount;
}

int	    get_vertex_count()
{
	return kVertexCount;
}


//****************************************************************************80
//
//  FUNCTION PROTOTYPES
//
//****************************************************************************80

char ch_cap ( char c );
bool ch_eqi ( char c1, char c2 );
int ch_index_last ( char* string, char c );
bool ch_is_space ( char c );
int ch_pad ( int *char_index, int *null_index, char *s, int max_string );
char ch_read ( FILE *filein );
int ch_to_digit ( char c );
int ch_write ( FILE *fileout, char c );
void kVertexNormal_set ( );
void cor3_range ( );
void data_check ( );
void data_report ( );
int data_write ( );
int dxf_read ( FILE *filein );
int dxf_write ( FILE *fileout );
int edge_count ( );
void edge_null_delete ( );
void kFaceArea_set ( );
void kFaceNormal_ave ( );
void face_null_delete ( );
int face_print ( int iface );
void face_reverse_order ( );
int face_subset ( );
void face_to_line ( );
void face_to_vertex_material ( );
char *file_ext ( char *file_name );
float float_read ( FILE *filein );
float float_reverse_bytes ( float x );
int float_write ( FILE *fileout, float float_val );
void hello ( );
void help ( );
int hrc_read ( FILE *filein );
int hrc_write ( FILE *fileout );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
int i4_modp ( int i, int j );
int i4_wrap ( int ival, int ilo, int ihi );
void init_program_data ( );
int interact ( );
int iv_read ( FILE *filein );
int iv_write ( FILE *fileout );
int i4vec_max ( int n, int *a );
long int long_int_read ( FILE *filein );
int long_int_write ( FILE *fileout, long int int_val );
void node_to_vertex_material ( );

int obj_read ( FILE *filein );
int obj_write ( FILE *fileout );
int rcol_find ( float a[][COR3_MAX], int m, int n, float r[] );
float rgb_to_hue ( float r, float g, float b );
bool s_eqi ( char* string1, char* string2 );
int s_len_trim ( char *s );
int s_to_i4 ( char *s, int *last, bool *error );
bool s_to_i4vec ( char *s, int n, int ivec[] );
float s_to_r4 ( char *s, int *lchar, bool *error );
bool s_to_r4vec ( char *s, int n, float rvec[] );
short int short_int_read ( FILE *filein );
int short_int_write ( FILE *fileout, short int int_val );
void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn );
int stla_read ( FILE *filein );
int stla_write ( FILE *fileout );
int stlb_write ( FILE *fileout );

void tmat_init ( float a[4][4] );
void tmat_mxm ( float a[4][4], float b[4][4], float c[4][4] );
void tmat_mxp ( float a[4][4], float x[4], float y[4] );
void tmat_mxp2 ( float a[4][4], float x[][3], float y[][3], int n );
void tmat_mxv ( float a[4][4], float x[4], float y[4] );
void tmat_rot_axis ( float a[4][4], float b[4][4], float angle, char axis );
void tmat_rot_vector ( float a[4][4], float b[4][4], float angle,
  float v1, float v2, float v3 );
void tmat_scale ( float a[4][4], float b[4][4], float sx, float sy, float sz );
void tmat_shear ( float a[4][4], float b[4][4], char *axis, float s );
void tmat_trans ( float a[4][4], float b[4][4], float x, float y, float z );

void vertex_normal_set ( );
void vertex_to_kFaceMaterial ( );
void vertex_to_node_material ( );


void set_file_name(wchar_t* file_name)
{

}

//****************************************************************************80

char ch_cap ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_CAP capitalizes a single character.
//
//  Discussion:
//
//    This routine should be equivalent to the library "toupper" function.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 July 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the character to capitalize.
//
//    Output, char CH_CAP, the capitalized character.
//
{
  if ( 97 <= c && c <= 122 )
  {
    c = c - 32;
  }

  return c;
}
//****************************************************************************80

bool ch_eqi ( char c1, char c2 )

//****************************************************************************80
//
//  Purpose:
//
//    CH_EQI is true if two characters are equal, disregarding case.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C1, char C2, the characters to compare.
//
//    Output, bool CH_EQI, is true if the two characters are equal,
//    disregarding case.
//
{
  if ( 97 <= c1 && c1 <= 122 )
  {
    c1 = c1 - 32;
  }
  if ( 97 <= c2 && c2 <= 122 )
  {
    c2 = c2 - 32;
  }

  return ( c1 == c2 );
}
//****************************************************************************80

int ch_index_last ( char *s, char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_INDEX_LAST finds the last occurrence of a character in a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a pointer to a string to be searched.
//
//    Input, char C, the character to be searched for in s.
//
//    Output, int CH_INDEX_LAST, the index in s of the last occurrence
//    of C, or -1 if c does not occur in s.
//
{
  int i;
  int j;
  int nchar;

  j = -1;

  nchar = strlen ( s );

  for ( i = 0; i < nchar;++i)
  {
    if ( s[i] == c )
    {
      j = i;
    }
  }

  return j;

}
//****************************************************************************80

bool ch_is_space ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_IS_SPACE is TRUE if a character represents "white space".
//
//  Discussion:
//
//    A white space character is a space, a form feed, a newline, a carriage
//    return, a horizontal tab, or a vertical tab.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the character to be analyzed.
//
//    Output, bool CH_IS_SPACE, is TRUE if C is a whitespace character.
//
{
  if ( c == ' ' )
  {
    return true;
  }
  else if ( c == '\f' )
  {
    return true;
  }
  else if ( c == '\n' )
  {
    return true;
  }
  else if ( c == '\r' )
  {
    return true;
  }
  else if ( c == '\t' )
  {
    return true;
  }
  else if ( c == '\v' )
  {
    return true;
  }
  else
  {
    return false;
  }
}
//****************************************************************************80

int ch_pad ( int *char_index, int *null_index, char *s, int max_string )

//****************************************************************************80
//
//  Purpose:
//
//    CH_PAD "pads" a character in a string with a blank on either side.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *CHAR_INDEX, the position of the character to be
//    padded.  On output, this is increased by 1.
//
//    Input/output, int *NULL_INDEX, the position of the terminating NULL in
//    the string.  On output, this is increased by 2.
//
//    Input/output, char S[MAX_STRING], the string to be manipulated.
//
//    Input, int MAX_STRING, the maximum number of characters that can be stored
//    in s.
//
//    Output, int CH_PAD, is 0 if the operation worked, and 1 otherwise.
//
{
  int i;

  if ( *char_index < 0 ||
      *null_index <= *char_index ||
      max_string - 1 < *char_index )
  {
    return 1;
  }

  if ( max_string - 1 < (*null_index) + 2 )
  {
    return 1;
  }

  for ( i = *null_index + 2; *char_index + 2 < i; i-- )
  {
    s[i] = s[i-2];
  }
  s[*char_index+2] = ' ';
  s[*char_index+1] = s[*char_index];
  s[*char_index] = ' ';

  *char_index = *char_index + 1;
  *null_index = *null_index + 2;

  return 0;
}
//****************************************************************************80

char ch_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    CH_READ reads one character from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  char c;

  c = ( char ) fgetc ( filein );

  return c;
}
//****************************************************************************80

int ch_to_digit ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_TO_DIGIT returns the integer value of a base 10 digit.
//
//  Example:
//
//     C   DIGIT
//    ---  -----
//    '0'    0
//    '1'    1
//    ...  ...
//    '9'    9
//    ' '    0
//    'X'   -1
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the decimal digit, '0' through '9' or blank are legal.
//
//    Output, int CH_TO_DIGIT, the corresponding integer value.  If C was
//    'illegal', then DIGIT is -1.
//
{
  int digit;

  if ( '0' <= c && c <= '9' )
  {
    digit = c - '0';
  }
  else if ( c == ' ' )
  {
    digit = 0;
  }
  else
  {
    digit = -1;
  }

  return digit;
}
//****************************************************************************80

int ch_write ( FILE *fileout, char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_WRITE writes one character to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  fputc ( c, fileout );

  return 1;
}

//****************************************************************************80

void kVertexNormal_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    COR3_NORMAL_SET computes node normal vectors.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    18 November 1998
//
//  Author:
//
//    John Burkardt
//
{
  int   icor3;
  int   iface;
  int   ivert;
  int   j;
  float norm;
  float temp;

  for ( icor3 = 0; icor3 < kVertexCount; icor3++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      kVertexNormal[j][icor3] = 0.0;
    }
  }
//
//  Add up the normals at all the faces to which the node belongs.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      icor3 = face[ivert][iface];
      for ( j = 0; j < 3; j++ )
      {
        kVertexNormal[j][icor3] = kVertexNormal[j][icor3]
          + kFaceVertexNormal[j][ivert][iface];
      }
    }
  }
//
//  Renormalize.
//
  for ( icor3 = 0; icor3 < kVertexCount; icor3++ )
  {
    norm = 0.0;
    for ( j = 0; j < 3; j++ )
    {
      temp = kVertexNormal[j][icor3];
      norm = norm + temp * temp;
    }

    if ( norm == 0.0 )
    {
      norm = 3.0;
      for ( j = 0; j < 3; j++ )
      {
        kVertexNormal[j][icor3] = 1.0;
      }
    }

    norm = ( float ) sqrt ( norm );

    for ( j = 0; j < 3; j++ )
    {
      kVertexNormal[j][icor3] = kVertexNormal[j][icor3] / norm;
    }
  }

  return;
}
//****************************************************************************80

void cor3_range ( )

//****************************************************************************80
//
//  Purpose:
//
//    COR3_RANGE computes the coordinate minima and maxima.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    31 August 1998
//
//  Author:
//
//    John Burkardt
//
{
  int   i;
  float xave;
  float xmax;
  float xmin;
  float yave;
  float ymax;
  float ymin;
  float zave;
  float zmax;
  float zmin;

  xave = cor3[0][0];
  xmax = cor3[0][0];
  xmin = cor3[0][0];

  yave = cor3[1][0];
  ymax = cor3[1][0];
  ymin = cor3[1][0];

  zave = cor3[2][0];
  zmax = cor3[2][0];
  zmin = cor3[2][0];

  for ( i = 1; i < kVertexCount;++i )
  {
    xave = xave + cor3[0][i];
    if ( cor3[0][i] < xmin )
    {
      xmin = cor3[0][i];
    }
    if ( xmax < cor3[0][i] )
    {
      xmax = cor3[0][i];
    }

    yave = yave + cor3[1][i];
    if ( cor3[1][i] < ymin )
    {
      ymin = cor3[1][i];
    }
    if ( ymax < cor3[1][i] )
    {
      ymax = cor3[1][i];
    }

    zave = zave + cor3[2][i];
    if ( cor3[2][i] < zmin )
    {
      zmin = cor3[2][i];
    }
    if ( zmax < cor3[2][i] )
    {
      zmax = cor3[2][i];
    }
  }

  xave = xave / kVertexCount;
  yave = yave / kVertexCount;
  zave = zave / kVertexCount;

  cout << "\n";
  cout << "COR3_RANGE - Data range:\n";
  cout << "\n";
  cout << "   Minimum   Average   Maximum  Range\n";
  cout << "\n";
  cout << "X  " << setw(10) << xmin        << "  "
                << setw(10) << xave        << "  "
                << setw(10) << xmax        << "  "
                << setw(10) << xmax - xmin << "\n";
  cout << "Y  " << setw(10) << ymin        << "  "
                << setw(10) << yave        << "  "
                << setw(10) << ymax        << "  "
                << setw(10) << ymax - ymin << "\n";
  cout << "Z  " << setw(10) << zmin        << "  "
                << setw(10) << zave        << "  "
                << setw(10) << zmax        << "  "
                << setw(10) << zmax - zmin << "\n";

  return;
}
//****************************************************************************80

void data_check ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_CHECK checks the input data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 May 2005
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int nfix;

  if ( COLOR_MAX < color_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << color_num << " colors.\n";
    cout << "  There was only room for " << COLOR_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of COLOR_MAX, recompile, and rerun the program.\n";
    color_num = COLOR_MAX;
  }

  if ( COR3_MAX < kVertexCount )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << kVertexCount << " points.\n";
    cout << "  There was only room for " << COR3_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of COR3_MAX, recompile, and rerun the program.\n";
    kVertexCount = COR3_MAX;
  }

  if ( FACE_MAX < kFaceCount )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << kFaceCount << " faces.\n";
    cout << "  There was only room for " << FACE_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of FACE_MAX, recompile, and rerun the program.\n";
    kFaceCount = FACE_MAX;
  }

  if ( LINES_MAX < line_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << line_num << " line items.\n";
    cout << "  There was only room for " << LINES_MAX << ".\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of LINES_MAX, recompile, and rerun the program.\n";
    line_num = LINES_MAX;
  }

  nfix = 0;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    if ( ORDER_MAX < kFaceOrder[iface] )
    {
      kFaceOrder[iface] = ORDER_MAX;
      nfix = nfix + 1;
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  Corrected " << nfix
         << " faces using more than " << ORDER_MAX
         << " vertices per face.\n";
  }

  for ( i = 0; i < material_num;++i )
  {
    if ( strcmp ( material_name[i], "" ) == 0 )
    {
      strcpy ( material_name[i], "Material_0000" );
    }
  }

  for ( i = 0; i < texture_num;++i )
  {
    if ( strcmp ( texture_name[i], "" ) == 0 )
    {
      strcpy ( texture_name[i], "Texture_0000" );
    }
  }

  cout << "\n";
  cout << "DATA_CHECK - Data checked.\n";

  return;
}
//****************************************************************************80

void data_init ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_INIT initializes the internal graphics data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 July 2000
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int ivert;
  int j;
  int k;

  strcpy( anim_name, "" );

  for ( i = 0; i < 3;++i )
  {
    background_rgb[i] = 0.0;
  }

  for ( i = 0; i < 3;++i )
  {
    for ( j = 0; j < COR3_MAX; j++ )
    {
      cor3[i][j] = 0.0;
    }
  }

  for ( i = 0; i < COR3_MAX;++i )
  {
    kVertexMaterial[i] = 0;
  }

  for ( i = 0; i < 3;++i )
  {
    for ( j = 0; j < COR3_MAX; j++ )
    {
      kVertexNormal[i][j] = 0.0;
    }
  }

  for ( j = 0; j < COR3_MAX; j++ )
  {
    kVertexTexUv[0][j] = 0.0;
    kVertexTexUv[1][j] = 0.0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      face[ivert][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    kFaceFlags[iface] = 6;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    kFaceMaterial[iface] = 0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( i = 0; i < 3;++i )
    {
      kFaceNormal[i][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    kFaceObject[iface] = -1;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    kFaceOrder[iface] = 0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_smooth[iface] = 1;
  }

  for ( i = 0; i < LINES_MAX;++i )
  {
    line_dex[i] = -1;
  }

  for ( i = 0; i < LINES_MAX;++i )
  {
    line_material[i] = 0;
  }

  strcpy ( material_binding, "DEFAULT" );

  for ( j = 0; j < MATERIAL_MAX; j++ )
  {
    strcpy ( material_name[j], "Material_0000" );
  }

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < MATERIAL_MAX; j++ )
    {
      material_rgba[i][j] = 0.0;
    }
  }

  strcpy ( normal_binding, "DEFAULT" );

  for ( j = 0; j < ORDER_MAX*FACE_MAX; j++ )
  {
    for ( i = 0; i < 3;++i )
    {
      normal_temp[i][j] = 0;
    }
  }

  color_num = 0;
  kVertexCount = 0;
  kFaceCount = 0;
  group_num = 0;
  line_num = 0;
  material_num = 0;
  object_num = 0;
  texture_num = 0;

  strcpy ( object_name, "IVCON" );

  for ( i = 0; i < 3;++i )
  {
    origin[i] = 0.0;
  }

  for ( i = 0; i < 3;++i )
  {
    pivot[i] = 0.0;
  }

  for ( j = 0; j < COLOR_MAX; j++ )
  {
    rgbcolor[0][j] = 0.299;
    rgbcolor[1][j] = 0.587;
    rgbcolor[2][j] = 0.114;
  }

  strcpy ( texture_binding, "DEFAULT" );

  for ( j = 0; j < TEXTURE_MAX; j++ )
  {
    strcpy ( texture_name[j], "Texture_0000" );
  }

  tmat_init ( transform_matrix );

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      kFaceVertexMaterial[ivert][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      for ( i = 0; i < 3;++i )
      {
        kFaceVertexNormal[i][ivert][iface] = 0.0;
      }
    }
  }

  for ( j = 0; j < 3; j++ )
  {
    for ( k = 0; k < FACE_MAX;++k )
    {
      kFaceVertexRgb[0][j][k] = 0.299;
      kFaceVertexRgb[1][j][k] = 0.587;
      kFaceVertexRgb[2][j][k] = 0.114;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      for ( i = 0; i < 2;++i )
      {
        kFaceVertexTexUv[i][ivert][iface] = 0.0;
      }
    }
  }

  if ( debug )
  {
    cout << "\n";
    cout << "DATA_INIT: Graphics data initialized.\n";
  }

  return;
}
//****************************************************************************80

bool data_post_process ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_READ reads a file into internal graphics data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    08 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, bool DATA_READ, is TRUE if the data was read successfully.
//
{

  int icor3;
  int ierror;
  int iface;
  int iline;
  int ivert;
  int ntemp;

//
//  Initialize some data.
//
  max_order2 = 0;
  bad_num = 0;
  bytes_num = 0;
  comment_num = 0;
  dup_num = 0;
  text_num = 0;

//  Restore the transformation matrix.
//
  tmat_init ( transform_matrix );
//
//  Report on what we read.
//
  if ( kFaceCount < FACE_MAX )
  {
    ntemp = kFaceCount;
  }
  else
  {
    ntemp = FACE_MAX;
  }

  max_order2 = i4vec_max ( ntemp, kFaceOrder );

  data_report ( );
//
//  Warn about any errors that occurred during reading.
//
  if ( ierror == ERROR )
  {
    cout << "\n";
    cout << "DATA_READ - Fatal error!\n";
    cout << "  An error occurred while reading the input file.\n";
    return false;
  }
//
//  Check the data.
//  You MUST wait until after this check before doing other computations,
//  since COR3_NUM and other variables could be much larger than the legal
//  maximums, until corrected by DATA_CHECK.
//
  data_check ( );
//
//  MATERIALS FIXUPS:
//
//  If there are no materials at all, define one.
//
  if ( material_num < 1 )
  {
    material_num = 1;
    strcpy ( material_name[0], "Material_0000" );
    material_rgba[0][0] = 0.7;
    material_rgba[1][0] = 0.7;
    material_rgba[2][0] = 0.7;
    material_rgba[3][0] = 1.0;
  }
//
//  If a node has not been assigned a material, set it to material 0.
//
  for ( icor3 = 0; icor3 < kVertexCount; icor3++ )
  {
    if ( kVertexMaterial[icor3] < 0 || material_num - 1 < kVertexMaterial[icor3] )
    {
      kVertexMaterial[icor3] = 0;
    }
  }
//
//  If a vertex has not been assigned a material, set it to material 0.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      if ( kFaceVertexMaterial[ivert][iface] < 0 ||
           material_num - 1 < kFaceVertexMaterial[ivert][iface] )
      {
        kFaceVertexMaterial[ivert][iface] = 0;
      }
    }
  }
//
//  If a face has not been assigned a material, set it to material 0.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    if ( kFaceMaterial[iface] < 0 || material_num - 1 < kFaceMaterial[iface] )
    {
      kFaceMaterial[iface] = 0;
    }
  }
//
//  If a line item has not been assigned a material, set it to material 0.
//
  for ( iline = 0; iline < line_num; iline++ )
  {
    if ( line_dex[iline] == -1 )
    {
      line_material[iline] = -1;
    }
    else if ( line_material[iline] < 0 ||
             material_num - 1 < line_material[iline] )
    {
      line_material[iline] = 0;
    }
  }
//
//  Delete edges of zero length.
//
  edge_null_delete ( );
//
//  Compute the area of each face.
//
  kFaceArea_set ( );
//
//  Delete faces with zero area.
//
  face_null_delete ( );
//
//  Recompute zero face-vertex normals from vertex positions.
//
  vertex_normal_set ( );
//
//  Compute the node normals from the vertex normals.
//
  kVertexNormal_set ( );
//
//  Recompute zero face normals by averaging face-vertex normals.
//
  kFaceNormal_ave ( );
//
//  Report on the nodal coordinate range.
//
  cor3_range ( );

  return true;
}
//****************************************************************************80

void data_report ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_REPORT gives a summary of the contents of the data file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  cout << "\n";
  cout << "DATA_REPORT - The input file contains:\n";
  cout << "\n";
  cout << "  Bad data items             " << bad_num << "\n";
  cout << "  Text lines                 " << text_num << "\n";
  cout << "  Text bytes (binary data)   " << bytes_num << "\n";
  cout << "  Colors                     " << color_num << "\n";
  cout << "  Comments                   " << comment_num << "\n";
  cout << "  Duplicate points           " << dup_num << "\n";
  cout << "  Faces                      " << kFaceCount << "\n";
  cout << "  Groups                     " << group_num << "\n";
  cout << "  Vertices per face, maximum " << max_order2 << "\n";
  cout << "  Line items                 " << line_num << "\n";
  cout << "  Points                     " << kVertexCount << "\n";
  cout << "  Objects                    " << object_num << "\n";

  return;
}

//****************************************************************************80

int dxf_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    DXF_READ reads an AutoCAD DXF file.
//
//  Example:
//
//      0
//    SECTION
//      2
//    HEADER
//    999
//    diamond.dxf created by IVREAD.
//    999
//    Original data in diamond.obj.
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    TABLES
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    BLOCKS
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    ENTITIES
//      0
//    LINE
//      8
//    0
//     10
//      0.00  (X coordinate of beginning of line.)
//     20
//      0.00  (Y coordinate of beginning of line.)
//     30
//      0.00  (Z coordinate of beginning of line.)
//     11
//      1.32  (X coordinate of end of line.)
//     21
//      1.73  (Y coordinate of end of line.)
//     31
//      2.25  (Z coordinate of end of line.)
//      0
//    3DFACE
//      8
//     Cube
//    10
//    -0.50  (X coordinate of vertex 1)
//    20
//     0.50  (Y coordinate of vertex 1)
//    30
//      1.0  (Z coordinate of vertex 1)
//    11
//     0.50  (X coordinate of vertex 2)
//    21
//     0.50  (Y coordinate of vertex 2)
//    31
//      1.0  (Z coordinate of vertex 2)
//    12
//     0.50  (X coordinate of vertex 3)
//    22
//     0.50  (Y coordinate of vertex 3)
//    32
//     0.00  (Z coordinate of vertex 3)
//      0
//    ENDSEC
//      0
//    EOF
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int   code;
  int   count;
  float cvec[3];
  int   icor3;
  char  input1[LINE_MAX_LEN];
  char  input2[LINE_MAX_LEN];
  int   ivert;
  float rval;
  int   width;
  int   linemode;
  int   cpos;

  linemode = 0;
  ivert = 0;
//
//  Read the next two lines of the file into INPUT1 and INPUT2.
//

  for ( ;; )
  {

//
//  INPUT1 should contain a single integer, which tells what INPUT2
//  will contain.
//
    if ( fgets ( input1, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;

    count = sscanf ( input1, "%d%n", &code, &width );
    if ( count <= 0 )
    {
      break;
    }
//
//  Read the second line, and interpret it according to the code.
//
    if ( fgets ( input2, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;

    if ( code == 0 )
    {

//
//  Finish off the face.
//
      if ( 0 < ivert )
      {
        kFaceOrder[kFaceCount] = ivert;
        kFaceCount = kFaceCount + 1;
        ivert = 0;
      }

      if ( strncmp( input2, "LINE", 4 ) == 0 )
      {
        linemode = 1;
      }
      else if ( strncmp( input2, "3DFACE", 6 ) == 0 )
      {
        linemode = 0;
        ivert = 0;
      }
    }
    else
    {
      for (cpos = 0; input1[cpos] == ' '; cpos++)
      { };

      if ( input1[cpos] == '1' || input1[cpos] == '2' || input1[cpos] == '3' )
      {
        count = sscanf ( input2, "%e%n", &rval, &width );

        switch ( input1[cpos] )
        {
          case '1':
            if ( 0 < line_num )
            {
              if ( linemode )
              {
                line_dex[line_num] = - 1;
                line_material[line_num] = - 1;
                line_num = line_num + 1;
              }
            }
            cvec[0] = rval;
            break;

          case '2':
            cvec[1] = rval;
            break;

          case '3':
            cvec[2] = rval;

            if ( kVertexCount < 1000 )
            {
              icor3 = rcol_find ( cor3, 3, kVertexCount, cvec );
            }
            else
            {
              icor3 = -1;
            }

            if ( icor3 == -1 )
            {
              icor3 = kVertexCount;
              if ( kVertexCount < COR3_MAX )
              {
                cor3[0][kVertexCount] = cvec[0];
                cor3[1][kVertexCount] = cvec[1];
                cor3[2][kVertexCount] = cvec[2];
              }
              kVertexCount = kVertexCount + 1;
            }
            else
            {
              dup_num = dup_num + 1;
            }

            if ( linemode )
            {
              line_dex[line_num] = icor3;
              line_material[line_num] = 0;
              line_num = line_num + 1;
            }
            else
            {
              face[ivert][kFaceCount] = icor3;
              ivert = ivert + 1;
            }
            break;

          default:
            break;
        }
      }
    }
  }

  if ( 0 < line_num )
  {
    if ( linemode )
    {
      line_dex[line_num] = - 1;
      line_material[line_num] = - 1;
      line_num = line_num + 1;
    }
  }
  return 0;
}


int edge_count ( )

//****************************************************************************80
//
//  Purpose:
//
//    EDGE_COUNT determines the number of edges in a graph.
//
//  Discussion:
//
//    The routine extracts the successive pairs of vertices that
//    define each edge of a face.  It reorders each pair so that
//    the lesser element is listed first.  It sorts the entire list.
//    Then it counts the unique entries.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    08 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, int EDGE_COUNT, the number of unique edges.
//
{
  int *edge;
  int edge_num;
  int edge_num_old;
  int i;
  int iface;
  int indx;
  int isgn;
  int j;
  int k;
  int vert;
  int vert2;
//
//  First count the number of edges with duplication.
//
  edge_num = 0;
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    edge_num = edge_num + kFaceOrder[iface];
  }
//
//  Allocate space, and store the edges.
//
  edge = new int[edge_num*2];

  edge_num = 0;
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( vert = 0; vert < kFaceOrder[iface]; vert++ )
    {
      i = face[vert][iface];
      vert2 = i4_wrap ( vert+1, 0, kFaceOrder[iface]-1 );
      j = face[vert2][iface];
      edge[0+edge_num*2] = i4_min ( i, j );
      edge[1+edge_num*2] = i4_max ( i, j );
      edge_num = edge_num + 1;
    }
  }
//
//  Sort the edges.
//
  i = 0;
  indx = 0;
  isgn = 0;
  j = 0;
//
//  Call the external heap sorter.
//
  for ( ; ; )
  {
    sort_heap_external ( edge_num, &indx, &i, &j, isgn );
//
//  Interchange the I and J objects.
//
    if ( 0 < indx )
    {
      k           = edge[0+i*2];
      edge[0+i*2] = edge[0+j*2];
      edge[0+j*2] = k;
      k           = edge[1+i*2];
      edge[1+i*2] = edge[1+j*2];
      edge[1+j*2] = k;
    }
//
//  Compare the I and J objects.
//
    else if ( indx < 0 )
    {
      if ( edge[0+i*2] < edge[0+j*2] )
      {
        isgn = -1;
      }
      else if ( edge[0+i*2] == edge[0+j*2] )
      {
        if ( edge[1+i*2] < edge[1+j*2] )
        {
          isgn = -1;
        }
        else if ( edge[1+i*2] == edge[1+j*2] )
        {
          isgn = 0;
        }
        else
        {
          isgn = 1;
        }
      }
      else
      {
        isgn = 1;
      }
    }
    else if ( indx == 0 )
    {
      break;
    }

  }
//
//  Count the unique entries.
//
  edge_num_old = edge_num;

  edge_num = 0;

  for ( i = 0; i < edge_num_old;++i )
  {
    if ( i == 0 )
    {
      edge_num = 1;
    }
    else
    {
      if ( edge[0+(i-1)*2] != edge[0+i*2] ||
          edge[1+(i-1)*2] != edge[1+i*2] )
      {
        edge_num = edge_num + 1;
      }
    }

  }

  delete [] edge;

  return edge_num;
}
//****************************************************************************80

void edge_null_delete ( )

//****************************************************************************80
//
//  Purpose:
//
//    EDGE_NULL_DELETE deletes face edges with zero length.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  float distsq;
  int face2[ORDER_MAX];
  int kFaceOrder2;
  int iface;
  int inode;
  int ivert;
  int j;
  int jnode;
  int jvert;
  int edge_num;
  int edge_num_del;
  float vertex_normal2[3][ORDER_MAX];
  float x;
  float y;
  float z;

  edge_num = 0;
  edge_num_del = 0;
//
//  Consider each face.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
//
//  Consider each pair of consecutive vertices.
//
    kFaceOrder2 = 0;

    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      edge_num = edge_num + 1;

      jvert = ivert + 1;
      if ( kFaceOrder[iface] <= jvert )
      {
        jvert = 0;
      }

      inode = face[ivert][iface];
      jnode = face[jvert][iface];


      x = cor3[0][inode] - cor3[0][jnode];
      y = cor3[1][inode] - cor3[1][jnode];
      z = cor3[2][inode] - cor3[2][jnode];

      distsq = x * x + y * y + z * z;

      if ( distsq != 0.0 )
      {
        face2[kFaceOrder2] = face[ivert][iface];
        vertex_normal2[0][kFaceOrder2] = kFaceVertexNormal[0][ivert][iface];
        vertex_normal2[1][kFaceOrder2] = kFaceVertexNormal[1][ivert][iface];
        vertex_normal2[2][kFaceOrder2] = kFaceVertexNormal[2][ivert][iface];
        kFaceOrder2 = kFaceOrder2 + 1;
      }
      else
      {
        edge_num_del = edge_num_del + 1;
      }

    }

    kFaceOrder[iface] = kFaceOrder2;
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      face[ivert][iface] = face2[ivert];
      for ( j = 0; j < 3; j++ )
      {
        kFaceVertexNormal[j][ivert][iface] = vertex_normal2[j][ivert];
      }
    }

  }

  cout << "\n";
  cout << "EDGE_NULL_DELETE:\n";
  cout << "  There are a total of " << edge_num << " edges.\n";
  cout << "  Of these, " << edge_num_del << " were of zero length,\n";
  cout << "  and were deleted.\n";

  return;
}
//****************************************************************************80

void kFaceArea_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_AREA_SET computes the area of the faces.
//
//  Discussion:
//
//    The area is the sum of the areas of the triangles formed by
//    node N with consecutive pairs of nodes.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 July 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Adrian Bowyer and John Woodwark,
//    A Programmer's Geometry,
//    Butterworths, 1983.
//
{
  float alpha;
  float area_max;
  float area_min;
  float area_tri;
  float base;
  float dot;
  float height;
  int i;
  int i1;
  int i2;
  int i3;
  int iface;
  int kFaceCount_del;
  float tol;
  float x;
  float x1;
  float x2;
  float x3;
  float y;
  float y1;
  float y2;
  float y3;
  float z;
  float z1;
  float z2;
  float z3;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    kFaceArea[iface] = 0.0;

    for ( i = 0; i < kFaceOrder[iface]-2;++i )
    {
      i1 = face[i][iface];
      i2 = face[i+1][iface];
      i3 = face[i+2][iface];

      x1 = cor3[0][i1];
      y1 = cor3[1][i1];
      z1 = cor3[2][i1];

      x2 = cor3[0][i2];
      y2 = cor3[1][i2];
      z2 = cor3[2][i2];

      x3 = cor3[0][i3];
      y3 = cor3[1][i3];
      z3 = cor3[2][i3];
//
//  Find the projection of (P3-P1) onto (P2-P1).
//
      dot =
        ( x2 - x1 ) * ( x3 - x1 ) +
        ( y2 - y1 ) * ( y3 - y1 ) +
        ( z2 - z1 ) * ( z3 - z1 );

      base = sqrt (
          ( x2 - x1 ) * ( x2 - x1 )
        + ( y2 - y1 ) * ( y2 - y1 )
        + ( z2 - z1 ) * ( z2 - z1 ) );
//
//  The height of the triangle is the length of (P3-P1) after its
//  projection onto (P2-P1) has been subtracted.
//
      if ( base == 0.0 )
      {
        height = 0.0;
      }
      else
      {
        alpha = dot / ( base * base );

        x = x3 - x1 - alpha * ( x2 - x1 );
        y = y3 - y1 - alpha * ( y2 - y1 );
        z = z3 - z1 - alpha * ( z2 - z1 );

        height = sqrt ( x * x + y * y + z * z );
      }

      area_tri = 0.5 * base * height;
      kFaceArea[iface] = kFaceArea[iface] + area_tri;

    }

  }

  area_min = kFaceArea[0];
  area_max = kFaceArea[0];

  for ( iface = 1; iface < kFaceCount; iface++ )
  {
    if ( kFaceArea[iface] < area_min )
    {
      area_min = kFaceArea[iface];
    }
    if ( area_max < kFaceArea[iface] )
    {
      area_max = kFaceArea[iface];
    }
  }

  cout << "\n";
  cout << "FACE_AREA_SET:\n";
  cout << "  Minimum face area is " << area_min << "\n";
  cout << "  Maximum face area is " << area_max << "\n";

  tol = area_max / 10000.0;

  if ( area_min < tol )
  {
    kFaceCount_del = 0;

    for ( iface = 0; iface < kFaceCount; iface++ )
    {
      if ( kFaceArea[iface] < tol )
      {
        kFaceOrder[iface] = 0;
        kFaceCount_del = kFaceCount_del + 1;
      }
    }

    cout << "  Marked " << kFaceCount_del << " tiny faces for deletion.\n";

  }

  return;
}
//****************************************************************************80

void kFaceNormal_ave ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_NORMAL_AVE sets face normals as average of face vertex normals.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int ivert;
  int nfix;
  float norm;
  float x;
  float y;
  float z;

  if ( kFaceCount <= 0 )
  {
    return;
  }

  nfix = 0;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
//
//  Check the norm of the current normal vector.
//
    x = kFaceNormal[0][iface];
    y = kFaceNormal[1][iface];
    z = kFaceNormal[2][iface];
    norm = ( float ) sqrt ( x * x + y * y + z * z );

    if ( norm == 0.0 )
    {
      nfix = nfix + 1;

      for ( i = 0; i < 3;++i )
      {
        kFaceNormal[i][iface] = 0.0;
      }

      for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
      {
        for ( i = 0; i < 3;++i )
        {
          kFaceNormal[i][iface] = kFaceNormal[i][iface] +
            kFaceVertexNormal[i][ivert][iface];
        }
      }

      x = kFaceNormal[0][iface];
      y = kFaceNormal[1][iface];
      z = kFaceNormal[2][iface];
      norm = ( float ) sqrt ( x * x + y * y + z * z );

      if ( norm == 0.0 )
      {
        for ( i = 0; i < 3;++i )
        {
          kFaceNormal[i][iface] = ( float ) ( 1.0 / sqrt ( 3.0 ) );
        }
      }
      else
      {
        for ( i = 0; i < 3;++i )
        {
          kFaceNormal[i][iface] = kFaceNormal[i][iface] / norm;
        }
      }
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "FACE_NORMAL_AVE: Recomputed " << nfix << " face normals\n";
    cout << "  by averaging face vertex normals.\n";
  }
  return;
}
//****************************************************************************80

void face_null_delete ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_NULL_DELETE deletes faces of order less than 3.
//
//  Discussion:
//
//    Thanks to Susan M. Fisher, University of North Carolina,
//    Department of Computer Science, for pointing out a coding error
//    in FACE_NULL_DELETE that was overwriting all the data!
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    30 November 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;
  int j;
  int kFaceCount2;
//
//  FACE_NUM2 is the number of faces we'll keep.
//
  kFaceCount2 = 0;
//
//  Check every face.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
//
//  Keep it only if it has order 3 or more.
//
    if ( 3 <= kFaceOrder[iface] )
    {
//
//  We don't have to slide data down in the array until
//  NUMFACE2 and IFACE get out of synch, that is, after
//  we've discarded at least one face.
//
      if ( kFaceCount2 != iface )
      {
        kFaceArea[kFaceCount2] = kFaceArea[iface];
        kFaceMaterial[kFaceCount2] = kFaceMaterial[iface];
        kFaceOrder[kFaceCount2] = kFaceOrder[iface];
        for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
        {
          face[ivert][kFaceCount2] = face[ivert][iface];
          kFaceVertexMaterial[ivert][kFaceCount2] = kFaceVertexMaterial[ivert][iface];
          for ( j = 0; j < 3; j++ )
          {
            kFaceVertexNormal[j][ivert][kFaceCount2] = kFaceVertexNormal[j][ivert][iface];
          }
        }

      }
//
//  Update the count only after we've used the un-incremented value
//  as a pointer.
//
      kFaceCount2 = kFaceCount2 + 1;

    }

  }

  cout << "\n";
  cout << "FACE_NULL_DELETE\n";
  cout << "  There are a total of " << kFaceCount << " faces.\n";
  cout << "  Of these, " << kFaceCount2 << " passed the order test.\n";

  kFaceCount = kFaceCount2;

  return;
}
//****************************************************************************80

int face_print ( int iface )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_PRINT prints out information about a face.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    31 August 1998
//
//  Author:
//
//    John Burkardt
//
{
  int ivert;
  int j;
  int k;

  if ( iface < 0 || kFaceCount-1 < iface )
  {
    cout << "\n";
    cout << "FACE_PRINT - Fatal error!\n";
    cout << "  Face indices must be between 1 and " << kFaceCount << "\n";
    cout << "  But your requested value was " << iface << "\n";
    return 1;
  }

  cout << "\n";
  cout << "FACE_PRINT\n";
  cout << "  Information about face " << iface << "\n";
  cout << "\n";
  cout << "  Number of vertices is " << kFaceOrder[iface] << "\n";
  cout << "\n";
  cout << "  Vertex list:\n";
  cout << "    Vertex #, Node #, Material #, X, Y, Z:\n";
  cout << "\n";
  for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
  {
    j = face[ivert][iface];
    k = kFaceVertexMaterial[ivert][iface];
    cout
     << setw(6)  << ivert      << "  "
     << setw(6)  << j          << "  "
     << setw(6)  << k          << "  "
     << setw(10) << cor3[0][j] << "  "
     << setw(10) << cor3[1][j] << "  "
     << setw(10) << cor3[2][j] << "\n";
  }

  cout << "\n";
  cout << "  Face normal vector:\n";
  cout << "\n";
  cout
    << setw(10) << kFaceNormal[0][iface] << "  "
    << setw(10) << kFaceNormal[1][iface] << "  "
    << setw(10) << kFaceNormal[2][iface] << "\n";

  cout << "\n";
  cout << "  Vertex face normals:\n";;
  cout << "\n";
  for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
  {
    cout
      << setw(6)  << ivert                          << "  "
      << setw(10) << kFaceVertexNormal[0][ivert][iface] << "  "
      << setw(10) << kFaceVertexNormal[1][ivert][iface] << "  "
      << setw(10) << kFaceVertexNormal[2][ivert][iface] << "\n";
  }

  return 0;

}
//****************************************************************************80

void face_reverse_order ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_REVERSE_ORDER reverses the order of the nodes in each face.
//
//  Discussion:
//
//    Reversing the order of the nodes requires that the normal vectors
//    be reversed as well, so this routine will automatically reverse
//    the normals associated with nodes, vertices and faces.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    28 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int itemp;
  int ivert;
  int j;
  int m;
  float temp;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    m = kFaceOrder[iface];

    for ( ivert = 0; ivert < ( m / 2 ); ivert++ )
    {
      itemp = face[ivert][iface];
      face[ivert][iface] = face[m-1-ivert][iface];
      face[m-1-ivert][iface] = itemp;

      itemp = kFaceVertexMaterial[ivert][iface];
      kFaceVertexMaterial[ivert][iface] = kFaceVertexMaterial[m-1-ivert][iface];
      kFaceVertexMaterial[m-1-ivert][iface] = itemp;

      for ( j = 0; j < 3; j++ )
      {
        temp = kFaceVertexNormal[j][ivert][iface];
        kFaceVertexNormal[j][ivert][iface] = kFaceVertexNormal[j][m-1-ivert][iface];
        kFaceVertexNormal[j][m-1-ivert][iface] = temp;
      }

      for ( j = 0; j < 2; j++ )
      {
        temp = kFaceVertexTexUv[j][ivert][iface];
        kFaceVertexTexUv[j][ivert][iface] = kFaceVertexTexUv[j][m-1-ivert][iface];
        kFaceVertexTexUv[j][m-1-ivert][iface] = temp;
      }

    }

  }

  for ( i = 0; i < kVertexCount;++i )
  {
    for ( j = 0; j < 3; j++ )
    {
      kVertexNormal[j][i] = - kVertexNormal[j][i];
    }
  }

  for ( i = 0; i < kFaceCount;++i )
  {
    for ( j = 0; j < 3; j++ )
    {
      kFaceNormal[j][i] = - kFaceNormal[j][i];
    }
  }

  cout << "\n";
  cout << "FACE_REVERSE_ORDER\n";
  cout << "  Each list of nodes defining a face\n";
  cout << "  has been reversed; related information,\n";
  cout << "  including normal vectors, was also updated.\n";

  return;
}
//****************************************************************************80

int face_subset ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_SUBSET selects a subset of the current faces as the new object.
//
//  Discussion:
//
//    The original graphic object is overwritten by the new one.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 October 1998
//
//  Author:
//
//    John Burkardt
//
//
{
  int i;
  int iface;
  int iface1;
  int iface2;
  int inc;
  int ivert;
  int j;
  int k;
  int kVertexCount2;

  line_num = 0;
//
//  Get the first and last faces to save, IFACE1 and IFACE2.
//
  cout << "\n";
  cout << "Enter lowest face number to save between 0 and "
       << kFaceCount - 1 << ":  ";
  scanf ( "%d", &iface1 );
  if ( iface1 < 0 || kFaceCount - 1 < iface1 )
  {
    cout << "Illegal choice!\n";
    return 1;
  }

  cout << "\n";
  cout << "Enter highest face number to save between "
    << iface1 << " and " << kFaceCount - 1 << ":  ";
  scanf ( "%d", &iface2 );
  if ( iface2 < iface1 || kFaceCount - 1 < iface2 )
  {
    cout << "Illegal choice!\n";
    return 1;
  }

  inc = iface1;
//
//  "Slide" the data for the saved faces down the face arrays.
//
  for ( iface = 0; iface < iface2 + 1 - iface1; iface++ )
  {
    kFaceOrder[iface] = kFaceOrder[iface+inc];
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      face[ivert][iface] = face[ivert][iface+inc];
      kFaceVertexMaterial[ivert][iface] = kFaceVertexMaterial[ivert][iface+inc];
      for ( i = 0; i < 3;++i )
      {
        kFaceVertexNormal[i][ivert][iface] =
          kFaceVertexNormal[i][ivert][iface+inc];
        kFaceVertexRgb[i][ivert][iface] = kFaceVertexRgb[i][ivert][iface+inc];
      }
    }
    for ( i = 0; i < 3;++i )
    {
      kFaceNormal[i][iface] = kFaceNormal[i][iface+inc];
    }
  }
//
//  Now reset the number of faces.
//
  kFaceCount = iface2 + 1 - iface1;
//
//  Now, for each point I, set LIST(I) = J if point I is the J-th
//  point we are going to save, and 0 otherwise.  Then J will be
//  the new label of point I.
//
  for ( i = 0; i < kVertexCount;++i )
  {
    list[i] = -1;
  }

  kVertexCount2 = 0;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      j = face[ivert][iface];
      if ( list[j] == -1 )
      {
        kVertexCount2 = kVertexCount2 + 1;
        list[j] = kVertexCount2;
      }
    }
  }
//
//  Now make the nonzero list entries rise in order, so that
//  we can compress the COR3 data in a minute.
//
  kVertexCount2 = 0;

  for ( i = 0; i < kVertexCount;++i )
  {
    if ( list[i] != -1 )
    {
      list[i] = kVertexCount2;
      kVertexCount2 = kVertexCount2 + 1;
    }
  }
//
//  Relabel the FACE array with the new node indices.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      j = face[ivert][iface];
      face[ivert][iface] = list[j];
    }
  }
//
//  Rebuild the COR3 array by sliding data down.
//
  for ( i = 0; i < kVertexCount;++i )
  {
    k = list[i];
    if ( k != -1 )
    {
      for ( j = 0; j < 3; j++ )
      {
        cor3[j][k] = cor3[j][i];
      }
    }
  }

  kVertexCount = kVertexCount2;

  return 0;
}
//****************************************************************************80

void face_to_line ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_TO_LINE converts face information to line information.
//
//  Discussion:
//
//    In some cases, the graphic information represented by polygonal faces
//    must be converted to a representation based solely on line segments.
//    This is particularly true if a VLA file is being written.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int icor3;
  int iface;
  int ivert;
  int jcor3;
  int jvert;
//
//  Case 0:
//  No line pruning.
//
  if ( line_prune == 0 )
  {
    for ( iface = 0; iface < kFaceCount; iface++ )
    {
      for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
      {
        icor3 = face[ivert][iface];

        line_num = line_num + 1;
        if ( line_num <= LINES_MAX )
        {
          line_dex[line_num] = icor3;
          line_material[line_num] = kFaceVertexMaterial[ivert][iface];
        }
      }

      ivert = 0;
      icor3 = face[ivert][iface];

      line_num = line_num + 1;
      if ( line_num <= LINES_MAX )
      {
        line_dex[line_num] = icor3;
        line_material[line_num] = kFaceVertexMaterial[ivert][iface];
      }

      line_num = line_num + 1;
      if ( line_num <= LINES_MAX )
      {
        line_dex[line_num] = -1;
        line_material[line_num] = -1;
      }
    }

  }
//
//  Case 2:
//  Simple-minded line pruning.
//  Only draw line (I,J) if I < J.
//
  else
  {
    for ( iface = 0; iface < kFaceCount; iface++ )
    {
      for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
      {
        icor3 = face[ivert][iface];

        if ( ivert + 1 < kFaceOrder[iface] )
        {
          jvert = ivert + 1;
        }
        else
        {
          jvert = 0;
        }

        jcor3 = face[jvert][iface];

        if ( icor3 < jcor3 )
        {
          if ( line_num + 3 < LINES_MAX )
          {
            line_num = line_num + 1;
            line_dex[line_num] = icor3;
            line_material[line_num] = kFaceVertexMaterial[ivert][iface];

            line_num = line_num + 1;
            line_dex[line_num] = jcor3;
            line_material[line_num] = kFaceVertexMaterial[jvert][iface];

            line_num = line_num + 1;
            line_dex[line_num] = -1;
            line_material[line_num] = -1;

          }
        }
      }
    }

  }

  return;
}
//****************************************************************************80

void face_to_vertex_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_TO_VERTEX_MAT extends face material definitions to vertices.
//
//  Discussion:
//
//    Assuming material indices are defined for all the faces, this
//    routine assigns to each vertex of a face the material of that face.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      kFaceVertexMaterial[ivert][iface] = kFaceMaterial[iface];
    }
  }

  return;
}
//****************************************************************************80

char *file_ext ( char *file_name )

//****************************************************************************80
//
//  Purpose:
//
//    FILE_EXT picks out the extension in a file name.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    21 July 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;

  i = ch_index_last ( file_name, '.' );

  if ( i == -1 )
  {
    return NULL;
  }
  else
  {
    return file_name + i + 1;
  }
}
//****************************************************************************80

float float_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_READ reads 1 float from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  float rval;
  float temp;

  fread ( &temp, sizeof ( float ), 1, filein );

  if ( byte_swap )
  {
    rval = float_reverse_bytes ( temp );
  }
  else
  {
    rval = temp;
  }

  return rval;
}
//****************************************************************************80

float float_reverse_bytes ( float x )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_REVERSE_BYTES reverses the four bytes in a float.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    X, a float whose bytes are to be reversed.
//
//    FLOAT_REVERSE_BYTES, a float with bytes in reverse order from those in X.
//
{
  char c;
  union {
    float yfloat;
    char ychar[4];
  } y;

  y.yfloat = x;

  c = y.ychar[0];
  y.ychar[0] = y.ychar[3];
  y.ychar[3] = c;

  c = y.ychar[1];
  y.ychar[1] = y.ychar[2];
  y.ychar[2] = c;

  return ( y.yfloat );
}
//****************************************************************************80

int float_write ( FILE *fileout, float float_val )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_WRITE writes 1 float to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 September 1998
//
//  Author:
//
//    John Burkardt
//
{
  int nbyte = sizeof ( float );
  float temp;

  if ( byte_swap )
  {
    temp = float_reverse_bytes ( float_val );
  }
  else
  {
    temp = float_val;
  }

  fwrite ( &temp, nbyte, 1, fileout );

  return nbyte;
}
//****************************************************************************80

bool gmod_arch_check ( )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_ARCH_CHECK inquires into some features of the computer architecture.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 September 2003
//
//  Author:
//
//    Zik Saleeba
//
//  Parameters:
//
//    Output, bool GMOD_ARCH_CHECK is TRUE if the size of a float is four bytes
//    and we use IEEE floats (either big-endian or little-endian).
//
{
  static unsigned char one[4];
  int temp;

  temp = sizeof ( float );
  if ( temp != 4 )
  {
    return false;
  }

  *(float *)one = 1.0;
//
//  Little endian IEEE floats.
//
  if ( one[0] == 0 && one[1] == 0 && one[2] == 128 && one[3] == 63 )
  {
    return true;
  }
//
//  Big endian IEEE floats.
//
  if ( one[0] == 63 && one[1] == 128 && one[2] == 0 && one[3] == 0 )
  {
    return true;
  }

  return false;
}

//****************************************************************************80

int i4_max ( int i1, int i2 )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MAX returns the maximum of two I4s.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, are two integers to be compared.
//
//    Output, int I4_MAX, the larger of I1 and I2.
//
{
  if ( i2 < i1 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//****************************************************************************80

int i4_min ( int i1, int i2 )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MIN returns the smaller of two I4's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, two integers to be compared.
//
//    Output, int I4_MIN, the smaller of I1 and I2.
//
{
  if ( i1 < i2 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//****************************************************************************80

int i4_modp ( int i, int j )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MODP returns the nonnegative remainder of I4 division.
//
//  Discussion:
//
//    If
//      NREM = I4_MODP ( I, J )
//      NMULT = ( I - NREM ) / J
//    then
//      I = J * NMULT + NREM
//    where NREM is always nonnegative.
//
//    The MOD function computes a result with the same sign as the
//    quantity being divided.  Thus, suppose you had an angle A,
//    and you wanted to ensure that it was between 0 and 360.
//    Then mod(A,360) would do, if A was positive, but if A
//    was negative, your result would be between -360 and 0.
//
//    On the other hand, I4_MODP(A,360) is between 0 and 360, always.
//
//  Example:
//
//        I         J     MOD  I4_MODP   I4_MODP Factorization
//
//      107        50       7       7    107 =  2 *  50 + 7
//      107       -50       7       7    107 = -2 * -50 + 7
//     -107        50      -7      43   -107 = -3 *  50 + 43
//     -107       -50      -7      43   -107 =  3 * -50 + 43
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I, the number to be divided.
//
//    Input, int J, the number that divides I.
//
//    Output, int I4_MODP, the nonnegative remainder when I is
//    divided by J.
//
{
  int value;

  if ( j == 0 )
  {
    cout << "\n";
    cout << "I4_MODP - Fatal error!\n";
    cout << "  I4_MODP ( I, J ) called with J = " << j << "\n";
    exit ( 1 );
  }

  value = i % j;

  if ( value < 0 )
  {
    value = value + abs ( j );
  }

  return value;
}
//****************************************************************************80

int i4_wrap ( int ival, int ilo, int ihi )

//****************************************************************************80
//
//  Purpose:
//
//    I4_WRAP forces an I4 to lie between given limits by wrapping.
//
//  Example:
//
//    ILO = 4, IHI = 8
//
//    I  I4_WRAP
//
//    -2     8
//    -1     4
//     0     5
//     1     6
//     2     7
//     3     8
//     4     4
//     5     5
//     6     6
//     7     7
//     8     8
//     9     4
//    10     5
//    11     6
//    12     7
//    13     8
//    14     4
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int IVAL, an integer value.
//
//    Input, int ILO, IHI, the desired bounds for the integer value.
//
//    Output, int I4_WRAP, a "wrapped" version of IVAL.
//
{
  int jhi;
  int jlo;
  int value;
  int wide;

  jlo = i4_min ( ilo, ihi );
  jhi = i4_max ( ilo, ihi );

  wide = jhi + 1 - jlo;

  if ( wide == 1 )
  {
    value = jlo;
  }
  else
  {
    value = jlo + i4_modp ( ival - jlo, wide );
  }

  return value;
}
//****************************************************************************80

void init_program_data ( )

//****************************************************************************80
//
//  Purpose:
//
//    INIT_PROGRAM_DATA initializes the internal program data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  byte_swap = false;
  debug = false;
  line_prune = 1;
  color_num = 0;
  kVertexCount = 0;
  kFaceCount = 0;
  line_num = 0;

  kVertex[0] = cor3[0];
  kVertex[1] = cor3[1];
  kVertex[2] = cor3[2];

  for (int i = 0 ; i < ORDER_MAX ;++i)
	  kFace[i]=face[i];
  if ( debug )
  {
    cout << "\n";
    cout << "INIT_PROGRAM_DATA: Program data initialized.\n";
  }

  return;

}

//****************************************************************************80

int i4vec_max ( int n, int *a )

//****************************************************************************80
//
//  Purpose:
//
//    I4VEC_MAX returns the maximum element in an I4VEC.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int  i;
  int *ia;
  int  imax;

  if ( n <= 0 )
  {
    imax = 0;
  }
  else
  {
    ia = a;
    imax = *ia;
    for ( i = 1; i < n;++i )
    {
      ia = ia + 1;
      if ( imax < *ia )
      {
        imax = *ia;
      }
    }
  }
  return imax;
}

//****************************************************************************80

long int long_int_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    LONG_INT_READ reads a long int from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  union {
    long int yint;
    char ychar[4];
  } y;

  if ( byte_swap )
  {
    y.ychar[3] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[0] = fgetc ( filein );
  }
  else
  {
    y.ychar[0] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[3] = fgetc ( filein );
  }

  return y.yint;
}
//****************************************************************************80

int long_int_write ( FILE *fileout, long int int_val )

//****************************************************************************80
//
//  Purpose:
//
//    LONG_INT_WRITE writes a long int to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    long int yint;
    char ychar[4];
  } y;

  y.yint = int_val;

  if ( byte_swap )
  {
    fputc ( y.ychar[3], fileout );
    fputc ( y.ychar[2], fileout );
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[0], fileout );
  }
  else
  {
    fputc ( y.ychar[0], fileout );
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[2], fileout );
    fputc ( y.ychar[3], fileout );
  }

  return 4;
}
//****************************************************************************80



void node_to_vertex_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    NODE_TO_VERTEX_MAT extends node material definitions to vertices.
//
//  Discussion:
//
//    A NODE is a point in space.
//    A VERTEX is a node as used in a particular face.
//    One node may be used as a vertex in several faces, or none.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;
  int node;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      node = face[ivert][iface];
      kFaceVertexMaterial[ivert][iface] = kVertexMaterial[node];
    }
  }

  return;
}
//****************************************************************************80

int obj_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    OBJ_READ reads a Wavefront OBJ file.
//
//  Example:
//
//    #  magnolia.obj
//
//    mtllib ./vp.mtl
//
//    g
//    v -3.269770 -39.572201 0.876128
//    v -3.263720 -39.507999 2.160890
//    ...
//    v 0.000000 -9.988540 0.000000
//    g stem
//    s 1
//    usemtl brownskn
//    f 8 9 11 10
//    f 12 13 15 14
//    ...
//    f 788 806 774
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int count;
  int i;
  int ivert;
  char *next;
  char *next2;
  char *next3;
  int node;
  int vertex_normal_num;
  float r1;
  float r2;
  float r3;
  char token[LINE_MAX_LEN];
  char token2[LINE_MAX_LEN];
  int width;
//
//  Initialize.
//
  vertex_normal_num = 0;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' )
    {
      continue;
    }

    if ( *next == '#' || *next == '$' )
    {
      comment_num = comment_num + 1;
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  BEVEL
//  Bevel interpolation.
//
    if ( s_eqi ( token, "BEVEL" ) )
    {
      continue;
    }
//
//  BMAT
//  Basis matrix.
//
    else if ( s_eqi ( token, "BMAT" ) )
    {
      continue;
    }
//
//  C_INTERP
//  Color interpolation.
//
    else if ( s_eqi ( token, "C_INTERP" ) )
    {
      continue;
    }
//
//  CON
//  Connectivity between free form surfaces.
//
    else if ( s_eqi ( token, "CON" ) )
    {
      continue;
    }
//
//  CSTYPE
//  Curve or surface type.
//
    else if ( s_eqi ( token, "CSTYPE" ) )
    {
      continue;
    }
//
//  CTECH
//  Curve approximation technique.
//
    else if ( s_eqi ( token, "CTECH" ) )
    {
      continue;
    }
//
//  CURV
//  Curve.
//
    else if ( s_eqi ( token, "CURV" ) )
    {
      continue;
    }
//
//  CURV2
//  2D curve.
//
    else if ( s_eqi ( token, "CURV2" ) )
    {
      continue;
    }
//
//  D_INTERP
//  Dissolve interpolation.
//
    else if ( s_eqi ( token, "D_INTERP" ) )
    {
      continue;
    }
//
//  DEG
//  Degree.
//
    else if ( s_eqi ( token, "DEG" ) )
    {
      continue;
    }
//
//  END
//  End statement.
//
    else if ( s_eqi ( token, "END" ) )
    {
      continue;
    }
//
//  F V1 V2 V3
//    or
//  F V1/VT1/VN1 V2/VT2/VN2 ...
//    or
//  F V1//VN1 V2//VN2 ...
//
//  Face.
//  A face is defined by the vertices.
//  Optionally, slashes may be used to include the texture vertex
//  and vertex normal indices.
//
//  OBJ line node indices are 1 based rather than 0 based.
//  So we have to decrement them before loading them into FACE.
//
    else if ( s_eqi ( token, "F" ) )
    {
      ivert = 0;
      kFaceOrder[kFaceCount] = 0;
//
//  Read each item in the F definition as a token, and then
//  take it apart.
//
      for ( ;; )
      {
        count = sscanf ( next, "%s%n", token2, &width );
        next = next + width;

        if ( count != 1 )
        {
          break;
        }

        count = sscanf ( token2, "%d%n", &node, &width );
        next2 = token2 + width;

        if ( count != 1 )
        {
          break;
        }

        if ( ivert < ORDER_MAX && kFaceCount < FACE_MAX )
        {
          face[ivert][kFaceCount] = node-1;
          kFaceVertexMaterial[ivert][kFaceCount] = 0;
          kFaceOrder[kFaceCount] = kFaceOrder[kFaceCount] + 1;
        }
//
//  If there's a slash, skip to the next slash, and extract the
//  index of the normal vector.
//
        if ( *next2 == '/' )
        {
          for ( next3 = next2 + 1; next3 < token2 + LINE_MAX_LEN; next3++ )
          {
            if ( *next3 == '/' )
            {
              next3 = next3 + 1;
              count = sscanf ( next3, "%d%n", &node, &width );

              node = node - 1;
              if ( 0 <= node && node < vertex_normal_num )
              {
                for ( i = 0; i < 3;++i )
                {
                  kFaceVertexNormal[i][ivert][kFaceCount] = normal_temp[i][node];
                }
              }
              break;
            }
          }
        }
        ivert = ivert + 1;
      }
      kFaceCount = kFaceCount + 1;
    }
//
//  G
//  Group name.
//
    else if ( s_eqi ( token, "G" ) )
    {
      continue;
    }
//
//  HOLE
//  Inner trimming hole.
//
    else if ( s_eqi ( token, "HOLE" ) )
    {
      continue;
    }
//
//  L
//  I believe OBJ line node indices are 1 based rather than 0 based.
//  So we have to decrement them before loading them into LINE_DEX.
//
    else if ( s_eqi ( token, "L" ) )
    {
      for ( ;; )
      {
        count = sscanf ( next, "%d%n", &node, &width );
        next = next + width;

        if ( count != 1 )
        {
          break;
        }

        if ( line_num < LINES_MAX  )
        {
          line_dex[line_num] = node-1;
          line_material[line_num] = 0;
        }
        line_num = line_num + 1;

      }

      if ( line_num < LINES_MAX )
      {
        line_dex[line_num] = -1;
        line_material[line_num] = -1;
      }
      line_num = line_num + 1;

    }
//
//  LOD
//  Level of detail.
//
    else if ( s_eqi ( token, "LOD" ) )
    {
      continue;
    }
//
//  MG
//  Merging group.
//
    else if ( s_eqi ( token, "MG" ) )
    {
      continue;
    }
//
//  MTLLIB
//  Material library.
//

    else if ( s_eqi ( token, "MTLLIB" ) )
    {
      continue;
    }
//
//  O
//  Object name.
//
    else if ( s_eqi ( token, "O" ) )
    {
      continue;
    }
//
//  P
//  Point.
//
    else if ( s_eqi ( token, "P" ) )
    {
      continue;
    }
//
//  PARM
//  Parameter values.
//
    else if ( s_eqi ( token, "PARM" ) )
    {
      continue;
    }
//
//  S
//  Smoothing group
//
    else if ( s_eqi ( token, "S" ) )
    {
      continue;
    }
//
//  SCRV
//  Special curve.
//
    else if ( s_eqi ( token, "SCRV" ) )
    {
      continue;
    }
//
//  SHADOW_OBJ
//  Shadow casting.
//
    else if ( s_eqi ( token, "SHADOW_OBJ" ) )
    {
      continue;
    }
//
//  SP
//  Special point.
//
    else if ( s_eqi ( token, "SP" ) )
    {
      continue;
    }
//
//  STECH
//  Surface approximation technique.
//
    else if ( s_eqi ( token, "STECH" ) )
    {
      continue;
    }
//
//  STEP
//  Stepsize.
//
    else if ( s_eqi ( token, "CURV" ) )
    {
      continue;
    }
//
//  SURF
//  Surface.
//
    else if ( s_eqi ( token, "SURF" ) )
    {
      continue;
    }
//
//  TRACE_OBJ
//  Ray tracing.
//
    else if ( s_eqi ( token, "TRACE_OBJ" ) )
    {
      continue;
    }
//
//  TRIM
//  Outer trimming loop.
//
    else if ( s_eqi ( token, "TRIM" ) )
    {
      continue;
    }
//
//  USEMTL
//  Material name.
//
    else if ( s_eqi ( token, "USEMTL" ) )
    {
      continue;
    }
//
//  V X Y Z W
//  Geometric vertex.
//  W is optional, a weight for rational curves and surfaces.
//  The default for W is 1.
//
    else if ( s_eqi ( token, "V" ) )
    {
      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      if ( kVertexCount < COR3_MAX )
      {
        cor3[0][kVertexCount] = r1;
        cor3[1][kVertexCount] = r2;
        cor3[2][kVertexCount] = r3;
      }

      kVertexCount = kVertexCount + 1;

    }
//
//  VN
//  Vertex normals.
//
    else if ( s_eqi ( token, "VN" ) )
    {
      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      if ( vertex_normal_num < ORDER_MAX * FACE_MAX )
      {
        normal_temp[0][vertex_normal_num] = r1;
        normal_temp[1][vertex_normal_num] = r2;
        normal_temp[2][vertex_normal_num] = r3;
      }

      vertex_normal_num = vertex_normal_num + 1;

    }
//
//  VT
//  Vertex texture.
//
    else if ( s_eqi ( token, "VT" ) )
    {
      continue;
    }
//
//  VP
//  Parameter space vertices.
//
    else if ( s_eqi ( token, "VP" ) )
    {
      continue;
    }
//
//  Unrecognized
//
    else
    {
      bad_num = bad_num + 1;
    }

  }
  return 0;
}
//****************************************************************************80

int obj_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    OBJ_WRITE writes a Wavefront OBJ file.
//
//  Example:
//
//    #  magnolia.obj
//
//    mtllib ./vp.mtl
//
//    g
//    v -3.269770 -39.572201 0.876128
//    v -3.263720 -39.507999 2.160890
//    ...
//    v 0.000000 -9.988540 0.000000
//    g stem
//    s 1
//    usemtl brownskn
//    f 8 9 11 10
//    f 12 13 15 14
//    ...
//    f 788 806 774
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 September 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, FILE *FILEOUT, a pointer to the output file.
//
{
  int i;
  int iface;
  int indexvn;
  int ivert;
  int k;
  bool next;
  int text_num;
  float w;
//
//  Initialize.
//
  text_num = 0;
  w = 1.0;

  fprintf ( fileout, "# %s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "# Original data in %s.\n", filein_name );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "g %s\n", object_name );
  fprintf ( fileout, "\n" );

  text_num = text_num + 5;
//
//  V: vertex coordinates.
//
  for ( i = 0; i < kVertexCount;++i )
  {
    fprintf ( fileout, "v %f %f %f %f\n",
      cor3[0][i], cor3[1][i], cor3[2][i], w );
    text_num = text_num + 1;
  }
//
//  VN: Vertex face normal vectors.
//
  if ( 0 < kFaceCount )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      fprintf ( fileout, "vn %f %f %f\n", kFaceVertexNormal[0][ivert][iface],
        kFaceVertexNormal[1][ivert][iface], kFaceVertexNormal[2][ivert][iface] );
      text_num = text_num + 1;
    }
  }
//
//  F: faces.
//
  if ( 0 < kFaceCount )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  indexvn = 0;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    fprintf ( fileout, "f" );
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      indexvn = indexvn + 1;
      fprintf ( fileout, " %d//%d", face[ivert][iface]+1, indexvn );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }
//
//  L: lines.
//
  if ( 0 < line_num )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  next = true;

  for ( i = 0; i < line_num;++i )
  {
    k = line_dex[i];

    if ( k == -1 )
    {
      fprintf ( fileout, "\n" );
      text_num = text_num + 1;
      next = true;
    }
    else
    {
      if ( next )
      {
        fprintf ( fileout, "l" );
        next = false;
      }
      fprintf ( fileout, " %d", k+1 );
    }

  }

  fprintf ( fileout, "\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "OBJ_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}


int rcol_find ( float a[][COR3_MAX], int m, int n, float r[] )

//****************************************************************************80
//
//  Purpose:
//
//    RCOL_FIND finds if a vector occurs in a table.
//
//  Discussion:
//
//    Explicitly forcing the second dimension to be COR3_MAX is a kludge.
//    I have to figure out how to do this as pointer references.
//
//    Also, since the array is not sorted, this routine should not be carelessly
//    called repeatedly for really big values of N, because you'll waste a
//    lot of time.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    27 April 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int icol;
  int j;

  icol = -1;

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m;++i )
    {
      if ( a[i][j] != r[i] )
      {
        break;
      }
      if ( i == m-1 )
      {
        return j;
      }
    }
  }

  return icol;
}
//****************************************************************************80

float rgb_to_hue ( float r, float g, float b )

//****************************************************************************80
//
//  Purpose:
//
//    RGB_TO_HUE converts (R,G,B) colors to a hue value between 0 and 1.
//
//  Discussion:
//
//    The hue computed here should be the same as the H value computed
//    for HLS and HSV, except that it ranges from 0 to 1 instead of
//    0 to 360.
//
//    A monochromatic color ( white, black, or a shade of gray) does not
//    have a hue.  This routine will return a special value of H = -1
//    for such cases.
//
//  Example:
//
//    Color    R    G    B     H
//
//    red  1.0  0.0  0.0   0.00
//    yellow   1.0  1.0  0.0   0.16
//    green    0.0  1.0  0.0   0.33
//    cyan     0.0  1.0  1.0   0.50
//    blue     0.0  0.0  1.0   0.67
//    magenta  1.0  0.0  1.0   0.83
//
//    black    0.0  0.0  0.0  -1.00
//    gray     0.5  0.5  0.5  -1.00
//    white    1.0  1.0  1.0  -1.00
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, float R, G, B, the red, green and blue values of the color.
//    These values should be between 0 and 1.
//
//    Output, float RGB_TO_HUE, the corresponding hue of the color, or -1.0 if
//    the color is monochromatic.
//
{
  float h;
  float rgbmax;
  float rgbmin;
//
//  Make sure the colors are between 0 and 1.
//
  if ( r < 0.0 )
  {
    r = 0.0;
  }
  else if ( 1.0 < r )
  {
    r = 1.0;
  }

  if ( g < 0.0 )
  {
    g = 0.0;
  }
  else if ( 1.0 < g )
  {
    g = 1.0;
  }

  if ( b < 0.0 )
  {
    b = 0.0;
  }
  else if ( 1.0 < b )
  {
    b = 1.0;
  }
//
//  Compute the minimum and maximum of R, G and B.
//
  rgbmax = r;
  if ( rgbmax < g )
  {
    rgbmax = g;
  }
  if ( rgbmax < b )
  {
    rgbmax = b;
  }

  rgbmin = r;
  if ( g < rgbmin )
  {
    rgbmin = g;
  }
  if ( b < rgbmin )
  {
    rgbmin = b;
  }
//
//  If RGBMAX = RGBMIN, the color has no hue.
//
  if ( rgbmax == rgbmin )
  {
    h = - 1.0;
  }
//
//  Otherwise, we need to determine the dominant color.
//
  else
  {
    if ( r == rgbmax )
    {
      h = ( g - b ) / ( rgbmax - rgbmin );
    }
    else if ( g == rgbmax )
    {
      h = 2.0 + ( b - r ) / ( rgbmax - rgbmin );
    }
    else if ( b == rgbmax )
    {
      h = 4.0 + ( r - g ) / ( rgbmax - rgbmin );
    }

    h = h / 6.0;
//
//  Make sure H lies between 0 and 1.0.
//
    if ( h < 0.0 )
    {
      h = h + 1.0;
    }
    else if ( 1.0 < h )
    {
      h = h - 1.0;
    }

  }

  return h;
}
//****************************************************************************80

bool s_eqi ( char *s1, char *s2 )

//****************************************************************************80
//
//  Purpose:
//
//    S_EQI reports whether two strings are equal, ignoring case.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S1, char *S2, pointers to two strings.
//
//    Output, bool S_EQI, is true if the strings are equal.
//
{
  int i;
  int nchar;
  int nchar1;
  int nchar2;

  nchar1 = strlen ( s1 );
  nchar2 = strlen ( s2 );
  nchar = i4_min ( nchar1, nchar2 );

//
//  The strings are not equal if they differ over their common length.
//
  for ( i = 0; i < nchar;++i )
  {

    if ( ch_cap ( s1[i] ) != ch_cap ( s2[i] ) )
    {
      return false;
    }
  }
//
//  The strings are not equal if the longer one includes nonblanks
//  in the tail.
//
  if ( nchar < nchar1 )
  {
    for ( i = nchar; i < nchar1;++i )
    {
      if ( s1[i] != ' ' )
      {
        return false;
      }
    }
  }
  else if ( nchar < nchar2 )
  {
    for ( i = nchar; i < nchar2;++i )
    {
      if ( s2[i] != ' ' )
      {
        return false;
      }
    }
  }

  return true;

}
//****************************************************************************80

int s_len_trim ( char *s )

//****************************************************************************80
//
//  Purpose:
//
//    S_LEN_TRIM returns the length of a string to the last nonblank.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a pointer to a string.
//
//    Output, int S_LEN_TRIM, the length of the string to the last nonblank.
//    If S_LEN_TRIM is 0, then the string is entirely blank.
//
{
  int n;
  char *t;

  n = strlen ( s );
  t = s + strlen ( s ) - 1;

  while ( 0 < n )
  {
    if ( *t != ' ' )
    {
      return n;
    }
    t--;
    n--;
  }

  return n;
}
//****************************************************************************80

int s_to_i4 ( char *s, int *last, bool *error )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_I4 reads an I4 from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a string to be examined.
//
//    Output, int *LAST, the last character of S used to make IVAL.
//
//    Output, bool *ERROR is TRUE if an error occurred.
//
//    Output, int *S_TO_I4, the integer value read from the string.
//    If the string is blank, then IVAL will be returned 0.
//
{
  char c;
  int i;
  int isgn;
  int istate;
  int ival;

  *error = false;
  istate = 0;
  isgn = 1;
  i = 0;
  ival = 0;

  while ( *s )
  {
    c = s[i];
    i = i + 1;
//
//  Haven't read anything.
//
    if ( istate == 0 )
    {
      if ( c == ' ' )
      {
      }
      else if ( c == '-' )
      {
        istate = 1;
        isgn = -1;
      }
      else if ( c == '+' )
      {
        istate = 1;
        isgn = + 1;
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = true;
        return ival;
      }
    }
//
//  Have read the sign, expecting digits.
//
    else if ( istate == 1 )
    {
      if ( c == ' ' )
      {
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = true;
        return ival;
      }
    }
//
//  Have read at least one digit, expecting more.
//
    else if ( istate == 2 )
    {
      if ( '0' <= c && c <= '9' )
      {
        ival = 10 * (ival) + c - '0';
      }
      else
      {
        ival = isgn * ival;
        *last = i - 1;
        return ival;
      }

    }
  }
//
//  If we read all the characters in the string, see if we're OK.
//
  if ( istate == 2 )
  {
    ival = isgn * ival;
    *last = s_len_trim ( s );
  }
  else
  {
    *error = true;
    *last = 0;
  }

  return ival;
}
//****************************************************************************80

bool s_to_i4vec ( char *s, int n, int ivec[] )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_I4VEC reads an I4VEC from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 January 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, the string to be read.
//
//    Input, int N, the number of values expected.
//
//    Output, int IVEC[N], the values read from the string.
//
//    Output, bool S_TO_I4VEC, is TRUE if an error occurred.
//
{
  bool error;
  int i;
  int lchar;

  error = false;

  for ( i = 0; i < n;++i )
  {
    ivec[i] = s_to_i4 ( s, &lchar, &error );

    if ( error )
    {
      cout << "\n";
      cout << "S_TO_I4VEC - Fatal error!\n";
      cout << "  S_TO_I4 returned error while reading item " << i << "\n";
      return error;
    }

    s = s + lchar;

  }

  return error;
}
//****************************************************************************80

float s_to_r4 ( char *s, int *lchar, bool *error )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_R4 reads an R4 from a string.
//
//  Discussion:
//
//    This routine will read as many characters as possible until it reaches
//    the end of the string, or encounters a character which cannot be
//    part of the real number.
//
//    Legal input is:
//
//       1 blanks,
//       2 '+' or '-' sign,
//       2.5 spaces
//       3 integer part,
//       4 decimal point,
//       5 fraction part,
//       6 'E' or 'e' or 'D' or 'd', exponent marker,
//       7 exponent sign,
//       8 exponent integer part,
//       9 exponent decimal point,
//      10 exponent fraction part,
//      11 blanks,
//      12 final comma or semicolon.
//
//    with most quantities optional.
//
//  Example:
//
//    S                 R
//
//    '1'               1.0
//    '     1   '       1.0
//    '1A'              1.0
//    '12,34,56'        12.0
//    '  34 7'          34.0
//    '-1E2ABCD'        -100.0
//    '-1X2ABCD'        -1.0
//    ' 2E-1'           0.2
//    '23.45'           23.45
//    '-4.2E+2'         -420.0
//    '17d2'            1700.0
//    '-14e-2'         -0.14
//    'e2'              100.0
//    '-12.73e-9.23'   -12.73 * 10.0**(-9.23)
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    07 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, the string containing the
//    data to be read.  Reading will begin at position 1 and
//    terminate at the end of the string, or when no more
//    characters can be read to form a legal real.  Blanks,
//    commas, or other nonnumeric data will, in particular,
//    cause the conversion to halt.
//
//    Output, int *LCHAR, the number of characters read from
//    the string to form the number, including any terminating
//    characters such as a trailing comma or blanks.
//
//    Output, bool *ERROR, is true if an error occurred.
//
//    Output, float S_TO_R4, the real value that was read from the string.
//
{
  char c;
  int ihave;
  int isgn;
  int iterm;
  int jbot;
  int jsgn;
  int jtop;
  int nchar;
  int ndig;
  float r;
  float rbot;
  float rexp;
  float rtop;
  char TAB = 9;

  nchar = s_len_trim ( s );
  *error = false;
  r = 0.0E+00;
  *lchar = -1;
  isgn = 1;
  rtop = 0.0E+00;
  rbot = 1.0E+00;
  jsgn = 1;
  jtop = 0;
  jbot = 1;
  ihave = 1;
  iterm = 0;

  for ( ; ; )
  {
    c = s[*lchar+1];
    *lchar = *lchar + 1;
//
//  Blank or TAB character.
//
    if ( c == ' ' || c == TAB )
    {
      if ( ihave == 2 )
      {
      }
      else if ( ihave == 6 || ihave == 7 )
      {
        iterm = 1;
      }
      else if ( 1 < ihave )
      {
        ihave = 11;
      }
    }
//
//  Comma.
//
    else if ( c == ',' || c == ';' )
    {
      if ( ihave != 1 )
      {
        iterm = 1;
        ihave = 12;
        *lchar = *lchar + 1;
      }
    }
//
//  Minus sign.
//
    else if ( c == '-' )
    {
      if ( ihave == 1 )
      {
        ihave = 2;
        isgn = -1;
      }
      else if ( ihave == 6 )
      {
        ihave = 7;
        jsgn = -1;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Plus sign.
//
    else if ( c == '+' )
    {
      if ( ihave == 1 )
      {
        ihave = 2;
      }
      else if ( ihave == 6 )
      {
        ihave = 7;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Decimal point.
//
    else if ( c == '.' )
    {
      if ( ihave < 4 )
      {
        ihave = 4;
      }
      else if ( 6 <= ihave && ihave <= 8 )
      {
        ihave = 9;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Exponent marker.
//
    else if ( ch_eqi ( c, 'E' ) || ch_eqi ( c, 'D' ) )
    {
      if ( ihave < 6 )
      {
        ihave = 6;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Digit.
//
    else if ( ihave < 11 && '0' <= c && c <= '9' )
    {
      if ( ihave <= 2 )
      {
        ihave = 3;
      }
      else if ( ihave == 4 )
      {
        ihave = 5;
      }
      else if ( ihave == 6 || ihave == 7 )
      {
        ihave = 8;
      }
      else if ( ihave == 9 )
      {
        ihave = 10;
      }

      ndig = ch_to_digit ( c );

      if ( ihave == 3 )
      {
        rtop = 10.0E+00 * rtop + ( float ) ndig;
      }
      else if ( ihave == 5 )
      {
        rtop = 10.0E+00 * rtop + ( float ) ndig;
        rbot = 10.0E+00 * rbot;
      }
      else if ( ihave == 8 )
      {
        jtop = 10 * jtop + ndig;
      }
      else if ( ihave == 10 )
      {
        jtop = 10 * jtop + ndig;
        jbot = 10 * jbot;
      }
    }
//
//  Anything else is regarded as a terminator.
//
    else
    {
      iterm = 1;
    }
//
//  If we haven't seen a terminator, and we haven't examined the
//  entire string, go get the next character.
//
    if ( iterm == 1 || nchar <= *lchar + 1 )
    {
      break;
    }

  }
//
//  If we haven't seen a terminator, and we have examined the
//  entire string, then we're done, and LCHAR is equal to NCHAR.
//
  if ( iterm != 1 && (*lchar) + 1 == nchar )
  {
    *lchar = nchar;
  }
//
//  Number seems to have terminated.  Have we got a legal number?
//  Not if we terminated in states 1, 2, 6 or 7!
//
  if ( ihave == 1 || ihave == 2 || ihave == 6 || ihave == 7 )
  {
    *error = true;
    return r;
  }
//
//  Number seems OK.  Form it.
//
  if ( jtop == 0 )
  {
    rexp = 1.0E+00;
  }
  else
  {
    if ( jbot == 1 )
    {
      rexp = pow ( ( double ) 10.0E+00, ( double ) ( jsgn * jtop ) );
    }
    else
    {
      rexp = jsgn * jtop;
      rexp = rexp / jbot;
      rexp = pow ( ( double ) 10.0E+00, ( double ) rexp );
    }

  }

  r = isgn * rexp * rtop / rbot;

  return r;
}
//****************************************************************************80

bool s_to_r4vec ( char *s, int n, float rvec[] )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_R4VEC reads an R4VEC from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 February 2001
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, character ( len = * ) S, the string to be read.
//
//    Input, integer N, the number of values expected.
//
//    Output, real RVEC[N], the values read from the string.
//
//    Output, bool S_TO_R4VEC, is true if an error occurred.
//
{
  bool error;
  int i;
  int lchar;

  error = false;

  for ( i = 0; i < n;++i )
  {
    rvec[i] = s_to_r4 ( s, &lchar, &error );

    if ( error )
    {
      return error;
    }

    s = s + lchar;

  }

  return error;
}
//****************************************************************************80

short int short_int_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    SHORT_INT_READ reads a short int from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  unsigned char  c1;
  unsigned char  c2;
  short int      ival;

  c1 = fgetc ( filein );
  c2 = fgetc ( filein );

  ival = c1 | ( c2 << 8 );

  return ival;
}
//****************************************************************************80

int short_int_write ( FILE *fileout, short int short_int_val )

//****************************************************************************80
//
//  Purpose:
//
//    SHORT_INT_WRITE writes a short int to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    short int yint;
    char ychar[2];
  } y;

  y.yint = short_int_val;

  if ( byte_swap )
  {
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[0], fileout );
  }
  else
  {
    fputc ( y.ychar[0], fileout );
    fputc ( y.ychar[1], fileout );
  }

  return 2;
}


void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn )

//****************************************************************************80
//
//  Purpose:
//
//    SORT_HEAP_EXTERNAL externally sorts a list of items into linear order.
//
//  Discussion:
//
//    The actual list is not passed to the routine.  Hence it may
//    consist of ints, reals, numbers, names, etc.  The user,
//    after each return from the routine, will be asked to compare or
//    interchange two items.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 May 1999
//
//  Author:
//
//    Original FORTRAN77 version by Albert Nijenhuis, Herbert Wilf.
//    C++ version by John Burkardt.
//
//  Parameters:
//
//    Input, int N, the length of the input list.
//
//    Input/output, int *INDX.
//
//    The user must set INDX to 0 before the first call.
//
//    On return,
//
//      if INDX is greater than 0, the user must interchange
//      items I and J and recall the routine.
//
//      If INDX is less than 0, the user is to compare items I
//      and J and return in ISGN a negative value if I is to
//      precede J, and a positive value otherwise.
//
//      If INDX is 0, the sorting is done.
//
//    Output, int *I, *J.  On return with INDX positive,
//    elements I and J of the user's list should be
//    interchanged.  On return with INDX negative, elements I
//    and J are to be compared by the user.
//
//    Input, int ISGN. On return with INDX negative, the
//    user should compare elements I and J of the list.  If
//    item I is to precede item J, set ISGN negative,
//    otherwise set ISGN positive.
//
{
  static int k = 0;
  static int k1 = 0;
  static int n1 = 0;

  if ( *indx == 0 )
  {

    n1 = n;
    k = n / 2;
    k1 = k;

  }
  else if ( *indx < 0 )
  {

    if ( *indx == -2 )
    {

      if ( isgn < 0 )
      {
        *i = *i + 1;
      }

      *j = k1 - 1;
      k1 = *i + 1;
      *indx = - 1;
      return;

    }

    if ( 0 < isgn )
    {
      *indx = 2;
      return;
    }

    if ( k <= 1 )
    {

      if ( n1 == 1 )
      {
        *indx = 0;
      }
      else
      {
        *i = n1 - 1;
        n1 = n1 - 1;
        *j = 0;
        *indx = 1;
      }

      return;

    }

    k = k - 1;
    k1 = k;

  }
  else if ( *indx == 1 )
  {
    k1 = k;
  }

  for ( ;; )
  {

    *i = 2 * k1 - 1;

    if ( *i == n1 - 1 )
    {
      *j = k1 - 1;
      k1 = *i + 1;
      *indx = - 1;
      return;
    }
    else if ( *i <= n1 - 1 )
    {
      *j = *i + 1;
      *indx = - 2;
      return;
    }

    if ( k <= 1 )
    {
      break;
    }

    k = k - 1;
    k1 = k;
  }

  if ( n1 == 1 )
  {
    *indx = 0;
  }
  else
  {
    *i = n1 - 1;
    n1 = n1 - 1;
    *j = 0;
    *indx = 1;
  }

  return;
}
//****************************************************************************80

int stla_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    STLA_READ reads an ASCII STL (stereolithography) file.
//
//  Example:
//
//    solid MYSOLID
//      facet normal 0.4 0.4 0.2
//        outerloop
//          vertex  1.0 2.1 3.2
//          vertex  2.1 3.7 4.5
//          vertex  3.1 4.5 6.7
//        endloop
//      endfacet
//      ...
//      facet normal 0.2 0.2 0.4
//        outerloop
//          vertex  2.0 2.3 3.4
//          vertex  3.1 3.2 6.5
//          vertex  4.1 5.5 9.0
//        endloop
//      endfacet
//    endsolid MYSOLID
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  int   count;
  int   i;
  int   icor3;
  int   ivert;
  char *next;
  float r1;
  float r2;
  float r3;
  float r4;
  float temp[3];
  char  token[LINE_MAX_LEN];
  int   width;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' || *next == '#' || *next == '!' || *next == '$' )
    {
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  FACET
//
    if ( s_eqi ( token, "facet" ) )
    {
//
//  Get the XYZ coordinates of the normal vector to the face.
//
      sscanf ( next, "%*s %e %e %e", &r1, &r2, &r3 );

      if ( kFaceCount < FACE_MAX )
      {
        kFaceNormal[0][kFaceCount] = r1;
        kFaceNormal[1][kFaceCount] = r2;
        kFaceNormal[2][kFaceCount] = r3;
      }

      fgets ( input, LINE_MAX_LEN, filein );
      text_num = text_num + 1;

      ivert = 0;

      for ( ;; )
      {
        fgets ( input, LINE_MAX_LEN, filein );
        text_num = text_num + 1;

        count = sscanf ( input, "%*s %e %e %e", &r1, &r2, &r3 );

        if ( count != 3 )
        {
          break;
        }

        temp[0] = r1;
        temp[1] = r2;
        temp[2] = r3;

        if ( kVertexCount < COR3_MAX )
        {
          icor3 = rcol_find ( cor3, 3, kVertexCount, temp );
        }
        else
        {
          icor3 = -1;
        }

        if ( icor3 == -1 )
        {
          icor3 = kVertexCount;

          if ( kVertexCount < COR3_MAX )
          {
            for ( i = 0; i < 3;++i )
            {
              cor3[i][kVertexCount] = temp[i];
            }
          }
          kVertexCount = kVertexCount + 1;
        }
        else
        {
          dup_num = dup_num + 1;
        }

        if ( ivert < ORDER_MAX && kFaceCount < FACE_MAX )
        {
          face[ivert][kFaceCount] = icor3;
          kFaceVertexMaterial[ivert][kFaceCount] = 0;
          for ( i = 0; i < 3;++i )
          {
            kFaceVertexNormal[i][ivert][kFaceCount] = kFaceNormal[i][kFaceCount];
          }
        }

        ivert = ivert + 1;
      }

      fgets ( input, LINE_MAX_LEN, filein );
      text_num = text_num + 1;

      if ( kFaceCount < FACE_MAX )
      {
        kFaceOrder[kFaceCount] = ivert;
      }

      kFaceCount = kFaceCount + 1;

    }
//
//  COLOR
//

    else if ( s_eqi ( token, "color" ) )
    {
      sscanf ( next, "%*s %f %f %f %f", &r1, &r2, &r3, &r4 );
    }
//
// SOLID
//
    else if ( s_eqi ( token, "solid" ) )
    {
      object_num = object_num + 1;
    }
//
// ENDSOLID
//
    else if ( s_eqi ( token, "endsolid" ) )
    {
    }
//
//  Unexpected or unrecognized.
//
    else
    {
      cout << "\n";
      cout << "STLA_READ - Fatal error!\n";
      cout << "  Unrecognized first word on line.\n";
      return 1;
    }

  }
  return 0;
}
//****************************************************************************80

int stla_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    STLA_WRITE writes an ASCII STL (stereolithography) file.
//
//  Discussion:
//
//    The polygons in an STL file should only be triangular.  This routine
//    will try to automatically decompose higher-order polygonal faces into
//    suitable triangles, without actually modifying the internal graphics
//    data.
//
//  Example:
//
//    solid MYSOLID
//      facet normal 0.4 0.4 0.2
//        outerloop
//          vertex  1.0 2.1 3.2
//          vertex  2.1 3.7 4.5
//          vertex  3.1 4.5 6.7
//        endloop
//      endfacet
//      ...
//      facet normal 0.2 0.2 0.4
//        outerloop
//          vertex  2.0 2.3 3.4
//          vertex  3.1 3.2 6.5
//          vertex  4.1 5.5 9.0
//        endloop
//      endfacet
//    endsolid
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 September 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  int icor3;
  int iface;
  int jvert;
  int kFaceCount2;
  int text_num;
//
//  Initialize.
//
  text_num = 0;
  kFaceCount2 = 0;

  fprintf ( fileout, "solid MYSOLID created by IVCON, original data in %s\n",
    filein_name );

  text_num = text_num + 1;

  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( jvert = 2; jvert < kFaceOrder[iface]; jvert++ )
    {
      kFaceCount2 = kFaceCount2 + 1;

      fprintf ( fileout, "  facet normal %f %f %f\n",
        kFaceNormal[0][iface], kFaceNormal[1][iface], kFaceNormal[2][iface] );

      fprintf ( fileout, "    outer loop\n" );

      icor3 = face[0][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      icor3 = face[jvert-1][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      icor3 = face[jvert][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      fprintf ( fileout, "    endloop\n" );
      fprintf ( fileout, "  endfacet\n" );
      text_num = text_num + 7;
    }
  }

  fprintf ( fileout, "endsolid MYSOLID\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "STLA_WRITE - Wrote " << text_num << " text lines.\n";

  if ( kFaceCount != kFaceCount2 )
  {
    cout << "  Number of faces in original data was " << kFaceCount << ".\n";
    cout << "  Number of triangular faces in decomposed data is "
         << kFaceCount2 << ".\n";
  }

  return 0;
}
//****************************************************************************80

int stlb_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    STLB_READ reads a binary STL (stereolithography) file.
//
//  Example:
//
//    80 byte string = header containing nothing in particular
//
//    4 byte int = number of faces
//
//    For each face:
//
//      3 4-byte floats = components of normal vector to face;
//      3 4-byte floats = coordinates of first node;
//      3 4-byte floats = coordinates of second node;
//      3 4-byte floats = coordinates of third and final node;
//        2-byte int = attribute, whose value is 0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  short int attribute = 0;
  char c;
  float cvec[3];
  int icor3;
  int i;
  int iface;
  int ivert;
//
//  80 byte Header.
//
  for ( i = 0; i < 80;++i )
  {
    c = ch_read ( filein );
    if ( debug )
    {
      cout << c << "\n";
    }
    bytes_num = bytes_num + 1;
 }
//
//  Number of faces.
//
  kFaceCount = long_int_read ( filein );
  bytes_num = bytes_num + 4;
//
//  For each (triangular) face,
//    components of normal vector,
//    coordinates of three vertices,
//    2 byte "attribute".
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    kFaceOrder[iface] = 3;
    kFaceMaterial[iface] = 0;

    for ( i = 0; i < 3;++i )
    {
      kFaceNormal[i][iface] = float_read ( filein );
      bytes_num = bytes_num + 4;
    }

    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      for ( i = 0; i < 3;++i )
      {
        cvec[i] = float_read ( filein );
        bytes_num = bytes_num + 4;
      }

      if ( kVertexCount < COR3_MAX )
      {
        icor3 = rcol_find ( cor3, 3, kVertexCount, cvec );
      }
      else
      {
        icor3 = -1;
      }

      if ( icor3 == -1 )
      {
        icor3 = kVertexCount;
        if ( kVertexCount < COR3_MAX )
        {
          cor3[0][kVertexCount] = cvec[0];
          cor3[1][kVertexCount] = cvec[1];
          cor3[2][kVertexCount] = cvec[2];
        }
        kVertexCount = kVertexCount + 1;
      }
      else
      {
        dup_num = dup_num + 1;
      }

      face[ivert][iface] = icor3;

    }
    attribute = short_int_read ( filein );
    if ( debug )
    {
      cout << "ATTRIBUTE = " << attribute << "\n";
    }
    bytes_num = bytes_num + 2;
  }

  return 0;
}
//****************************************************************************80

int stlb_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    STLB_WRITE writes a binary STL (stereolithography) file.
//
//  Discussion:
//
//    The polygons in an STL file should only be triangular.  This routine
//    will try to automatically decompose higher-order polygonal faces into
//    suitable triangles, without actually modifying the internal graphics
//    data.
//
//  Example:
//
//    80 byte string = header containing nothing in particular
//
//    4 byte int = number of faces
//
//    For each face:
//
//      3 4-byte floats = components of normal vector to face;
//      3 4-byte floats = coordinates of first node;
//      3 4-byte floats = coordinates of second node;
//      3 4-byte floats = coordinates of third and final node;
//        2-byte int = attribute, whose value is 0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  short int attribute = 0;
  char c;
  int i;
  int icor3;
  int iface;
  int jvert;
  int kFaceCount2;
//
//  Write the 80 byte header.
//
  for ( i = 0; i < 80;++i )
  {
    c = ' ';
    bytes_num = bytes_num + ch_write ( fileout, c );
  }
//
//  Number of faces.
//
  kFaceCount2 = 0;
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    kFaceCount2 = kFaceCount2 + kFaceOrder[iface] - 2;
  }

  bytes_num = bytes_num + long_int_write ( fileout, kFaceCount2 );
//
//  For each (triangular) face,
//    components of normal vector,
//    coordinates of three vertices,
//    2 byte "attribute".
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( jvert = 2; jvert < kFaceOrder[iface]; jvert++ )
    {
      for ( i = 0; i < 3;++i )
      {
        bytes_num = bytes_num + float_write ( fileout, kFaceNormal[i][iface] );
      }

      icor3 = face[0][iface];
      for ( i = 0; i < 3;++i )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      icor3 = face[jvert-1][iface];
      for ( i = 0; i < 3;++i )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      icor3 = face[jvert][iface];
      for ( i = 0; i < 3;++i )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      bytes_num = bytes_num + short_int_write ( fileout, attribute );

    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "STLB_WRITE - Wrote " << bytes_num << " bytes.\n";

  if ( kFaceCount != kFaceCount2 )
  {
    cout << "  Number of faces in original data was " << kFaceCount << ".\n";
    cout << "  Number of triangular faces in decomposed data is "
         << kFaceCount2 << ".\n";
  }

  return 0;
}
//****************************************************************************80

//****************************************************************************80

void tmat_init ( float a[4][4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_INIT initializes the geometric transformation matrix.
//
//  Discussion:
//
//    The geometric transformation matrix can be thought of as a 4 by 4
//    matrix "A" having components:
//
//      r11 r12 r13 t1
//      r21 r22 r23 t2
//      r31 r32 r33 t3
//        0   0   0  1
//
//    This matrix encodes the rotations, scalings and translations that
//    are applied to graphical objects.
//
//    A point P = (x,y,z) is rewritten in "homogeneous coordinates" as
//    PH = (x,y,z,1).  Then to apply the transformations encoded in A to
//    the point P, we simply compute A * PH.
//
//    Individual transformations, such as a scaling, can be represented
//    by simple versions of the transformation matrix.  If the matrix
//    A represents the current set of transformations, and we wish to
//    apply a new transformation B, { the original points are
//    transformed twice:  B * ( A * PH ).  The new transformation B can
//    be combined with the original one A, to give a single matrix C that
//    encodes both transformations: C = B * A.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
{
  int i;
  int j;

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      if ( i == j )
      {
        a[i][j] = 1.0;
      }
      else
      {
        a[i][j] = 0.0;
      }
    }
  }
  return;
}
//****************************************************************************80

void tmat_mxm ( float a[4][4], float b[4][4], float c[4][4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXM multiplies two geometric transformation matrices.
//
//  Discussion:
//
//    The product is accumulated in a temporary array, and { assigned
//    to the result.  Therefore, it is legal for any two, or all three,
//    of the arguments to share memory.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the first geometric transformation matrix.
//
//    Input, float B[4][4], the second geometric transformation matrix.
//
//    Output, float C[4][4], the product A * B.
//
{
  float d[4][4];
  int i;
  int j;
  int k;

  for ( i = 0; i < 4;++i )
  {
    for ( k = 0; k < 4;++k )
    {
      d[i][k] = 0.0;
      for ( j = 0; j < 4; j++ )
      {
        d[i][k] = d[i][k] + a[i][j] * b[j][k];
      }
    }
  }

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      c[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_mxp ( float a[4][4], float x[4], float y[4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXP multiplies a geometric transformation matrix times a point.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[4], the point to be multiplied.  The fourth component
//    of X is implicitly assigned the value of 1.
//
//    Output, float Y[4], the result of A*X.  The product is accumulated in
//    a temporary vector, and { assigned to the result.  Therefore, it
//    is legal for X and Y to share memory.
//
{
  int i;
  int j;
  float z[4];

  for ( i = 0; i < 3;++i )
  {
    z[i] = a[i][3];
    for ( j = 0; j < 3; j++ )
    {
      z[i] = z[i] + a[i][j] * x[j];
    }
  }

  for ( i = 0; i < 3;++i )
  {
    y[i] = z[i];
  }
  return;
}
//****************************************************************************80

void tmat_mxp2 ( float a[4][4], float x[][3], float y[][3], int n )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXP2 multiplies a geometric transformation matrix times N points.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[N][3], the points to be multiplied.
//
//    Output, float Y[N][3], the transformed points.  Each product is
//    accumulated in a temporary vector, and { assigned to the
//    result.  Therefore, it is legal for X and Y to share memory.
//
//
{
  int i;
  int j;
  int k;
  float z[4];

  for ( k = 0; k < n;++k )
  {
    for ( i = 0; i < 3;++i )
    {
      z[i] = a[i][3];
      for ( j = 0; j < 3; j++ )
      {
        z[i] = z[i] + a[i][j] * x[k][j];
      }
    }

    for ( i = 0; i < 3;++i )
    {
      y[k][i] = z[i];
    }

  }
  return;
}
//****************************************************************************80

void tmat_mxv ( float a[4][4], float x[4], float y[4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXV multiplies a geometric transformation matrix times a vector.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 August 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[3], the vector to be multiplied.  The fourth component
//    of X is implicitly assigned the value of 1.
//
//    Output, float Y[3], the result of A*X.  The product is accumulated in
//    a temporary vector, and assigned to the result.  Therefore, it
//    is legal for X and Y to share memory.
//
{
  int i;
  int j;
  float z[4];

  for ( i = 0; i < 3;++i )
  {
    z[i] = 0.0;
    for ( j = 0; j < 3; j++ )
    {
      z[i] = z[i] + a[i][j] * x[j];
    }
    z[i] = z[i] + a[i][3];
  }

  for ( i = 0; i < 3;++i )
  {
    y[i] = z[i];
  }
  return;
}
//****************************************************************************80

void tmat_rot_axis ( float a[4][4], float b[4][4], float angle,
  char axis )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_ROT_AXIS applies an axis rotation to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 April 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float ANGLE, the angle, in degrees, of the rotation.
//
//    Input, character AXIS, is 'X', 'Y' or 'Z', specifying the coordinate
//    axis about which the rotation occurs.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;
  float theta;

  theta = angle * DEG_TO_RAD;

  tmat_init ( c );

  if ( axis == 'X' || axis == 'x' )
  {
    c[1][1] =   cos ( theta );
    c[1][2] = - sin ( theta );
    c[2][1] =   sin ( theta );
    c[2][2] =   cos ( theta );
  }
  else if ( axis == 'Y' || axis == 'y' )
  {
    c[0][0] =   cos ( theta );
    c[0][2] =   sin ( theta );
    c[2][0] = - sin ( theta );
    c[2][2] =   cos ( theta );
  }
  else if ( axis == 'Z' || axis == 'z' )
  {
    c[0][0] =   cos ( theta );
    c[0][1] = - sin ( theta );
    c[1][0] =   sin ( theta );
    c[1][1] =   cos ( theta );
  }
  else
  {
    cout << "\n";
    cout << "TMAT_ROT_AXIS - Fatal error!\n";
    cout << "  Illegal rotation axis: " << axis << ".\n";
    cout << "  Legal choices are 'X', 'Y', or 'Z'.\n";
    return;
  }

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_rot_vector ( float a[4][4], float b[4][4], float angle,
  float v1, float v2, float v3 )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_ROT_VECTOR applies a rotation about a vector to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    27 July 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float ANGLE, the angle, in degrees, of the rotation.
//
//    Input, float V1, V2, V3, the X, Y and Z coordinates of a (nonzero)
//    point defining a vector from the origin.  The rotation will occur
//    about this axis.
//
{
  float c[4][4];
  float ca;
  float d[4][4];
  int i;
  int j;
  float sa;
  float theta;

  if ( v1 * v1 + v2 * v2 + v3 * v3 == 0.0 )
  {
    return;
  }

  theta = angle * DEG_TO_RAD;

  tmat_init ( c );

  ca = cos ( theta );
  sa = sin ( theta );

  c[0][0] =                v1 * v1 + ca * ( 1.0 - v1 * v1 );
  c[0][1] = ( 1.0 - ca ) * v1 * v2 - sa * v3;
  c[0][2] = ( 1.0 - ca ) * v1 * v3 + sa * v2;

  c[1][0] = ( 1.0 - ca ) * v2 * v1 + sa * v3;
  c[1][1] =                v2 * v2 + ca * ( 1.0 - v2 * v2 );
  c[1][2] = ( 1.0 - ca ) * v2 * v3 - sa * v1;

  c[2][0] = ( 1.0 - ca ) * v3 * v1 - sa * v2;
  c[2][1] = ( 1.0 - ca ) * v3 * v2 + sa * v1;
  c[2][2] =                v3 * v3 + ca * ( 1.0 - v3 * v3 );

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_scale ( float a[4][4], float b[4][4], float sx, float sy,
  float sz )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_SCALE applies a scaling to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float SX, SY, SZ, the scalings to be applied to the X, Y and
//    Z coordinates.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;

  tmat_init ( c );

  c[0][0] = sx;
  c[1][1] = sy;
  c[2][2] = sz;

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_shear ( float a[4][4], float b[4][4], char *axis, float s )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_SHEAR applies a shear to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, character*3 AXIS, is 'XY', 'XZ', 'YX', 'YZ', 'ZX' or 'ZY',
//    specifying the shear equation:
//
//      XY:  x' = x + s * y;
//      XZ:  x' = x + s * z;
//      YX:  y' = y + s * x;
//      YZ:  y' = y + s * z;
//      ZX:  z' = z + s * x;
//      ZY:  z' = z + s * y.
//
//    Input, float S, the shear coefficient.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;

  tmat_init ( c );

  if ( strcmp ( axis, "XY" ) == 0 || strcmp ( axis, "xy" ) == 0 )
  {
    c[0][1] = s;
  }
  else if ( strcmp ( axis, "XZ" ) == 0 || strcmp ( axis, "xz" ) == 0 )
  {
    c[0][2] = s;
  }
  else if ( strcmp ( axis, "YX" ) == 0 || strcmp ( axis, "yx" ) == 0 )
  {
    c[1][0] = s;
  }
  else if ( strcmp ( axis, "YZ" ) == 0 || strcmp ( axis, "yz" ) == 0 )
  {
    c[1][2] = s;
  }
  else if ( strcmp ( axis, "ZX" ) == 0 || strcmp ( axis, "zx" ) == 0 )
  {
    c[2][0] = s;
  }
  else if ( strcmp ( axis, "ZY" ) == 0 || strcmp ( axis, "zy" ) == 0 )
  {
    c[2][1] = s;
  }
  else
  {
    cout << "\n";
    cout << "TMAT_SHEAR - Fatal error!\n";
    cout << "  Illegal shear axis: " << axis << ".\n";
    cout << "  Legal choices are XY, XZ, YX, YZ, ZX, or ZY.\n";
    return;
  }

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_trans ( float a[4][4], float b[4][4], float x, float y,
  float z )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_TRANS applies a translation to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified transformation matrix.
//    A and B may share the same memory.
//
//    Input, float X, Y, Z, the translation.  This may be thought of as the
//    point that the origin moves to under the translation.
//
{
  int i;
  int j;

  for ( i = 0; i < 4;++i )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = a[i][j];
    }
  }
  b[0][3] = b[0][3] + x;
  b[1][3] = b[1][3] + y;
  b[2][3] = b[2][3] + z;

  return;
}



void vertex_normal_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    VERTEX_NORMAL_SET recomputes the face vertex normal vectors.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int i0;
  int i1;
  int i2;
  int iface;
  int ivert;
  int jp1;
  int jp2;
  int nfix;
  float norm;
  float temp;
  float x0;
  float x1;
  float x2;
  float xc;
  float y0;
  float y1;
  float y2;
  float yc;
  float z0;
  float z1;
  float z2;
  float zc;

  if ( kFaceCount <= 0 )
  {
    return;
  }

  nfix = 0;
//
//  Consider each face.
//
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    for ( ivert = 0; ivert < kFaceOrder[iface]; ivert++ )
    {
      norm = 0.0;
      for ( i = 0; i < 3;++i )
      {
        temp = kFaceVertexNormal[i][ivert][iface];
        norm = norm + temp * temp;
      }
      norm = ( float ) sqrt ( norm );

      if ( norm == 0.0 )
      {
        nfix = nfix + 1;

        i0 = face[ivert][iface];
        x0 = cor3[0][i0];
        y0 = cor3[1][i0];
        z0 = cor3[2][i0];

        jp1 = ivert + 1;
        if ( kFaceOrder[iface] <= jp1 )
        {
          jp1 = jp1 - kFaceOrder[iface];
        }
        i1 = face[jp1][iface];
        x1 = cor3[0][i1];
        y1 = cor3[1][i1];
        z1 = cor3[2][i1];

        jp2 = ivert + 2;
        if ( kFaceOrder[iface] <= jp2 )
        {
          jp2 = jp2 - kFaceOrder[iface];
        }
        i2 = face[jp2][iface];
        x2 = cor3[0][i2];
        y2 = cor3[1][i2];
        z2 = cor3[2][i2];

        xc = ( y1 - y0 ) * ( z2 - z0 ) - ( z1 - z0 ) * ( y2 - y0 );
        yc = ( z1 - z0 ) * ( x2 - x0 ) - ( x1 - x0 ) * ( z2 - z0 );
        zc = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );

        norm = ( float ) sqrt ( xc * xc + yc * yc + zc * zc );

        if ( norm == 0.0 )
        {
          xc = ( float ) 1.0 / sqrt ( 3.0 );
          yc = ( float ) 1.0 / sqrt ( 3.0 );
          zc = ( float ) 1.0 / sqrt ( 3.0 );
        }
        else
        {
          xc = xc / norm;
          yc = yc / norm;
          zc = zc / norm;
        }

        kFaceVertexNormal[0][ivert][iface] = xc;
        kFaceVertexNormal[1][ivert][iface] = yc;
        kFaceVertexNormal[2][ivert][iface] = zc;

      }
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "VERTEX_NORMAL_SET: Recomputed " << nfix
         << " face vertex normals.\n";
  }

  return;
}
//****************************************************************************80

void vertex_to_kFaceMaterial ( )

//****************************************************************************80
//
//  Purpose:
//
//    VERTEX_TO_FACE_MATERIAL extends vertex material definitions to faces.
//
//  Discussion:
//
//    Assuming material indices are defined for all the vertices, this
//    routine assigns to each face the material associated with its
//    first vertex.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;

  ivert = 0;
  for ( iface = 0; iface < kFaceCount; iface++ )
  {
    kFaceMaterial[iface] = kFaceVertexMaterial[ivert][iface];
  }

  return;
}
//****************************************************************************80

};