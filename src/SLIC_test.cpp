#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "SLIC.h"
//Math
#include <cmath>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <vector>
#include <iomanip>
#include <stddef.h>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>




using namespace cv;

int main(int argc, char* argv[])
{
	if (argc < 2) {
        cerr << "[ERROR] no input image" << endl;
        return -1;
    }

    Mat img = imread(argv[1], 1);

    if (!img.data) {
        cerr <<  "[ERROR] Could not open or find the image" << endl ;
        return -1;
    }

	int width = img.cols;
	int height = img.rows;

	Mat Connection_Matrix(8,10,CV_8UC2);

	Mat img_seg(height,width,CV_8UC3);

	// unsigned int (32 bits) to hold a pixel in ARGB format as follows:
	// from left to right,
	// the first 8 bits are for the alpha channel (and are ignored)
	// the next 8 bits are for the red channel
	// the next 8 bits are for the green channel
	// the last 8 bits are for the blue channel
	unsigned int *pbuff = new unsigned int[width*height];

	Vec3b intensity;
	unsigned int blue, green, red;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			intensity = img.at<Vec3b>(i, j);

			blue = intensity.val[0];
			green = intensity.val[1];
			red = intensity.val[2];
			pbuff[i*width+j] = (red << 16) + (green << 8) + blue;

		}
	}
