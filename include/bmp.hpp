#ifndef BMP_H
#define BMP_H

#include "Vector3f.h"
#include <string>

extern const double eps;

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

struct BITMAPFILEHEADER {
	dword bfSize;
	word bfReserved1;
	word bfReserved2;
	dword bfOffBits;
};

struct BITMAPINFOHEADER {
	dword biSize;
	dword biWidth;
	dword biHeight;
	word biPlanes;
	word biBitCount;
	dword biCompression;
	dword biSizeImage;
	dword biXPelsPerMeter;
	dword biYPelsPerMeter;
	dword biClrUsed;
	dword biClrImportant;
};

struct RGBQUAD {
	byte rgbBlue;
	byte rgbGreen;
	byte rgbRed;
	byte rgbReserved;
};

struct IMAGEDATA {
	byte red;
	byte green;
	byte blue;
	Vector3f GetColor() {
		return Vector3f( red , green , blue ) / 255;
	}
};

class Bmp {
	BITMAPFILEHEADER strHead;
	BITMAPINFOHEADER strInfo;
	bool ima_created;
	IMAGEDATA** ima;
    double stretchH, stretchW;

	void Release(){
        if (ima == nullptr) return;
        for ( int i = 0 ; i < strInfo.biHeight + 2; i++ )
            delete[] ima[i];
        delete[] ima;
    }
	
public:
	Bmp( int H = 0 , int W = 0 ){
        Initialize(H, W);
        stretchH = stretchW = 1.0;
    }
	~Bmp(){
        Release();
    }

	int GetH() { return strInfo.biHeight; }
	int GetW() { return strInfo.biWidth; }

    void SetStretch(Vector2f v){
        stretchH = v.x();
        stretchW = v.y();
    }

	Vector3f GetColor( int i , int j ) {
        return Vector3f( ima[i][j].red , ima[i][j].green , ima[i][j].blue ) / 255; 
    }
	void SetColor( int i , int j , Vector3f col){
        ima[i][j].red = ( int ) ( col.x() * 255 );
        ima[i][j].green = ( int ) ( col.y() * 255 );
        ima[i][j].blue = ( int ) ( col.z() * 255 );
    }

	void Initialize( int H , int W ){
        strHead.bfReserved1 = 0;
        strHead.bfReserved2 = 0;
        strHead.bfOffBits = 54;

        strInfo.biSize = 40;
        strInfo.biPlanes = 1;
        strInfo.biHeight = H;
        strInfo.biWidth = W;
        strInfo.biBitCount = 24;
        strInfo.biCompression = 0;
        strInfo.biSizeImage = H * W * 3;
        strInfo.biXPelsPerMeter = 0;
        strInfo.biYPelsPerMeter = 0;
        strInfo.biClrUsed = 0;
        strInfo.biClrImportant = 0;

        strHead.bfSize = strInfo.biSizeImage + strInfo.biBitCount;
        
        ima = new IMAGEDATA*[H + 2];
        for ( int i = 0 ; i < H + 2 ; i++ )
            ima[i] = new IMAGEDATA[W + 2];
    }
	void Input( std::string file ){
        Release();

        FILE *fpi = fopen( file.c_str() , "r" );
        word bfType;
        fread( &bfType , 1 , sizeof( word ) , fpi );
        fread( &strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpi );
        fread( &strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpi );
        
        RGBQUAD Pla;
        for ( int i = 0 ; i < ( int ) strInfo.biClrUsed ; i++ ) {
            fread( ( char * ) & ( Pla.rgbBlue ) , 1 , sizeof( byte ) , fpi );
            fread( ( char * ) & ( Pla.rgbGreen ) , 1 , sizeof( byte ) , fpi );
            fread( ( char * ) & ( Pla.rgbRed ) , 1 , sizeof( byte ) , fpi );
        }
        
        Initialize( strInfo.biHeight , strInfo.biWidth );
        
        int pitch = strInfo.biWidth % 4;
        for(int i = 0 ; i < strInfo.biHeight ; i++ ) {
            for(int j = 0 ; j < strInfo.biWidth ; j++ ) {
                fread( &ima[i][j].blue , 1 , sizeof( byte ) , fpi );
                fread( &ima[i][j].green , 1 , sizeof( byte ) , fpi );
                fread( &ima[i][j].red , 1 , sizeof( byte ) , fpi );
            }
            byte buffer = 0;
            for (int j = 0; j < pitch; j++)
                fread( &buffer , 1 , sizeof( byte ) , fpi );
        }

        fclose( fpi );
    }
	void Output( std::string file ){
        FILE *fpw = fopen( file.c_str() , "wb" );

        word bfType = 0x4d42;
        fwrite( &bfType , 1 , sizeof( word ) , fpw );
        fwrite( &strHead , 1 , sizeof( BITMAPFILEHEADER ) , fpw );
        fwrite( &strInfo , 1 , sizeof( BITMAPINFOHEADER ) , fpw );
        
        int pitch = strInfo.biWidth % 4;
        for ( int i = 0 ; i < strInfo.biHeight ; i++ ) {
            for ( int j = 0 ; j < strInfo.biWidth ; j++ ) {
                fwrite( &ima[i][j].blue , 1 , sizeof( byte ) , fpw );
                fwrite( &ima[i][j].green , 1 , sizeof( byte ) , fpw );
                fwrite( &ima[i][j].red , 1 , sizeof( byte ) , fpw );
            }
            byte buffer = 0;
            for (int j = 0; j < pitch; j++)
                fwrite( &buffer , 1 , sizeof( byte ) , fpw );
        }
        
        fclose( fpw );
    }
	Vector3f GetSmoothColor( double u , double v ){
        if (!(u >= -1e5 && u <= 1e5)) u = 0;
        if (!(v >= -1e5 && v <= 1e5)) v = 0;
        u /= stretchW;
        v /= stretchH;
        u = -u + 1e5;
        v = -v + 1e5;
        double U = ( u + eps - floor( u + eps ) ) * strInfo.biHeight;
        double V = ( v + eps - floor( v + eps ) ) * strInfo.biWidth;
        int U1 = ( int ) floor( U + eps ) , U2 = U1 + 1;
        int V1 = ( int ) floor( V + eps ) , V2 = V1 + 1;
        double rat_U = U2 - U;
        double rat_V = V2 - V;
        if ( U1 < 0 ) U1 = strInfo.biHeight - 1; 
        if ( U1 >= strInfo.biHeight ) U1 = 0;
        if ( U2 < 0 ) U2 = strInfo.biHeight - 1; 
        if ( U2 >= strInfo.biHeight ) U2 = 0;
        if ( V1 < 0 ) V1 = strInfo.biWidth - 1; 
        if ( V1 >= strInfo.biWidth ) V1 = 0;
        if ( V2 < 0 ) V2 = strInfo.biWidth - 1; 
        if ( V2 >= strInfo.biWidth ) V2 = 0;
        Vector3f ret;
        ret = ret + ima[U1][V1].GetColor() * rat_U * rat_V;
        ret = ret + ima[U1][V2].GetColor() * rat_U * ( 1 - rat_V );
        ret = ret + ima[U2][V1].GetColor() * ( 1 - rat_U ) * rat_V;
        ret = ret + ima[U2][V2].GetColor() * ( 1 - rat_U ) * ( 1 - rat_V );
        return ret;
    }
};

#endif
