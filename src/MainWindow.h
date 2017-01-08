
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "Volume.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>
#include <QVariant>


class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:

		MainWindow(QWidget *parent = 0);
		~MainWindow();

		Volume								 getVolume();
		int									 currentWidth;
		int								     currentHeight;
		int									 currentDepth;
		int								     currentState;
		
		

	
	protected slots :

		void								 openFileAction();
		void								 closeAction();
		void								 rotateCCW();
		void								 rotateCW();
		void								 doTrans();
		void								 pushPlusX();
		void								 pushPlusY();
		void								 pushPlusZ();
		void								 pushMinusX();
		void								 pushMinusY();
		void								 pushMinusZ();
		
		
		

	private:

		// USER INTERFACE ELEMENTS

		Ui_MainWindow						*m_Ui;
		float								getHighestValue(int x, int y);
		QImage								*image;
		float								getFirstValue(int x, int y, float threshold);
		float								getGrad(int x, int y,float threashold);
		double								transferfunction(int color, double a);
		int									getFirstZ(int x, int y,float threshold);
		QColor							    getAlpha(int x, int y, float threshold);

		// DATA 

		enum DataType
		{
			VOLUME					= 0,
		};

		struct FileType
		{
			QString			filename;
			DataType		type;
		}									 m_FileType;

		Volume								*m_Volume;						// for Volume-Rendering

};

#endif
