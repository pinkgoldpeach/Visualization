

#include "MainWindow.h"

#include <QFileDialog>

#include <QPainter>
#include <QPixmap>
#include <qmessagebox.h>
#include <math.h>       


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent), m_Volume(0), image(new QImage(280, 280, QImage::Format_RGB32))
{
	m_Ui = new Ui_MainWindow();
	m_Ui->setupUi(this);
	
	connect(m_Ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileAction()));
	connect(m_Ui->actionClose, SIGNAL(triggered()), this, SLOT(closeAction()));
	connect(m_Ui->CCW_rotation, SIGNAL(released()), this, SLOT(rotateCCW()));
	connect(m_Ui->pushPlusX, SIGNAL(released()), this, SLOT(pushPlusX()));
	connect(m_Ui->pushPlusY, SIGNAL(released()), this, SLOT(pushPlusY()));
	connect(m_Ui->pushPlusZ, SIGNAL(released()), this, SLOT(pushPlusZ()));
	connect(m_Ui->CW_rotation, SIGNAL(released()), this, SLOT(rotateCW()));
	connect(m_Ui->color_bar, SIGNAL(valueChanged(int)), this, SLOT(doTrans()));
	connect(m_Ui->MIP_trans, SIGNAL(released()), this, SLOT(doTrans()));
	connect(m_Ui->alpha_trans, SIGNAL(released()), this, SLOT(doTrans()));
	connect(m_Ui->FirstHit_trans, SIGNAL(released()), this, SLOT(doTrans()));
	connect(m_Ui->Grad_trans, SIGNAL(released()), this, SLOT(doTrans()));
	connect(m_Ui->pushMinusX, SIGNAL(released()), this, SLOT(pushMinusX()));
	connect(m_Ui->pushMinusY, SIGNAL(released()), this, SLOT(pushMinusY()));
	connect(m_Ui->pushMinusZ, SIGNAL(released()), this, SLOT(pushMinusZ()));

	

	//init slidebar with maximal value 255
	m_Ui->color_bar->setMaximum(255);
	m_Ui->color_bar->setMinimum(0);

}

MainWindow::~MainWindow()
{
	delete m_Volume;
}


//-------------------------------------------------------------------------------------------------
// Slots
//-------------------------------------------------------------------------------------------------

void MainWindow::openFileAction()
{
	QString filename = QFileDialog::getOpenFileName(this, "Data File", 0, tr("Data Files (*.dat *.gri *.csv)"));

	if (!filename.isEmpty())
	{
		// store filename
		m_FileType.filename = filename;
		std::string fn = filename.toStdString();
		bool success = false;

		// progress bar and top label
		m_Ui->progressBar->setEnabled(true);
		m_Ui->labelTop->setText("Loading data ...");

		// load data according to file extension
		if (fn.substr(fn.find_last_of(".") + 1) == "dat")		// LOAD VOLUME
		{
			// create VOLUME
			m_FileType.type = VOLUME; 
			m_Volume = new Volume();

			// load file
			success = m_Volume->loadFromFile(filename, m_Ui->progressBar);
		}

		//m_Ui->progressBar->setEnabled(false);

		// status message
		if (success)
		{
			QString type;
			if (m_FileType.type == VOLUME) type = "VOLUME";
			m_Ui->labelTop->setText("File LOADED [" + filename + "] - Type [" + type + "]");

			//initialisiere die Achsen
			currentWidth = m_Volume->width();		//x
			currentHeight = m_Volume->height();		//y
			currentDepth = m_Volume->depth();		//z

			//positive z richtung
			currentState = 3;

			//anfangs wird immer Richtung z-Achse abgebildet
			image = new QImage(currentWidth, currentHeight, QImage::Format_RGB32);
			m_Ui->label->setPixmap(QPixmap::fromImage(*image));
			
			doTrans();
		}
		else
		{
			
			m_Ui->labelTop->setText("ERROR loading file " + filename + "!");
			m_Ui->progressBar->setValue(0);
		}
	}
}

/***
Bildrotation gegen den Uhrzeigersinn
*/
void MainWindow::rotateCCW() {
	if (m_Ui->label->pixmap() != nullptr){
		QImage *tmp = new QImage(image->height(), image->width(), QImage::Format_RGB32);
		for (int i = 0; i<image->width(); i++) {
		for (int j = 0; j<image->height(); j++) {
			QRgb newValue = image->pixel(i, image->height() - 1 - j);
			tmp->setPixel(j, i, newValue);
		}
	}
	image = tmp;
	m_Ui->label->setPixmap(QPixmap::fromImage(*image));
	}

	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}


