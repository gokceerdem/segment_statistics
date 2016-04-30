	// TestDbProject.cpp : Defines the entry point for the console application.
	//
	#include <stdio.h>
	//#include "sqlite3.h"
	#include "stdafx.h"
	#include <cstdlib>
	#include "HEADER.h"

	using namespace std;
	using namespace cv;

	int labelColors[10000][2]={0};
	int myClrs[TOTAL_COLOR_Nr+1][3];
	int dummy[10000][3];

	vector<Point> contourArray;
	vector<vector<Point>> imageContours;
	vector<int> segmentClrs;

	list<hsvc> col_hash_map;
	coor c;

	list<SegmentFeatures> featureVector;
	SegmentFeatures mF;
	list<SegmentMoments> momentVector;
	SegmentMoments SegMom;

	ImData myImData;
	time_t tstart, tend, mstart, mend;


	stringstream imgstream,mystream,seq_stream,imgstr,segmentstream,cropstream;

	//Static array
	int label[pyrHeight][pyrWidth]={0};
	int I[pyrHeight][pyrWidth]={0};
	int Q[pyrHeight][pyrWidth+1]={0};
	
	int EQ[MAX_EXP_NrOf_LABELS]={0};
	int relationLUT[TOTAL_COLOR_Nr+1][TOTAL_COLOR_Nr+1]={0};
	int labelNr;

	int centerSum[2][1] = {0};

		double sqrt6 (double y) 
	{
		double x, z, tempf;
		unsigned long *tfptr = ((unsigned long *)&tempf) + 1;
		tempf = y;
		*tfptr = (0xbfcdd90a - *tfptr)>>1; 
		x =  tempf;
		z =  y*0.5;                       
		x = (1.5*x) - (x*x)*(x*z);    //The more you make replicates of this statement 
									//the higher the accuracy, here only 2 replicates are used  
		x = (1.5*x) - (x*x)*(x*z);       
		return x*y; 
		}  

		static int callback(void *NotUsed, int argc, char **argv, char **azColName){
		int i;
		for(i=0; i<argc; i++){
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}

		void saveMoments(list<SegmentMoments> momentVector,ImData &mid) {
			char mfile[250];
		
					sprintf(mfile,"C:\\Users\\Gokce\\Documents\\ObjectFeatureExtraction\\Features\\Moments-%s.txt",cstr);
				fstream file_op(mfile,ios::out);

				for (int j=0;j<mid.connComp.size();j++) {
			
						list<SegmentMoments>::iterator itm= momentVector.begin();
						std::advance(itm,j);
				
						file_op<<"Segment Nr. "<<j<<"	"<<
							"m00 =" << itm-> m00<<"	"<<
							"m20 = "<< itm->m20 <<"	"<<
							"m02 = "<< itm->m02 <<"	"<<
							"m11 = "<< itm->m11 <<"	"<<endl;
					}
		
				file_op.close();
		}

		void LUT(int relationLUT[TOTAL_COLOR_Nr+1][TOTAL_COLOR_Nr+1]){	
	

				for( int t = 0; t <TOTAL_COLOR_Nr+1; t++){
					relationLUT[t][t] = 1;}
				relationLUT[24][25]=1;//mx white & white
				relationLUT[25][24]=1;//white & mx white 
				//relationLUT[23][22]=1;//black & mx black
				//relationLUT[22][23]=1;
				relationLUT[23][18]=1;//black & dark gray
				relationLUT[18][23]=1;
				relationLUT[18][22]=1;//dark gray & mx black
				relationLUT[22][18]=1;
				//relationLUT[24][19]=1;//mx white & light gray 
				relationLUT[8][9]=1; // light blue & dark blue  
				relationLUT[21][3]=1;//yellow & brown
				relationLUT[3][20]=1;
		
		}

		void myColors(){
			int twentyfivecolors[TOTAL_COLOR_Nr+1][3]={
				{30,30,30},
				{255,0,0},//red				1
				{255,128,0},//orange		2
				{255,255,0},//yellow		3
				{219,253,0},//yellowgreen	4
				{102,255,102},//lightgreen	5
				{0,51,0},//darkgreen		6
				{0,255,255},//cyan			7
				{153,204,255},//lightblue	8
				{0,0,204},//darkblue		9
				{153,51,255},//violet		10
				{76,0,153},//purple			11
				{149,0,104},//pinkpurple	12
				{255,122,102},//pink1(yavruagzi) 13
				{199,40,63},//pink2(cherry)	14
				{255,204,255},//pink3(lightpink) 15
				{255,0,255},//magenta		16
				{255,70,255},//pink			17
				{64,64,64},//darkgray		18
				{192,192,192},//lightgray	19
				{58,25,25},//brown1(taba)	20
				{45,21,0},//brown			21
				{47,42,42},//mixedblack		22
				{0,0,0},//black				23
				{240,240,240},//mixedwhite	24
				{255,255,255},//white		25
			};

			for(int i = 0; i<TOTAL_COLOR_Nr+1; i++){
			for(int j = 0; j<3; j++){
		
				myClrs[i][j] = twentyfivecolors[i][j];

			}}
	
		}

		void colorImg(vector<Mat> channels, Mat ch0, Mat ch1, Mat ch2, int I[pyrHeight][pyrWidth],int imgnr){
			channels.clear();
	
				 myColors();
		
		 for ( int colorindex = 1; colorindex<TOTAL_COLOR_Nr+1; colorindex++)
		 {
					 for(int i = 0; i < myImData.h; i++) {
				for(int j = 0; j < myImData.w; j++) {

					int colorval= I[i][j];	

					if (colorval == colorindex){
					ch2.at<uchar>(i,j)= myClrs[colorindex][0];
					ch1.at<uchar>(i,j)= myClrs[colorindex][1];
					ch0.at<uchar>(i,j)= myClrs[colorindex][2];}

						}
							}
		 }
			Mat fin_img2;
			channels.push_back(ch0); //b
			channels.push_back(ch1); //g
			channels.push_back(ch2); //r
			merge(channels, fin_img2);

			//imgstr << "colored_by_respected_colors_"<< imagename << quant << type;
			imgstr << imagename1<<"_"<<imgnr << quant << type;
			string myfilename = imgstr.str();
			imgstr.str("");
			imwrite(myfilename, fin_img2);
		
			ch0.release();
			ch1.release();
			ch2.release();
			fin_img2.release();
			channels.clear();
		}	

		void Labeling(int &labelNr,int label[pyrHeight][pyrWidth],int I[pyrHeight][pyrWidth],int Q[pyrHeight][pyrWidth+1],int EQ[MAX_EXP_NrOf_LABELS],ImData &myImData){
		// Label (0,0) start point
				int L = 0;	
				labelNr =0;
				++L; ++labelNr;
				EQ[L]=(L);
				label[0][0]=L; Q[0][1]=L;

		// Label first row 	
				for(int x=1; x<myImData.w; x++){

					int y=0;
					int n1x=x-1; 

				if(I[y][n1x]==I[y][x]){
					label[y][x]=label[y][n1x];
					Q[y][x+1]=label[y][x];}
				if(I[y][n1x]!=I[y][x]){
						++L; ++labelNr;
						EQ[L]=(L);
						label[y][x]= L;
						Q[y][x+1]=L;}
				}
			
		// Label first column starting from second row	

						for(int y=1; y<myImData.h; y++){
							if(I[y][0]==I[y-1][0]){
							label[y][0]=label[y-1][0];
							Q[y][1]=label[y][0];}

							if(I[y][0]!=I[y-1][0]){
								++L; ++labelNr;
								EQ[L]=(L);
							label[y][0]=L;
							Q[y][1]=label[y][0];}
						}

		//Label the rest of the img
				
					for(int x=1; x<myImData.w; x++){
						for(int y=1; y<myImData.h; y++){
					
					
						int sx= x-1; int sy=y;
						int tx=x;	 int ty=y-1;

							if(I[y][x]==I[sy][sx]	&&	I[y][x]!=I[ty][tx]){
								label[y][x] = label[sy][sx];}

							if(I[y][x]!=I[sy][sx]	&&	I[y][x]==I[ty][tx]){
								label[y][x] = label[ty][tx];}

							if(I[y][x]!=I[sy][sx]	&&	I[y][x]!=I[ty][tx]){
									++L; ++labelNr;
									EQ[L]=(L);
									label[y][x] = L;}

							if(I[y][x]==I[sy][sx]	&&	I[y][x]==I[ty][tx]	&&	label[sy][sx]==label[ty][tx]){
									label[y][x] = label[ty][tx];}

							if(I[y][x]==I[sy][sx]	&&	I[y][x]==I[ty][tx]	&&	label[sy][sx]!=label[ty][tx]){
								int comp = (label[sy][sx]<label[ty][tx]); // Ls < Lt -->  1
																		  // Ls > Lt -->  0
								int L1,L2; //L1<L2
								comp ? L1 = label[sy][sx] : L1 = label[ty][tx];
								comp ? L2 = label[ty][tx] : L2 = label[sy][sx];
						
								label[y][x]=L1; 
								EQ[L2]=L1;}
							Q[y][x+1]=label[y][x];

							}}
					for(int i=0; i<myImData.h; i++){
							Q[i][0]=label[i][1];
					
							}

		}

		void LabelEqualization(int EQ[MAX_EXP_NrOf_LABELS],int label[pyrHeight][pyrWidth],ImData &myImData, int labelColors[10000][2]){
		//Equalization of labels
						for(int k =1; k<MAX_EXP_NrOf_LABELS; k++){

						if (EQ[k]==0){break;}
				
						if(EQ[k]!=k){
							EQ[EQ[k]] == EQ[k] ? 1 :  EQ[k]=EQ[EQ[EQ[k]]];
						}

						for(int i = 0; i < myImData.h; i++) {
								for(int j = 0; j < myImData.w; j++) {
					if(label[i][j] == k)
						label[i][j] = EQ[k];
					Q[i][j+1]=label[i][j];

					//labelColors[label[i][j]][0] = I[i][j];
										}
					}
					}
		}

		void createHash(String dy){
	
			string line;
			ifstream myfile (dy);
	
				while ( getline (myfile,line) )
				{
					stringstream   linestream(line);
					string         data;
					int hl,hh,sl,sh,vl,vh,color_name;// HueLow, HueHigh, SaturationLow, SaturationHigh, ValueLow, ValueHigh
			
					getline(linestream, data, '\t');

					linestream >> hh >> sl >> sh >> vl >> vh >> color_name;
					hsvc new_hsvc;
					new_hsvc.hlow = atoi(data.c_str());
					new_hsvc.hhigh = hh;
					new_hsvc.slow = sl;
					new_hsvc.shigh = sh;
					new_hsvc.vlow = vl;
					new_hsvc.vhigh = vh;
					new_hsvc.col_name = color_name;

					col_hash_map.push_back(new_hsvc);
		
				}
				myfile.close();
	
		}

		void keepcolors(String clrs){
			int cidx = 0;
			string line;
			ifstream file (clrs);
	
				while ( getline (file,line) )
				{
					stringstream   linestream(line);
					string        data;
					int r,g,b;
			
					getline(linestream, data, '\t');
					linestream  >> g >> b; 
	
						/* Array implementation*/
					dummy[cidx][0]= atoi(line.c_str());
					dummy[cidx][1]= g;
					dummy[cidx][2]= b;

					++cidx;
		
				}
				file.close();
	
		}

		void features(ImData &mid){	
	
	
		//	int segmentNr=0;
			for (int k = 0; k< mid.connComp.size(); k++){
		
				int cx = 0;
					int cy = 0;
	
		 				double filtSum0 = 0;
						double filtSum1 = 0;
						double filtSum2 = 0;
						double filtSum3 = 0;
						double filtSum4 = 0;
						double filtSum5 = 0;
						double filtSum6 = 0;
						double filtSum7 = 0;
						double filtSum8 = 0;
						double filtSum9 = 0;
						double filtSum10 = 0;
						double filtSum11 = 0;
						double filtSum12 = 0;
						double filtSum13 = 0;
						double filtSum14 = 0;
						double filtSum15 = 0; 
						double filtSum16 = 0;
						double filtSum17 = 0;
						double filtSum18 = 0;
						double filtSum19 = 0;
						double filtSum20 = 0;
						double filtSum21 = 0;
						double filtSum22 = 0;
						double filtSum23 = 0;
						double filtSum24 = 0;
						double filtSum25 = 0;
						double filtSum26 = 0;
						double filtSum27 = 0;
						double filtSum28 = 0;
						double filtSum29 = 0;
						double filtSum30 = 0;
						double filtSum31 = 0;
						double filtSum32 = 0;
						double filtSum33 = 0;
						double filtSum34 = 0;
						double filtSum35 = 0;
						double filtSum36 = 0;
						double filtSum37 = 0;
						double filtSum38 = 0;
						double filtSum39 = 0;
						double filtSum40 = 0;
						double filtSum41 = 0;
						double filtSum42 = 0;
						double filtSum43 = 0;
						double filtSum44 = 0;
						double filtSum45 = 0;
						double filtSum46 = 0;
						double filtSum47 = 0;
						double filtSum48 = 0;
						double filtSum49 = 0;
						double filtSum50 = 0;
						double filtSum51 = 0;
						double filtSum52 = 0;
						double filtSum53 = 0;
						double filtSum54 = 0; //hue

					 int centerxSum = 0;
					 int centerySum = 0;

					 //second order moments
					 double m20 = 0;
					 double m02 = 0;
					 double m11 = 0;
			
					double A =  mid.connComp.at(k).size(); // Area of the segment = m00
				//	SegMom.m00 = A;

						 mF.r = A / (pyrHeight* pyrWidth);
			
						 for(int s=0; s < mid.connComp.at(k).size(); s++)
						 {
						 // center of mass calculation --> mass(cx,cy)

						//centerSum = centerSum + memberpixel;
						int memberx = mid.connComp.at(k).at(s).x;
						  int membery  = mid.connComp.at(k).at(s).y;
			 
						centerxSum = centerxSum + memberx;
						centerySum = centerySum + membery;
				 			if( s == mid.connComp.at(k).size()-1)
							{		
						//cx and cy calculation
						 cx = (centerxSum) / A;
						 cy = (centerySum) / A;

							}
						 }
						 				 
		  if(cy<=UpObjTh){mF.segmentLocation=1;}
		  else if(cy>UpObjTh && cy<= MidObjTh){mF.segmentLocation = 0;}
		  else if(cy>MidObjTh){mF.segmentLocation=-1;}

					 for(int s=0; s < mid.connComp.at(k).size(); s++){
				
						 m20 = m20 + ((mid.connComp.at(k).at(s).x - cx)*(mid.connComp.at(k).at(s).x - cx));
						 m02 = m02 + ((mid.connComp.at(k).at(s).y - cy)*(mid.connComp.at(k).at(s).y - cy));
						 m11 = m11 + ((mid.connComp.at(k).at(s).x - cx)*(mid.connComp.at(k).at(s).y - cy));

	
								double dumVal0 = mid.filter.at(0).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum0	=	filtSum0	+	dumVal0;

								double dumVal1 = mid.filter.at(1).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum1	=	filtSum1	+	dumVal1;

								double dumVal2 = mid.filter.at(2).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum2	=	filtSum2	+	dumVal2;

								double dumVal3 = mid.filter.at(3).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum3	=	filtSum3	+	dumVal3;		

								double dumVal4 = mid.filter.at(4).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum4	=	filtSum4	+	dumVal4;		

								double dumVal5 = mid.filter.at(5).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum5	=	filtSum5	+	dumVal5;	
																
								double	dumVal6	=	mid.filter.at(6).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum6	=	filtSum6	+	dumVal6;

								double	dumVal7	=	mid.filter.at(7).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum7	=	filtSum7	+	dumVal7;

								double	dumVal8	=	mid.filter.at(8).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum8	=	filtSum8	+	dumVal8;

								double	dumVal9	=	mid.filter.at(9).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum9	=	filtSum9	+	dumVal9;

								double	dumVal10	=	mid.filter.at(10).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum10	=	filtSum10	+	dumVal10;

								double	dumVal11	=	mid.filter.at(11).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum11	=	filtSum11	+	dumVal11;

								double	dumVal12	=	mid.filter.at(12).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum12	=	filtSum12	+	dumVal12;

								double	dumVal13	=	mid.filter.at(13).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum13	=	filtSum13	+	dumVal13;

								double	dumVal14	=	mid.filter.at(14).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum14	=	filtSum14	+	dumVal14;
								
								double	dumVal15	=	mid.filter.at(15).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum15	=	filtSum15	+	dumVal15;
								
								double	dumVal16	=	mid.filter.at(16).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum16	=	filtSum16	+	dumVal16;
								
								double	dumVal17	=	mid.filter.at(17).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum17	=	filtSum17	+	dumVal17;
								
								double	dumVal18	=	mid.filter.at(18).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum18	=	filtSum18	+	dumVal18;
								
								double	dumVal19	=	mid.filter.at(19).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum19	=	filtSum19	+	dumVal19;
								
								double	dumVal20	=	mid.filter.at(20).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum20	=	filtSum20	+	dumVal20;
								
								double	dumVal21	=	mid.filter.at(21).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum21	=	filtSum21	+	dumVal21;
								
								double	dumVal22	=	mid.filter.at(22).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum22	=	filtSum22	+	dumVal22;
								
								double	dumVal23	=	mid.filter.at(23).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum23	=	filtSum23	+	dumVal23;
								
								double	dumVal24	=	mid.filter.at(24).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum24	=	filtSum24	+	dumVal24;
								
								double	dumVal25	=	mid.filter.at(25).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum25	=	filtSum25	+	dumVal25;
								
								double	dumVal26	=	mid.filter.at(26).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum26	=	filtSum26	+	dumVal26;
								
								double	dumVal27	=	mid.filter.at(27).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum27	=	filtSum27	+	dumVal27;
								
								double	dumVal28	=	mid.filter.at(28).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum28	=	filtSum28	+	dumVal28;
								
								double	dumVal29	=	mid.filter.at(29).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum29	=	filtSum29	+	dumVal29;
								
								double	dumVal30	=	mid.filter.at(30).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum30	=	filtSum30	+	dumVal30;
								
								double	dumVal31	=	mid.filter.at(31).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum31	=	filtSum31	+	dumVal31;
								
								double	dumVal32	=	mid.filter.at(32).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum32	=	filtSum32	+	dumVal32;
								
								double	dumVal33	=	mid.filter.at(33).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum33	=	filtSum33	+	dumVal33;
								
								double	dumVal34	=	mid.filter.at(34).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum34	=	filtSum34	+	dumVal34;
								
								double	dumVal35	=	mid.filter.at(35).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum35	=	filtSum35	+	dumVal35;

								double	dumVal36	=	mid.filter.at(36).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum36	=	filtSum36	+	dumVal36;
								
								double	dumVal37	=	mid.filter.at(37).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum37	=	filtSum37	+	dumVal37;
								
								double	dumVal38	=	mid.filter.at(38).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum38	=	filtSum38	+	dumVal38;
								
								double	dumVal39	=	mid.filter.at(39).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum39	=	filtSum39	+	dumVal39;
								
								double	dumVal40	=	mid.filter.at(40).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum40	=	filtSum40	+	dumVal40;
								
								double	dumVal41	=	mid.filter.at(41).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum41	=	filtSum41	+	dumVal41;
								
								double	dumVal42	=	mid.filter.at(42).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum42	=	filtSum42	+	dumVal42;
								
								double	dumVal43	=	mid.filter.at(43).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum43	=	filtSum43	+	dumVal43;
								
								double	dumVal44	=	mid.filter.at(44).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum44	=	filtSum44	+	dumVal44;
								
								double	dumVal45	=	mid.filter.at(45).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum45	=	filtSum45	+	dumVal45;
								
								double	dumVal46	=	mid.filter.at(46).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum46	=	filtSum46	+	dumVal46;
								
								double	dumVal47	=	mid.filter.at(47).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum47	=	filtSum47	+	dumVal47;
								
								double	dumVal48	=	mid.filter.at(48).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum48	=	filtSum48	+	dumVal48;
								
								double	dumVal49	=	mid.filter.at(49).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum49	=	filtSum49	+	dumVal49;
								
								double	dumVal50	=	mid.filter.at(50).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum50	=	filtSum50	+	dumVal50;
								
								double	dumVal51	=	mid.filter.at(51).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum51	=	filtSum51	+	dumVal51;
								
								double	dumVal52	=	mid.filter.at(52).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum52	=	filtSum52	+	dumVal52;
								
								double	dumVal53	=	mid.filter.at(53).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);
								filtSum53	=	filtSum53	+	dumVal53;
								
								double	dumVal54	=	mid.filter.at(54).at<uchar>(mid.connComp.at(k).at(s).y,mid.connComp.at(k).at(s).x);			  	
								filtSum54	=	filtSum54	+	dumVal54;

			  			if( s == mid.connComp.at(k).size()-1)
						{
							// normalized feature results
						double f0 = filtSum0 / A;
						f0 = f0 / 255; 
						mF.AverageFilterOutputs.push_back(f0);

						double f1 = filtSum1 / A;
						f1 = f1 / 255; 
						mF.AverageFilterOutputs.push_back(f1);

						double f2 = filtSum2 / A;
						f2 = f2 / 255; 
						mF.AverageFilterOutputs.push_back(f2);

						double f3 = filtSum3 / A;
						f3 = f3 / 255;
						mF.AverageFilterOutputs.push_back(f3);

						double f4 = filtSum4 / A;
						f4 = f4 / 255; 
						mF.AverageFilterOutputs.push_back(f4);

						double f5 = filtSum5 / A;
						f5 = f5 / 255; 
						mF.AverageFilterOutputs.push_back(f5);

						double	f6	=	filtSum6 / A;
						f6	=	f6	/ 255;
						mF.AverageFilterOutputs.push_back(f6);

						double	f7	=	filtSum7 / A;
						f7	=	f7	/ 255;
						mF.AverageFilterOutputs.push_back(f7);

						double	f8	=	filtSum8 / A;
						f8	=	f8	/ 255;
						mF.AverageFilterOutputs.push_back(f8);

						double	f9	=	filtSum9 / A;
						f9	=	f9	/ 255;
						mF.AverageFilterOutputs.push_back(f9);	

						double	f10	=	filtSum10 / A;
						f10	=	f10	/ 255;
						mF.AverageFilterOutputs.push_back(f10);
						
						double	f11	=	filtSum11 / A;
						f11	=	f11	/ 255;
						mF.AverageFilterOutputs.push_back(f11);

						double	f12	=	filtSum12 / A;
						f12	=	f12	/ 255;	
						mF.AverageFilterOutputs.push_back(f12);

						double	f13	=	filtSum13 / A;
						f13	=	f13	/ 255;
						mF.AverageFilterOutputs.push_back(f13);

						double	f14	=	filtSum14 / A;
						f14	=	f14	/ 255;
						mF.AverageFilterOutputs.push_back(f14);
								
						double	f15	=	filtSum15 / A;
						f15	=	f15	/ 255;
						mF.AverageFilterOutputs.push_back(f15);

						double	f16	=	filtSum16 / A;
						f16	=	f16	/ 255;
						mF.AverageFilterOutputs.push_back(f16);

						double	f17	=	filtSum17 / A;
						f17	=	f17	/ 255;	
						mF.AverageFilterOutputs.push_back(f17);	

						double	f18	=	filtSum18 / A;
						f18	=	f18	/ 255;
						mF.AverageFilterOutputs.push_back(f18);

						double	f19	=	filtSum19 / A;
						f19	=	f19	/ 255;	
						mF.AverageFilterOutputs.push_back(f19);	

						double	f20	=	filtSum20 / A;
						f20	=	f20	/ 255;
						mF.AverageFilterOutputs.push_back(f20);

						double	f21	=	filtSum21 / A;
						f21	=	f21	/ 255;
						mF.AverageFilterOutputs.push_back(f21);
								
						double	f22	=	filtSum22 / A;
						f22	=	f22	/ 255;		
						mF.AverageFilterOutputs.push_back(f22);

						double	f23	=	filtSum23 / A;
						f23	=	f23	/ 255;
						mF.AverageFilterOutputs.push_back(f23);

						double	f24	=	filtSum24 / A;
						f24	=	f24	/ 255;	
						mF.AverageFilterOutputs.push_back(f24);

						double	f25	=	filtSum25 / A;
						f25	=	f25	/ 255;
						mF.AverageFilterOutputs.push_back(f25);	

						double	f26	=	filtSum26 / A;
						f26	=	f26	/ 255;										
						mF.AverageFilterOutputs.push_back(f26);	
								
						double	f27	=	filtSum27 / A;
						f27	=	f27	/ 255;	
						mF.AverageFilterOutputs.push_back(f27);	

						double	f28	=	filtSum28 / A;
						f28	=	f28	/ 255;	
						mF.AverageFilterOutputs.push_back(f28);

						double	f29	=	filtSum29 / A;
						f29	=	f29	/ 255;
						mF.AverageFilterOutputs.push_back(f29);
								
						double	f30	=	filtSum30 / A;
						f30	=	f30	/ 255;
						mF.AverageFilterOutputs.push_back(f30);
								
						double	f31	=	filtSum31 / A;
						f31	=	f31	/ 255;	
						mF.AverageFilterOutputs.push_back(f31);

						double	f32	=	filtSum32 / A;
						f32	=	f32	/ 255;
						mF.AverageFilterOutputs.push_back(f32);

						double	f33	=	filtSum33 / A;
						f33	=	f33	/ 255;	
						mF.AverageFilterOutputs.push_back(f33);

						double	f34	=	filtSum34 / A;
						f34	=	f34	/ 255;
						mF.AverageFilterOutputs.push_back(f34);
								
						double	f35	=	filtSum35 / A;
						f35	=	f35	/ 255;
						mF.AverageFilterOutputs.push_back(f35);

						double	f36	=	filtSum36 / A;
						f36	=	f36	/ 255;
						mF.AverageFilterOutputs.push_back(f36);
								
						double	f37	=	filtSum37 / A;
						f37	=	f37	/ 255;
						mF.AverageFilterOutputs.push_back(f37);
								
						double	f38	=	filtSum38 / A;
						f38	=	f38	/ 255;
						mF.AverageFilterOutputs.push_back(f38);
								
						double	f39	=	filtSum39 / A;
						f39	=	f39	/ 255;
						mF.AverageFilterOutputs.push_back(f39);
								
						double	f40	=	filtSum40 / A;
						f40	=	f40	/ 255;
						mF.AverageFilterOutputs.push_back(f40);
								
						double	f41	=	filtSum41 / A;
						f41	=	f41	/ 255;
						mF.AverageFilterOutputs.push_back(f41);
								
						double	f42	=	filtSum42 / A;
						f42	=	f42	/ 255;
						mF.AverageFilterOutputs.push_back(f42);
								
						double	f43	=	filtSum43 / A;
						f43	=	f43	/ 255;
						mF.AverageFilterOutputs.push_back(f43);
								
						double	f44	=	filtSum44 / A;
						f44	=	f44	/ 255;
						mF.AverageFilterOutputs.push_back(f44);

						double	f45	=	filtSum45 / A;
						f45	=	f45	/ 255;
						mF.AverageFilterOutputs.push_back(f45);
								
						double	f46	=	filtSum46 / A;
						f46	=	f46	/ 255;
						mF.AverageFilterOutputs.push_back(f46);
								
						double	f47	=	filtSum47 / A;
						f47	=	f47	/ 255;
						mF.AverageFilterOutputs.push_back(f47);
								
						double	f48	=	filtSum48 / A;
						f48	=	f48	/ 255;
						mF.AverageFilterOutputs.push_back(f48);
								
						double	f49	=	filtSum49 / A;
						f49	=	f49	/ 255;
						mF.AverageFilterOutputs.push_back(f49);
								
						double	f50	=	filtSum50 / A;
						f50	=	f50	/ 255;
						mF.AverageFilterOutputs.push_back(f50);
								
						double	f51	=	filtSum51 / A;
						f51	=	f51	/ 255;
						mF.AverageFilterOutputs.push_back(f51);
								
						double	f52	=	filtSum52 / A;
						f52	=	f52	/ 255;
						mF.AverageFilterOutputs.push_back(f52);
								
						double	f53	=	filtSum53 / A;
						f53	=	f53	/ 255;						
						mF.AverageFilterOutputs.push_back(f53);


						//hue normalization is different
						double f54 = filtSum54 / A;
							f54 = f54 / 180; 
						mF.AverageFilterOutputs.push_back(f54);

						//mF.mass = Point(cx,cy);

						//calculation of major and minor axis
			
						m20	= m20/A; SegMom.m20 = m20;
						m02	= m02/A; SegMom.m02 = m02;
						m11	= m11/A; SegMom.m11 = m11;		
				
				
						double k4 = sqrt6(abs((4*(m11)*(m11))-((m20 - m02)*(m20 - m02))));
				
						double lambda1 = sqrt6(((m20 + m02)/2)+k4); //major
							lambda1 = lambda1/hypo;	// = q2 (normalized major length)
						double lambda2 = sqrt6(abs(((m20 + m02)/2)-k4)); //minor
							lambda2 = lambda2/hypo;	// =  q3 (normalized minor length)

			  
						mF.elongation.push_back(lambda1);
						mF.elongation.push_back(lambda2);		
						//eccentricity = abs((((m20 - m02)*(m20 - m02))-(4*(m11)*(m11)))/((m20 + m02)*(m20 + m02)));

						featureVector.push_back(mF);
					//	momentVector.push_back(SegMom);

						mF.AverageFilterOutputs.clear();
						mF.elongation.clear();
				
			//				segmentNr = segmentNr+1;
						}
					 }
			}
		}

		float sqrt5(const float m)
	{
		float i=0;
		float x1,x2;
		while( (i*i) <= m )
				i+=0.1f;
		x1=i;
		for(int j=0;j<10;j++)
		{
			x2=m;
			x2/=x1;
			x2+=x1;
			x2/=2;
			x1=x2;
		}
		return x2;
	}   

		void saveColorFreq(int segno, int arg_max_v ,int currentMax_v) {
		char mfile[250];
	
				sprintf(mfile,"C:\\Users\\Gokce\\Documents\\freq\\colorfreq-%s.txt",cstr);
			fstream file_op(mfile,ios::out);
						
					file_op<<segno<<"	"<< arg_max_v <<"	"<< currentMax_v <<"	"<<endl;
				
		
			file_op.close();
	}
		
		void hesapla(int);

		int main()
		{
			LUT(relationLUT);
			keepcolors("SegmentColors.txt");
			createHash("ColorQuantas.txt");
				
			char choice;
			do{
			int image_no, segment_no;
			cout<<"Image no (10-106): ";
			cin>>image_no;
		
				hesapla(image_no);
				/*cout<<"Enter choice : (C-Continiue//E-Exit)"<<endl;
				cin>>choice;
*/
			}while(1);
			//while(choice == 'C' || choice == 'c');
					return 0;
		}

		void hesapla(int image_no){
		//for(int ivar=10; ivar<106; ivar++){ //10-106
											//200-592 
											//600-696
				int ivar = image_no;

				mystream << imagename1 <<  std::setfill('0') << std::setw(4) << ivar << type;
				string myfilename = mystream.str();
				mystream.str("");
				Mat src = imread(myfilename ,CV_LOAD_IMAGE_COLOR); //original image

				Mat dst;

				pyrDown(src, dst, Size(src.cols/2, src.rows/2) ); //downsample original image
				bilateralFilter (dst,myImData.original, 9, 30, 30 );//blur downsampled image

				
				vector<Mat> dst_channels;
				split(myImData.original,dst_channels);
				//split(dst,dst_channels);


			    cvtColor(dst, myImData.filtIntensity, CV_BGR2GRAY); // downsampled image directly converted to intensity image
			

			//	cvtColor(myImData.original, myImData.intensity, CV_BGR2GRAY); // downsampled+blurred image converted into intensity image
		/*		imshow("", myImData.intensity);
				waitKey(0);*/

				cvtColor(myImData.original, myImData.hsvImg,  CV_BGR2HSV); // downsampled+blurred image converted into hsv image

				myImData.h = myImData.original.rows;
				myImData.w = myImData.original.cols;

				vector<Mat> hsvchannels;
				split(myImData.hsvImg,hsvchannels);

				myImData.hsv_filter.push_back(hsvchannels[0]);
					myImData.hsv_filter.push_back(hsvchannels[1]);
						myImData.hsv_filter.push_back(hsvchannels[2]);
	
			hsvchannels.clear();			

								for(int k = 0; k < ( myImData.h * myImData.w); k++){
									int x = k %  (myImData.w);
									int y = (k - x) % (myImData.w - 1);
								bool flag=false;

								for (list<hsvc>::iterator it=col_hash_map.begin(); it != col_hash_map.end(); ++it){

					
									int val_h = myImData.hsv_filter.at(0).at<uchar>(y,x);
									int val_s = myImData.hsv_filter.at(1).at<uchar>(y,x);
									int val_v = myImData.hsv_filter.at(2).at<uchar>(y,x);
            

									if(val_h >= it->hlow && val_h <= it->hhigh && val_s >= it->slow && 
										val_s <= it->shigh && val_v >= it->vlow && val_v <= it->vhigh){		
						
											I[y][x]=it->col_name; 
						
											flag=true; break;		
										}

								}
							}

								Labeling(labelNr,label, I, Q ,EQ, myImData);
			
					LabelEqualization(EQ, label, myImData, labelColors);


					  /*Merge small components with their nearest component*/

						std::unordered_map<int, int> occurrences;
 
						for (int i = 0; i < myImData.h; ++i){
							for(int j = 0; j < myImData.w; ++j){

							++occurrences[label[i][j]];}}

					   for (int i = 0; i < myImData.h; ++i){
							for(int j = 0; j < myImData.w; ++j){

							if(occurrences[label[i][j]] < MAX_PxNr_SMALL_AREA) {
								EQ[label[i][j]] = Q[i][j];
								Q[i][j+1] = Q[i][j];

							}	
							}}
					   occurrences.clear();

								vector<int> nIndx;
								int indx=1;

								while (indx!=labelNr+1){
								contourArray.clear();
								for(int i = 0; i < myImData.h; i++) {
							for(int j = 0; j < myImData.w; j++) {

								int val=label[i][j];		
								if (val == indx){
									contourArray.push_back(Point(j,i));
									}
							}
								}
								if(contourArray.empty() == false){
			
									myImData.connComp.push_back(contourArray);
									int clrv;
			
								 // int clrv = I[contourArray.at(0).y][contourArray.at(0).x]; //yanlýþ renk birleþtirme olmasýn diye deðiþtiriyorum burayý 07.10.2015
									if(contourArray.size() == 1){
										 clrv = I[contourArray.at(0).y][contourArray.at(0).x];}
									if(contourArray.size()>1){
										clrv = I[contourArray.at(contourArray.size()-1).y][contourArray.at(contourArray.size()-1).x];}

								segmentClrs.push_back(clrv);
				
								if(contourArray.size()> MIN_PxNr_BIG_AREA){
									nIndx.push_back(myImData.connComp.size()-1);	
								}

								}			
								++indx;
								}

				for (int nfc = 0; nfc< nIndx.size(); nfc++ ){

						int numberofcomponents = nIndx.at(nfc);
						Mat component_Img = Mat::zeros(myImData.h,myImData.w,CV_8UC1);
						Mat dilated_component_Img,dst;
						Mat eroded_;
						// Create binary image of big segment
					for(int comp =0; comp < myImData.connComp.at(numberofcomponents).size(); comp++){
					Point component = myImData.connComp.at(numberofcomponents).at(comp);
					component_Img.at<uchar>(component.y,component.x)=255;}

					dilate(component_Img,dilated_component_Img,dilation_element);

					// Obtain adjacent parts
					cv::bitwise_xor(component_Img,dilated_component_Img,dst);
					//imshow("dst", dst); //adjacent img
					//	waitKey(0);

		
					vector<Point> nonZeroCoordinates;		//keep adjacent pixels in here
					findNonZero(dst, nonZeroCoordinates);

				int	ColorNr1 = segmentClrs.at(numberofcomponents);
				int ColorNr2;
				int newLabel = label[myImData.connComp.at(numberofcomponents).at(0).y][myImData.connComp.at(numberofcomponents).at(0).x];

				for(int g = 0; g<nonZeroCoordinates.size(); g++){
	
				Point AdjPoint	= nonZeroCoordinates.at(g);
					ColorNr2	= I[AdjPoint.y][AdjPoint.x];
					if(relationLUT[ColorNr1][ColorNr2] == 1){
	
					EQ[label[AdjPoint.y][AdjPoint.x]] = newLabel; 

					}

 				}
	
		}

		LabelEqualization(EQ, label, myImData, labelColors);

				myImData.connComp.clear();

						int nindx=1;
					while (nindx!=labelNr+1){
				
					contourArray.clear();
					for(int i = 0; i < myImData.h; i++) {
				for(int j = 0; j < myImData.w; j++) {

					int nval=label[i][j];		
					if (nval == nindx){
			
						contourArray.push_back(Point(j,i));
			
					}
				}
					}
					if(contourArray.empty() == false){
						myImData.connComp.push_back(contourArray);
					}			
					++nindx;
					}

				std::unordered_map<int, int> voting;

				char mvotfile[250];				
				sprintf(mvotfile,"C:\\Users\\Gokce\\Documents\\freq\\colorfreq-%i.txt",image_no);
						
				fstream file_opv(mvotfile,ios::out);				
							
				for(int segment_no=0; segment_no<myImData.connComp.size();segment_no++ )
				{
					
					
					for (int d =0; d<myImData.connComp.at(segment_no).size(); d++)
						{
					Point component = myImData.connComp.at(segment_no).at(d);
					//c_Img.at<uchar>(component.y,component.x)=dst.at<uchar>(component.y,component.x);
					//int colorinfo = I[component.y][component.x];
					++voting[I[component.y][component.x]];

						}
					int currentMax = 0;
					int arg_max = 0;
					for(auto it = voting.cbegin(); it != voting.cend(); ++it ) {
						if (it ->second > currentMax) {
							arg_max = it->first;
							currentMax = it->second;
						}
					}
					cout << "Value " << arg_max << " occurs " << currentMax << " times " << endl;					
					file_opv << segment_no <<"	"<< arg_max<<"	"<< currentMax <<"	"<<endl;	
					voting.clear();
				
				}file_opv.close();
	
		
			/*	I[AdjPoint.y][AdjPoint.x]*/

			/*	int colorinfo = I[myImData.connComp.at(segment_no).at(10).y][myImData.connComp.at(segment_no).at(10).x];
				double avhueval = avghueval(myImData,segment_no);
				cout<<myImData.connComp.at(segment_no).size()<<endl << avhueval<<endl << colorinfo <<endl<<endl;
*/
				nindx = 0;
					nIndx.clear();
					for (int ph=0; ph<pyrHeight; ph++){
						for(int pw=0; pw<pyrWidth; pw++){
							label[ph][pw]=0;
							I[ph][pw]=0;

						}
					}
						for (int ph=0; ph<pyrHeight; ph++){
						for(int pw=0; pw<pyrWidth+1; pw++){
		
							Q[ph][pw]=0;
			
						}
					}

					for(int eqn=0; eqn<MAX_EXP_NrOf_LABELS; eqn++){EQ[eqn]=0;}

					
					segmentClrs.clear();
					contourArray.clear();
				/*	mychannels.clear();
					fin_img.release();*/
					featureVector.clear();

	/*				ch0.release();
					ch1.release();
					ch2.release();*/

					myImData.filter.clear();	
					myImData.hsv_filter.clear();
				//	myImData.hsv_col_info.clear();
					myImData.connComp.clear();
					myImData.original.release();
				//	myImData.intensity.release();
					myImData.filtIntensity.release();
					myImData.hsvImg.release();
					labelNr=0;


		//}
		//tend = time(0); 
			//cout << "It took "<< difftime(tend, tstart) <<" second(s)."<< endl;
		
		}
