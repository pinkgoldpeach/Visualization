#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <QProgressBar>


//-------------------------------------------------------------------------------------------------
// Voxel
//-------------------------------------------------------------------------------------------------

class Voxel
{
	public:

		Voxel();
		Voxel(float value);

		~Voxel();


		// VOXEL VALUE

		void					setValue( float value);
		float					getValue() ;

		//Gradientenberechnung
		void					setGrad( float x,  float y,  float z);
		void					setLight(int x, int y, int z, int width, int height, int depth);
		float					m_Grad[3];
		float					getGradLength();
		float					getLightLength();
		float					m_LightVec[3];
		


		// OPERATORS

		 bool				operator==( Voxel &other) ;
		 bool				operator!=( Voxel &other) ;
		 bool				operator>( Voxel &other) ;
		 bool				operator>=( Voxel &other) ;
		 bool				operator<( Voxel &other) ;
		 bool				operator<=( Voxel &other) ;

		 Voxel				operator+( Voxel &other) ;
		 Voxel				operator-( Voxel &other) ;
		 Voxel				operator*( float &value) ;
		 Voxel				operator/( float &value) ;
		
		 Voxel&			operator+=( Voxel &other);
		 Voxel&			operator-=( Voxel &other);
		 Voxel&			operator*=( float &value);
		 Voxel&			operator/=( float &value);


	private:

		float					m_Value;
		
		
		

};


//-------------------------------------------------------------------------------------------------
// Volume
//-------------------------------------------------------------------------------------------------

class Volume
{

	public:

		Volume();
		~Volume();


		// VOLUME DATA

		 Voxel&				voxel( int i) ;
		Voxel&				voxel( int x,  int y,  int z) ;
		 Voxel*				voxels() ;

		 int				width() ;
		 int				height() ;
		 int				depth() ;
		 
		 int				size() ;

		bool				loadFromFile(QString filename, QProgressBar* progressBar);


		private:

		std::vector<Voxel>		m_Voxels;

		int						m_Width;
		int						m_Height;
		int						m_Depth;
		bool				valuesAreOkay(int x, int y, int z);
		int						m_Size;

};