/***
Bildrotation Uhrzeigersinn
*/
void MainWindow::rotateCW()
{
	if (m_Ui->label->pixmap() != nullptr){
		QImage *tmp = new QImage(image->height(), image->width(), QImage::Format_RGB32);

		for (int i = 0; i<image->height(); i++) {
			for (int j = 0; j<image->width(); j++) {
				QRgb newValue = image->pixel(image->width() - 1 - j, i);
				tmp->setPixel(i, j, newValue);
			}
		}

		image = tmp;
		m_Ui->label->setPixmap(QPixmap::fromImage(*image));
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

/**
Sucht den hoechsten Dichtewert in Richtung der derzeitigen Sicht (currentState)
*/
float MainWindow::getHighestValue(int x, int y)
{
	float highest = 0.0;
	//Es wird in die positive Richtung geblickt
	if (currentState <= 3){ 
		for (int i = 0; i < currentDepth; i++)
		{
			float dValue;
			if (currentState == 3){ //positive z-Achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 2) {//positive y-Achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {//positive x-Achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > highest) 
			{
				highest = dValue;
			}
		}
	}
	else { //negative Achsen
		for (int i = currentDepth-1; i >=0; i--)
		{
			float dValue;
			if (currentState == 6){ //negative z achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 5) { //negative y achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else{ //negative x achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > highest)
			{
				highest = dValue;
			}
		}
	}
		return highest;	
}

//
float MainWindow::getFirstValue(int x, int y, float threshold)
{
	//suche nach dem ersten dichtewert welches groesser ist als der uebergebene schwellwert
	if (currentState <= 3) {
		for (int i = 0; i < currentDepth; i++)
		{
			float dValue =0;
			if (currentState == 3){  //positive z achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 2) {  //positive y achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {  //positive x achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > threshold){
				return dValue;
			}
		}
	}
	else
	{
		for (int i = currentDepth - 1; i >= 0; i--)
		{
			float dValue;
			if (currentState == 6){  //negative z achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 5) {  //negative y achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {  //negative x achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > threshold){
				return dValue;
			}
		}
	}
	

}

void MainWindow::closeAction()
{
	close();
}



Volume MainWindow::getVolume()
{
	return *m_Volume;
}


/*
* Render je nach ausgewaehlte Rendertechnik
*/
void MainWindow::doTrans()
{
	if (m_Ui->label->pixmap() != nullptr){
		
		
		QImage *tmp = new QImage(currentWidth, currentHeight, QImage::Format_RGB32);
		float tmpBar = ((float)m_Ui->color_bar->value());

		for (int x = 0; x < currentWidth; x++)
		{
			QRgb value;
			for (int y = 0; y < currentHeight; y++)
			{
				if (m_Ui->MIP_trans->isChecked())
				{

					//Do MIP transformation
					float highest = getHighestValue(x, y);
					if (highest == 0) {
						value = qRgb(255, 255, 255);
						tmp->setPixel(x, y, value);
					}
					else
					{
						value = qRgb(255, (1 - highest) *tmpBar, 255);
						tmp->setPixel(x, y, value);
					}
				}
				else if (m_Ui->FirstHit_trans->isChecked()) 
				{
					
					//Do First Hit 
					float first = getFirstValue(x, y, float(tmpBar/255));
					if (first == 0) {
						value = qRgb(255, 255, 255);
						tmp->setPixel(x, y, value);
					}
					else
					{
						
						value = qRgb(255,(1- first) * 255, 255);

						tmp->setPixel(x, y, value);
					}
				}
				else if (m_Ui->alpha_trans->isChecked())
				{
					
					//Do Alpha Compositing
					value = getAlpha(x, y, (tmpBar/255)).rgba();
					tmp->setPixel(x, y, value);
						
				}
				else if (m_Ui->Grad_trans->isChecked())
				{
					

					//Do Gradientenshading
					float cos = getGrad(x, y, (tmpBar/255));
					if (cos > 0)
					{
						cos = 1;
					}
					float ambiantLight = 0.9;	
					value = qRgb(255, (1 - cos*ambiantLight) * 255, 255);
					tmp->setPixel(x, y, value);
				}				
			}
		}
		image = tmp;
		m_Ui->label->setPixmap(QPixmap::fromImage(*image));
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

int MainWindow::getFirstZ(int x, int y, float threshold){
	int firstZ = -1;
	//suche nach dem ersten z Index welche eine obeflaeche sein koennte

	if (currentState <= 3) {
		for (int i = 0; i < currentDepth; i++)
		{
			float dValue;
			if (currentState == 3){  //positive z achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 2) {  //positive y achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {  //positive x achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > threshold){
				firstZ = i;
				i = currentDepth;
			}
		}
	}
	else
	{
		for (int i = currentDepth - 1; i >= 0; i--)
		{
			float dValue;
			if (currentState == 6){  //negative z achse
				dValue = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 5) {  //negative y achse
				dValue = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {  //negative x achse
				dValue = (m_Volume->voxel(i, x, y)).getValue();
			}

			if (dValue > threshold){
				firstZ = i;
				i = 0;
			}
		}
	}
	return firstZ;
}

//Gradientenshading - calculate cosAlpha (Dot Product of N an L)
float MainWindow::getGrad(int x, int y, float threshold)
{
	//suche nach dem ersten z Index welche eine obeflaeche sein koennte
	int z = getFirstZ(x,y,threshold);

	if (z >= 0){
		float N[3];
		float L[3];

		//uebergebe N normierten Gradientenvektor und L normierten Vektor zur Lichtquelle
		if (currentState == 3 || currentState == 6)
		{  //z achse
			if (currentState == 6){
				m_Volume->voxel(x, y, z).setLight(x, y, z, currentWidth*0.25, currentHeight*0.25, currentDepth);
			}
			else
			{
				m_Volume->voxel(x, y, z).setLight(x, y, z, currentWidth*0.25, currentHeight*0.25, 0);
			}
			N[0] = (m_Volume->voxel(x, y, z).m_Grad[0]) / (m_Volume->voxel(x, y, z).getGradLength());
			N[1] = (m_Volume->voxel(x, y, z).m_Grad[1]) / (m_Volume->voxel(x, y, z).getGradLength());
			N[2] = (m_Volume->voxel(x, y, z).m_Grad[2]) / (m_Volume->voxel(x, y, z).getGradLength());

			L[0] = (m_Volume->voxel(x, y, z).m_LightVec[0]) / (m_Volume->voxel(x, y, z).getLightLength());
			L[1] = (m_Volume->voxel(x, y, z).m_LightVec[1]) / (m_Volume->voxel(x, y, z).getLightLength());
			L[2] = (m_Volume->voxel(x, y, z).m_LightVec[2]) / (m_Volume->voxel(x, y, z).getLightLength());
		}
		else if (currentState == 2 || currentState == 5)
		{  
			//y achse
			if (currentState == 2){
				m_Volume->voxel(y, z, x).setLight(y, z, x, currentHeight*0.25, 0, currentWidth*0.25);
			}
			else
			{
				m_Volume->voxel(y, z, x).setLight(y, z, x, currentHeight*0.25, currentDepth, (currentWidth*0.25));
			}
			N[0] = (m_Volume->voxel(y, z, x).m_Grad[0]) / (m_Volume->voxel(y, z, x).getGradLength());
			N[1] = (m_Volume->voxel(y, z, x).m_Grad[1]) / (m_Volume->voxel(y, z, x).getGradLength());
			N[2] = (m_Volume->voxel(y, z, x).m_Grad[2]) / (m_Volume->voxel(y, z, x).getGradLength());

			
			L[0] = (m_Volume->voxel(y, z, x).m_LightVec[0]) / (m_Volume->voxel(y, z, x).getLightLength());
			L[1] = (m_Volume->voxel(y, z, x).m_LightVec[1]) / (m_Volume->voxel(y, z, x).getLightLength());
			L[2] = (m_Volume->voxel(y, z, x).m_LightVec[2]) / (m_Volume->voxel(y, z, x).getLightLength());
		}
		else
		{  //x achse
			if (currentState == 4){
				m_Volume->voxel(z, x, y).setLight(z, x, y, currentDepth, currentWidth*0.25, currentHeight*0.25);
			}
			else 
			{
				m_Volume->voxel(z, x, y).setLight(z, x, y, 0, currentWidth*0.25, currentHeight*0.25);
			}
			N[0] = (m_Volume->voxel(z, x, y).m_Grad[0]) / (m_Volume->voxel(z, x, y).getGradLength());



			N[1] = (m_Volume->voxel(z, x, y).m_Grad[1]) / (m_Volume->voxel(z, x, y).getGradLength());
			N[2] = (m_Volume->voxel(z, x, y).m_Grad[2]) / (m_Volume->voxel(z, x, y).getGradLength());

			
			L[0] = (m_Volume->voxel(z, x, y).m_LightVec[0]) / (m_Volume->voxel(z, x, y).getLightLength());
			L[1] = (m_Volume->voxel(z, x, y).m_LightVec[1]) / (m_Volume->voxel(z, x, y).getLightLength());
			L[2] = (m_Volume->voxel(z, x, y).m_LightVec[2]) / (m_Volume->voxel(z, x, y).getLightLength());
		}


		float cos = (N[0] * L[0]) + (N[1] * L[1]) + (N[2] * L[2]);


		return cos;
	}
	else 
	{
		return 0;
	}
	
}

//Achsen werden angepasst
void MainWindow::pushPlusX()
{
	if (m_Ui->label->pixmap() != nullptr){
		currentWidth = m_Volume->height();
		currentHeight = m_Volume->depth();
		currentDepth = m_Volume->width();
		currentState = 1; //Richtung positive x-Achse
		doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

void MainWindow::pushPlusY()
{
	if (m_Ui->label->pixmap() != nullptr){
	currentWidth = m_Volume->depth();
	currentHeight = m_Volume->width();
	currentDepth = m_Volume->height();
	currentState = 2;//Richtung positive y-Achse
	doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
	
}

void MainWindow::pushPlusZ()
{
	if (m_Ui->label->pixmap() != nullptr){
	currentWidth = m_Volume->width();
	currentHeight = m_Volume->height();
	currentDepth = m_Volume->depth();
	currentState = 3; //Richtung positive z-Achse
	doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

void MainWindow::pushMinusX() {
	if (m_Ui->label->pixmap() != nullptr){
	currentWidth = m_Volume->height();
	currentHeight = m_Volume->depth();
	currentDepth = m_Volume->width();
	currentState = 4; //negative x Achse
	doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

void MainWindow::pushMinusY() {
	if (m_Ui->label->pixmap() != nullptr){
	currentWidth = m_Volume->depth();
	currentHeight = m_Volume->width();
	currentDepth = m_Volume->height();
	currentState = 5; //negative Y Achse
	doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}

void MainWindow::pushMinusZ() {
	if (m_Ui->label->pixmap() != nullptr){
	currentWidth = m_Volume->width();
	currentHeight = m_Volume->height();
	currentDepth = m_Volume->depth();
	currentState = 6; //negative Z Achse
	doTrans();
	}
	else
	{
		QMessageBox::warning(
			this,
			tr("Volume Rendering"),
			tr("No Image Loaded."));
	}
}


//Alpha Compositing
QColor MainWindow::getAlpha(int x, int y, float threshold){
	double r = 0;
	double g = 0;
	double b = 0;
	double a = 0;
	double currentAlpha;
	if (currentState <= 3) {
		for (int i = 0; i < currentDepth; i++)
		{
			if (currentState == 3) //positive z richtung
			{
				currentAlpha = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 2) //positive y richtung
			{
				currentAlpha = (m_Volume->voxel(y, i, x)).getValue();
			}
			else //positive x richtung
			{
				currentAlpha = (m_Volume->voxel(i, x, y)).getValue();
			}
			double currentRed = transferfunction(0, currentAlpha);
			double currentGreen = transferfunction(1, currentAlpha);
			double currenBlue = transferfunction(2, currentAlpha);
			r = (1 - a)*currentRed + r;
			g = (1 - a)*currentGreen + g;
			b = (1 - a)*currenBlue + b;

			a = ((1 - a)*currentAlpha + a);
			if (a >= 1){
				a = 1;
				break;
			}
			
		}
	}
	else {
		for (int i = currentDepth - 1; i >= 0; i--)
		{
			if (currentState == 6){  //negative z achse
				currentAlpha = (m_Volume->voxel(x, y, i)).getValue();
			}
			else if (currentState == 5) {  //negative y achse
				currentAlpha = (m_Volume->voxel(y, i, x)).getValue();

			}
			else {  //negative x achse
				currentAlpha = (m_Volume->voxel(i, x, y)).getValue();
			}	
			double currentRed = transferfunction(0, currentAlpha);
			double currentGreen = transferfunction(1, currentAlpha);
			double currenBlue = transferfunction(2, currentAlpha);
			r = (1 - a)*currentRed + r;
			g = (1 - a)*currentGreen + g;
			b = (1 - a)*currenBlue + b;

			a = ((1 - a)*currentAlpha + a);
			if (a >= 1){
				a = 1;
				break;
			}
		}

		
	}
	if (a > threshold){
		return QColor::QColor((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
	}
	else{
		return QColor::QColor(255, 255, 255, 255);
	}
}


// r = 0; g = 1; b = 2, gibt die Farbe fuer Alpha Compositing retour
double MainWindow::transferfunction(int color, double a){
	if (a >= 0.75 && a <= 1){
		if (color == 0){
			double temp = (double)((51 / 255)*a);
			return temp;
		}
		else{
			return (double)0;
		}
	}
	else if (a >= 0.5 && a < 0.75){
		if (color == 0){
			return (double)0;
		}
		else{
			double temp = (double)((153 / 255)*a);
			return temp;
		}
	}
	else if (a >= 0.25 && a < 0.5){
		if (color == 1){
			double temp = (double)((153 / 255)*a);
			return temp;
		}
		else{
			return (double)0;
		}
	}

	else if (a >= 0 && a < 0.25){
		if (color == 0){
			double temp = ((200 / 255)*a);
			return temp;
		}
		else if (color == 1){
			double temp = (double)(1 * a);
			return temp;
		}
		else{
			double temp = (double)((200 / 255)*a);
			return temp;
		}
	}
}
