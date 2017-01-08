#include "Volume.h"

#include <math.h>


//-------------------------------------------------------------------------------------------------
// Voxel
//-------------------------------------------------------------------------------------------------

Voxel::Voxel() 
{
	setValue(0.0f);
}


Voxel::Voxel( float value)
{
	setValue(value);
}

Voxel::~Voxel()
{
}

void Voxel::setValue( float value)
{
	m_Value = value;
}

void Voxel::setGrad( float x,  float y,  float z)
{
	m_Grad[0] = x;
	m_Grad[1] = y;
	m_Grad[2] = z;
}

void Voxel::setLight(int x, int y, int z, int width, int height, int depth)
{
	m_LightVec[0] = width-x;
	m_LightVec[1] = height - y;
	m_LightVec[2] =  depth - z;
}

float Voxel::getLightLength()
{
	float tmp = (m_LightVec[0] * m_LightVec[0]) + (m_LightVec[1] * m_LightVec[1]) + (m_LightVec[2] * m_LightVec[2]);
	return sqrt(tmp);
}

float Voxel::getGradLength()
{
	float tmp = (m_Grad[0] * m_Grad[0]) + (m_Grad[1] * m_Grad[1]) + (m_Grad[2] * m_Grad[2]);
	return sqrt(tmp);
}


 float Voxel::getValue() 
{
	return m_Value;
};

 bool Voxel::operator==( Voxel &other) 
{
	return (getValue() == other.getValue());
};

 bool Voxel::operator!=( Voxel &other) 
{
	return !(*this == other);
};

 bool Voxel::operator>( Voxel &other) 
{
	return getValue() > other.getValue();
};

 bool Voxel::operator>=( Voxel &other) 
{
	return getValue() >= other.getValue();
};

 bool Voxel::operator<( Voxel &other) 
{
	return getValue() < other.getValue();
};

 bool Voxel::operator<=( Voxel &other) 
{
	return getValue() <= other.getValue();
};

 Voxel& Voxel::operator+=( Voxel &other)
{
	m_Value += other.m_Value;
	return *this;
};

 Voxel& Voxel::operator-=( Voxel &other)
{
	m_Value -= other.m_Value;
	return *this;
};

 Voxel& Voxel::operator*=( float &value)
{
	m_Value *= value;
	return *this;
};

 Voxel& Voxel::operator/=( float &value)
{
	m_Value /= value;
	return *this;
};

 Voxel Voxel::operator+( Voxel &other) 
{
	Voxel voxNew = *this;
	voxNew += other;
	return voxNew;
};

 Voxel Voxel::operator-( Voxel &other) 
{
	Voxel voxNew = *this;
	voxNew -= other;
	return voxNew;
};

 Voxel Voxel::operator*( float &value) 
{
	Voxel voxNew = *this;
	voxNew *= value;
	return voxNew;
};

 Voxel Voxel::operator/( float &value) 
{
	Voxel voxNew = *this;
	voxNew /= value;
	return voxNew;
};


//-------------------------------------------------------------------------------------------------
// Volume
//-------------------------------------------------------------------------------------------------

Volume::Volume()
	: m_Width(1), m_Height(1), m_Depth(1), m_Size(0), m_Voxels(1)
{
}

Volume::~Volume()
{
}



Voxel& Volume::voxel( int x,  int y,  int z) 
{
	return m_Voxels[x + y*m_Width + z*m_Width*m_Height];
}

 Voxel& Volume::voxel( int i) 
{
	return m_Voxels[i];
}

 Voxel* Volume::voxels() 
{
	return &(m_Voxels.front());
};

 int Volume::width() 
{
	return m_Width;
};

 int Volume::height() 
{
	return m_Height;
};

 int Volume::depth() 
{
	return m_Depth;
};

 int Volume::size() 
{
	return m_Size;
};


//-------------------------------------------------------------------------------------------------
// Volume File Loader
//-------------------------------------------------------------------------------------------------

 bool Volume::loadFromFile(QString filename, QProgressBar* progressBar)
 {
	 // load file
	 FILE *fp = NULL;
	 fopen_s(&fp, filename.toStdString().c_str(), "rb");
	 if (!fp)
	 {
		 std::cerr << "+ Error loading file: " << filename.toStdString() << std::endl;
		 return false;
	 }

	 // progress bar

	 progressBar->setRange(0, m_Size + 10);
	 progressBar->setValue(0);


	 // read header and set volume dimensions

	 unsigned short uWidth, uHeight, uDepth;
	 fread(&uWidth, sizeof(unsigned short), 1, fp);
	 fread(&uHeight, sizeof(unsigned short), 1, fp);
	 fread(&uDepth, sizeof(unsigned short), 1, fp);

	 m_Width = int(uWidth);
	 m_Height = int(uHeight);
	 m_Depth = int(uDepth);

	 // check dataset dimensions
	 if (
		 m_Width <= 0 || m_Width > 1000 ||
		 m_Height <= 0 || m_Height > 1000 ||
		 m_Depth <= 0 || m_Depth > 1000)
	 {
		 std::cerr << "+ Error loading file: " << filename.toStdString() << std::endl;
		 std::cerr << "Unvalid dimensions - probably loaded .dat flow file instead of .gri file?" << std::endl;
		 return false;
	 }

	 // compute dimensions
	 int slice = m_Width * m_Height;
	 m_Size = slice * m_Depth;
	 int test = INT_MAX;
	 m_Voxels.resize(m_Size);


	 // read volume data

	 // read into vector before writing data into volume to speed up process
	 std::vector<unsigned short> vecData;
	 vecData.resize(m_Size);
	 fread((void*)&(vecData.front()), sizeof(unsigned short), m_Size, fp);
	 fclose(fp);

	 progressBar->setValue(10);


	 // store volume data

	 for (int i = 0; i < m_Size; i++)
	 {
		 // data is converted to FLOAT values in an interval of [0.0 .. 1.0];
		 // uses 4095.0f to normalize the data, because only 12bit are used for the
		 // data values, and then 4095.0f is the maximum possible value
		 //(0.0f, std::fmin(1.0f, (float(vecData[i]) / 4095.0f));
		 float value = std::fmax(0.0f, float(vecData[i]) / 4095.0f);
		 m_Voxels[i] = Voxel(value);

		 progressBar->setValue(10 + i);
	 }

	 //Gradientenberechnung
	 for (int i = 0; i < width(); i++)
	 {
		 for (int j = 0; j < height(); j++)
		 {
			 for (int k = 0; k < depth(); k++)
			 {
				 if (Volume::valuesAreOkay(i, j, k)){
					 if (i == 40){
						 i = 40;
					 }
					 //forward differencing
					 float dX = (float)(((float)(voxel((i + 1), j, k).getValue()) - (float)(voxel(i, j, k).getValue())) / 2);
					 float dY = (float)(((float)(voxel(i, (j + 1), k).getValue()) - (float)(voxel(i, j, k).getValue())) / 2);
					 float dZ = (float)(((float)(voxel(i, j, (k + 1)).getValue()) - (float)(voxel(i, j, k).getValue())) / 2);
					 (voxel(i, j, k)).setGrad(dX, dY, dZ);
					 (voxel(i, j, k)).setLight(i,j,k, width(), height(), depth());
				 }

			 }

		 }
	 }
 
	progressBar->setValue(0);

	std::cout << "Loaded VOLUME with dimensions " << m_Width << " x " << m_Height << " x " << m_Depth << std::endl;

	return true;
}

bool Volume::valuesAreOkay(int x, int y, int z)
{
	if (x + 1 < height() && y + 1 < width() && z + 1 < depth()){
		return true;
	}
	return false;

}