//

	//----------------------------------
	// Initialize parameters
	//----------------------------------

	int k = 80; // Desired number of superpixels.
	double m = 10; // Compactness factor. use a value ranging from 10 to 40 depending on your needs. Default is 10
	int *klabels = new int[width*height];
	int numlabels = 0;

	int ad_mat[k][k];
	int rm;

	string filename(argv[1]);
	string savepath("");

	//SEEDS and Colours

	vector<double> kls;
	vector<double> kas;
	vector<double> kbs;
	vector<double> kxs;
	vector<double> kys;

	for(int i=0; i<k;i++){
		for(int j=0; j<k;j++){
			ad_mat[i][j]=0;
		}
	}

	for(int i=0; i<k; i++){
		if((i+10)<k){
			ad_mat[i+10][i]=1;
			ad_mat[i][i+10]=1;


		}
		rm = (i+1)%10;
		if((rm!=0)){
			ad_mat[i+1][i]=1;
			ad_mat[i][i+1]=1;

		}
	}






	//----------------------------------
	// Perform SLIC on the image buffer
	//----------------------------------
	SLIC segment;
	segment.PerformSLICO_ForGivenK(pbuff, width, height, klabels, numlabels, k, m, kls, kas, kbs, kxs, kys);
	// Alternately one can also use the function PerformSLICO_ForGivenStepSize() for a desired superpixel size
	//----------------------------------




	// Save the labels to a text file

	//----------------------------------
	//segment.SaveSuperpixelLabels(klabels, width, height, filename, savepath);
	//----------------------------------
	// Draw boundaries around segments
	//----------------------------------
	//segment.DrawContoursAroundSegments(pbuff, klabels, width, height, 0xef0000);
	segment.DrawContoursAroundSegmentsTwoColors(pbuff, klabels, width, height);
	//----------------------------------
	// Save the image with segment boundaries.
	//----------------------------------



	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			uchar *color = (uchar*)(pbuff+i*width+j);

			img.at<Vec3b>(i, j)[0] = color[0];
			img.at<Vec3b>(i, j)[1] = color[1];
			img.at<Vec3b>(i, j)[2] = color[2];

		}
	}
	//Draw centroids
	/*

	for (int i =0; i < numlabels;i++){
		int col = kxs[i];
		int row = kys[i];
		img.at<Vec3b>(row,col)[0]=0;
		img.at<Vec3b>(row,col)[1]=0;
		img.at<Vec3b>(row,col)[2]=255;
	}*/


	//Fill the matrix with the centroids





	cout << "num :" << numlabels << endl;

	// Column 1 is the index of the centroid
	// Column 2 indicates if it's a visited node or not
	// column 3 indicates the group index


	vector<vector<int>> tails;

	// Column 1 is the index of the centroid
	// Column 2 indicates if it's a visited node or not
	// column 3 indicates the group index
	int conexiones[numlabels][3];
	for(int i=0; i<numlabels; i++){
		conexiones[i][0]=i;
		conexiones[i][1]=0;
		conexiones[i][2]=0;
	}

	float color_d;
	float d_l, d_a, d_b;
	int group_i=1;
	int v_son;
	//Inicializar primer superpixel con grupo 1
	//conexiones[0][2]=group;



	for(int ic=0; ic < numlabels; ic++){
		if(conexiones[ic][1]==0){
			conexiones[ic][1]=1;
			conexiones[ic][2]=group_i;
			vector<int> temp;
			//Cada vector Temp contará con todos los integrantes de cada grupo
			temp.push_back(conexiones[ic][0]);
			for(int ic2=0; ic2 < temp.size() ; ic2++){
				for(int ic3=0; ic3<=20; ic3 ++ ){
					if(ic<10){
						if((ad_mat[temp[ic2]][temp[ic2]+ic3]==1)&&(conexiones[temp[ic2]+ic3][1]==0)){
							d_l=kls[ic]-kls[temp[ic2]+ic3];
							d_a=kas[ic]-kas[temp[ic2]+ic3];
							d_b=kbs[ic]-kbs[temp[ic2]+ic3];
							color_d=sqrt(pow(d_l,2) + pow(d_a,2) + pow(d_b,2));
							if (color_d<32){
								v_son = temp[ic2]+ic3;
								temp.push_back(v_son);
								conexiones[v_son][1]=1;
								conexiones[v_son][2]=group_i;
							}
						}

					}else if((ic>=10)&&(ic<70)){
						if((ad_mat[temp[ic2]][temp[ic2]+ic3-10]==1)&&(conexiones[temp[ic2]+ic3-10][1]==0)){
							d_l=kls[ic]-kls[temp[ic2]+ic3-10];
							d_a=kas[ic]-kas[temp[ic2]+ic3-10];
							d_b=kbs[ic]-kbs[temp[ic2]+ic3-10];
							color_d=sqrt(pow(d_l,2) + pow(d_a,2) + pow(d_b,2));
							if (color_d<32){
								v_son = temp[ic2]+ic3-10;
								temp.push_back(v_son);
								conexiones[v_son][1]=1;
								conexiones[v_son][2]=group_i;
							}
						}
					}else if(ic>=70){
						if((ad_mat[temp[ic2]][temp[ic2]+1]==1)&&(conexiones[temp[ic2]+1][1]==0)){
							d_l=kls[ic]-kls[temp[ic2]+1];
							d_a=kas[ic]-kas[temp[ic2]+1];
							d_b=kbs[ic]-kbs[temp[ic2]+1];
							color_d=sqrt(pow(d_l,2) + pow(d_a,2) + pow(d_b,2));
							if (color_d<32){
								v_son = temp[ic2]+1;
								temp.push_back(v_son);
								conexiones[v_son][1]=1;
								conexiones[v_son][2]=group_i;
							}
						}
					}

				}

			}
			tails.push_back(temp);
			group_i++;
		}
	}
	//Lol

	/*
	 *
	 * *
			for(int ic2=0; ic2<numlabels; ic2++){
				if (conexiones[ic2][1]==0){
					//Aqui'comenzar a checar que sea adyacente (aún no está) y checar que sea similar en color ( ya está)
					if(ic!=ic2){
						d_l=kls[ic]-kls[ic2];
						d_a=kas[ic]-kas[ic2];
						d_b=kbs[ic]-kbs[ic2];
						color_d=sqrt(pow(d_l,2) + pow(d_a,2) + pow(d_b,2));
						//cout<<"difcol: " <<color_d<<endl;
						if (color_d<35){
							conexiones[ic2][2]=group_i;
							conexiones[ic2][1]=1;
						}
					}

				}
			}
	 *
	 *
	 *
	 * /
	 */

	/*
	for (int i=0; i<numlabels; i++){
		cout<< "sup "<< conexiones[i][0]<<"  grupo: "<< conexiones[i][2] << endl;
	}*/

	//MATRIZ DE ADYACENCIA

	//Conection_Matrix.at<Vec2b>()=

	/*

	int col_seg;
	int row_seg;
	int tsz=width*height;
	float step;

	step = sqrt(int(tsz/k));
	col_seg=round(width/step);
	row_seg=round(height/step);

	cout << " columnas: " << col_seg << "   filas: " << row_seg << endl;


	*/

	for (int i=0; i<numlabels; i++){
		cout<<"G: "<<i<<"|  ";
		for(int j=0; j<tails[i].size();j++){
			cout<<tails[i][j]<<" ";
		}
		cout<<endl;
	}



	imwrite("slic.png", img);


	//----------------------------------
	// Clean up
	//----------------------------------
	if (pbuff) delete [] pbuff;
	if (klabels) delete [] klabels;

	return 0;
}
