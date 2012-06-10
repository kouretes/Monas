/**
* @file kc3table.cpp
* Convert a kcc .h file to a KSEG color table
* * @author Colin Graf
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;


struct SegHeader{
	char ID[2];//==KS
	char ruletype;//==R/C
	char size;//=1?
	char calType;//A:Autowb
	char calLines;
	char colorInfo;//A:Aliases
	char colorLines;
	char conf[4];//HSY\n OR Y[yres][ures][vres]
};



unsigned char calculatePixel(unsigned char yuv[3])
{
	int Feature01=yuv[0],Feature02=yuv[1], Feature03=yuv[2];
# 	include "classifier.h"
	return 0;
	
}

int main( int argc, char *argv[] )
{
  std::cout << "..::: KC3Table :::.." << std::endl;
  string dimensions="422";
  string output="segmentation.conf";
  int yres,ures,vres;
  struct SegHeader header;
  int i=0;
  header.ID[0]='K';
  header.ID[1]='S';
  header.ruletype='C';
  header.size='1';
  header.calType='A';//TODO
  header.calLines='0';
  header.colorInfo='A';
  header.colorLines='0';
  
  
  cout<<argc<<endl;
     // checking options
  while( i < argc ) {
    if ( std::string( argv[i] ) == "-d" )
    	dimensions = std::string( argv[++i] );
    else if ( std::string( argv[i] ) == "-o" )
    	output = string( argv[++i] );
    i++;
  }
  
  if(dimensions.size()!=3)
  {
	cout<<"Invalid dimension parameter!"<<endl;
	return 0;
  }
  header.conf[0]='Y';
  header.conf[1]=dimensions[0];
  header.conf[2]=dimensions[1];
  header.conf[3]=dimensions[2];
  yres=dimensions[0]-'0';
  ures=dimensions[1]-'0';
  vres=dimensions[2]-'0';
  cout<<"Table division:"<<yres<<" "<<ures<<" "<<vres<<endl;
  cout<<"Table size:"<<(256>>yres)<<"x"<<(256>>ures)<<"x"<<(256>>vres)<<endl;
  cout<<"Databytes:"<<((256>>yres)*(256>>ures)*(256>>vres)*sizeof(unsigned char ))<<endl;
  cout<<"Output file: "<<output<<endl;
  int y,u,v;
  unsigned char *	ctable =  (unsigned char *)malloc((256>>yres)*(256>>ures)*(256>>vres)*sizeof(unsigned char ));
	for(y=0;y<256>>yres;y++)
		for(u=0;u<256>>ures;u++)
			for(v=0;v<256>>vres;v++)
			{
				unsigned char data[3];
				
				
#define table_subscript(y,u,v) ( (y)*(256>>ures)*(256>>vres)*sizeof(unsigned char)+(u)*(256>>vres)*sizeof(unsigned char)+(v)*sizeof(unsigned char))

			   //INTERESTING POINT: value to sample for that region is actually the MIDDLE value 
                           // so if Y is 0-16-32..YRES=4
                           //Sampled values are  7 - 23- 39 etc
			   // This is done in HOPE of representing better the region at hand
			   data[0]=(y<<yres)+((1<<yres)-1)/2;
			   data[1]=(u<<ures)+((1<<ures)-1)/2;
			   data[2]=(v<<vres)+((1<<vres)-1)/2;
			   *(ctable+table_subscript(y,u,v))=calculatePixel(data);
			   if( *(ctable+table_subscript(y,u,v))!=calculatePixel(data) )
				cout<<(int)data[0]<<","<<(int)data[1]<<","<<(int)data[2]<<":"<<(int)*(ctable+table_subscript(y,u,v))<<":"<< (int)calculatePixel(data)<<endl ;
			}
			
  ofstream of(output.c_str(),ios_base::out|ios_base::binary|ios_base::trunc);
  
  of.write(reinterpret_cast<char *>(&header),sizeof(header));
  char Comment[1024];
  cout<<"Please type in a comment for the file:";
  cin.get(Comment,1024);
  of.write(Comment,strlen(Comment));
  of.put('\n');
  of.write(reinterpret_cast<char *>(ctable),(256>>yres)*(256>>ures)*(256>>vres)*sizeof(unsigned char ));
  of.close();
  cout<<"Done!"<<endl;
 return 0;
}
