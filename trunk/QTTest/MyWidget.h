#include <QMainWindow>
#include "ogrewidget.h"
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <QMessageBox>
#include <Qlabel>
#include "ui_my.h"
#include <QFile>
#include <QTextStream>
#include <sstream>
#include <QFileDialog>

class MyMainWindow : public QMainWindow
{
	Q_OBJECT;

	OgreWidget* ogreWidget;
	QPushButton* butt1;
	QPushButton* butt2;
	QPushButton* butt3;
	QPushButton* butt4;

	QPushButton* butt5;
	Ui::MainWindow ui;

	bool miao;


public:
	MyMainWindow() : QMainWindow()
	{
		setFocusPolicy(Qt::FocusPolicy::StrongFocus);

		ui.setupUi(this);
		
		ogreWidget = new OgreWidget;
		ui.horizontalLayout_2->addWidget(ogreWidget);

		ui.listWidget->addItem("Sinbad_001");
		ui.listWidget->addItem("Sinbad_002");


		//butt1 = new QPushButton(this);
		//butt1->setText("BackGroundColor");
		//butt1->setGeometry(0,0,30,30);

		//butt2 = new QPushButton(this);
		//butt2->setText("Rotate Y");
		//butt2->setGeometry(0,0,30,30);

		//butt3 = new QPushButton(this);
		//butt3->setText("Scale Up");
		//butt3->setGeometry(0,0,30,30);

		//butt4 = new QPushButton(this);
		//butt4->setText("Scale Down");
		//butt4->setGeometry(0,0,30,30);

		//butt5 = new QPushButton(this);
		//butt5->setText("Pick 1");
		//butt5->setGeometry(0,0,30,30);

		///*label = new QLabel(this);
		//label->setText("Coccobello");
		//label->setGeometry(0,0,30,30);*/

		//QVBoxLayout *layout = new QVBoxLayout;
		//layout->addWidget(ogreWidget);
		//layout->addWidget(butt1);
		//layout->addWidget(butt2);
		//layout->addWidget(butt3);
		//layout->addWidget(butt4);
		//layout->addWidget(butt5);
		//
		//this->setLayout(layout);

		//miao = true;

		//connect(butt1, SIGNAL(clicked()), this, SLOT(changeColour()));
		//connect(butt2, SIGNAL(clicked()), this, SLOT(rotate()));
		//connect(butt3, SIGNAL(clicked()), this, SLOT(scaleUp()));
		//connect(butt4, SIGNAL(clicked()), this, SLOT(scaleDown()));
		
		connect(ui.listWidget, SIGNAL(itemClicked( QListWidgetItem *)), this, SLOT(debug_row()));

		ui.actionExit->setShortcuts(QKeySequence::Quit);
		ui.actionExit->setStatusTip(tr("Exit the application"));
		connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

		connect(ui.aaa, SIGNAL(clicked()), this, SLOT(test()));	

		fillComboBox();
		connect(ui.comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(test_combo(int)));
		connect(ui.aaa_2, SIGNAL(clicked()), this, SLOT(destroyEntity()));
	}

	void fillComboBox()
	{
		QFile file("Objects.txt");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			std::cout << "NO" << std::endl;
		
		QTextStream in(&file);
		while (!in.atEnd()) 
		{
			QString line = in.readLine();
			ui.comboBox->addItem(line);
			std::cout << line.toStdString() << std::endl;
		}
	}

	void cameraMove(const Ogre::Vector3 &moveVector)
	{
		ogreWidget->mCamera->moveRelative(moveVector);
		ogreWidget->updateGL();
	}

	void keyPressEvent(QKeyEvent *e)
	{
		switch( e->key() )
		{
			case Qt::Key_S:
				std::cout << "S PRESSED" << std::endl;
				cameraMove(Ogre::Vector3(0, 0, 0.1));
				break;
			case Qt::Key_W:
				std::cout << "W PRESSED" << std::endl;
				cameraMove(Ogre::Vector3(0, 0, -0.1));
				break;
			case Qt::Key_A:
				std::cout << "A PRESSED" << std::endl;
				cameraMove(Ogre::Vector3(0.1, 0, 0));
				break;
			case Qt::Key_D:
				std::cout << "D PRESSED" << std::endl;
				cameraMove(Ogre::Vector3(-0.1, 0, 0));
				break;
			case Qt::Key_Escape:
				std::cout << "ESC PRESSED" << std::endl;
				close();
				break;

			case Qt::Key_Up:
				std::cout << "Key_Up PRESSED" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(0, 0.1, 0);
				ogreWidget->updateGL();
				break;
			case Qt::Key_Down:
				std::cout << "Key_Down PRESSED" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(0, -0.1, 0);
				ogreWidget->updateGL();
				break;
			case Qt::Key_Left:
				std::cout << "Key_Left PRESSED" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(-0.1, 0, 0);
				ogreWidget->updateGL();
				break;
			case Qt::Key_Right:
				std::cout << "Key_Right PRESSED" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(0.1, 0, 0);
				ogreWidget->updateGL();
				break;
			case Qt::Key_PageUp:
				std::cout << "Key_PageUp" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(0, 0, -0.1);
				ogreWidget->updateGL();
				break;
			case Qt::Key_PageDown:
				std::cout << "Key_PageDown" << std::endl;
				if(ogreWidget->g_node != nullptr)
					ogreWidget->g_node->translate(0, 0, 0.1);
				ogreWidget->updateGL();
				break;

			case Qt::Key_B:
				std::cout << "Key_B" << std::endl;
				changeColour();
				ogreWidget->updateGL();
				break;

			case  Qt::Key_Control:
				ogreWidget->mCtrlPress = true;
				break;
		}
	}

	void keyReleaseEvent(QKeyEvent *e)
	{
		switch( e->key() )
		{
			case  Qt::Key_Control:
				ogreWidget->mCtrlPress = false;
				break;
		}
	}

private slots:
	void test_combo(int v)
	{
		static int value = 0;
		std::stringstream ss;
		ss << value;

		std::cout << v << std::endl;
		std::cout << ui.comboBox->itemText(v).toStdString() << std::endl;

		QString model = ui.comboBox->itemText(v);
		ogreWidget->ent3 = ogreWidget->mSceneMgr->createEntity(
			Ogre::String(model.toStdString().c_str()) + Ogre::String(ss.str().c_str()), Ogre::String(model.toStdString().c_str()) + ".mesh");
		
		ogreWidget->node3 = ogreWidget->mSceneMgr->getRootSceneNode()->
			createChildSceneNode("NODO_"  + Ogre::String(ss.str().c_str()) + Ogre::String(model.toStdString().c_str()));

		ogreWidget->node3->translate(-2, 0, 0);
		ogreWidget->node3->scale(0.5f, 0.5f, 0.5f);
		ogreWidget->node3->attachObject(ogreWidget->ent3);

		++value;
		ogreWidget->updateGL();
	}

    void changeColour()
	{
		if(miao)
		{
			miao = false;
			ogreWidget->mViewport->setBackgroundColour( Ogre::ColourValue( 0.5 , 0.5 , 0.5 ) );
			ogreWidget->updateGL();
		}
		else
		{
			miao = true;
			ogreWidget->mViewport->setBackgroundColour( Ogre::ColourValue( 1.0 , 0.0 , 0.0 ) );
			ogreWidget->updateGL();
		}
	}

	void test()
	{
		QFile file("Objects.txt");
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			std::cout << "NO" << std::endl;
		
		QString line;
		QTextStream in(&file);
		while (!in.atEnd()) 
		{
			line = in.readLine();
			ui.comboBox->addItem(line);
			std::cout << line.toStdString() << std::endl;
		}

		QString filename = QFileDialog::getOpenFileName(this,
			tr("Open File"), line, tr("Mesh Files (*.mesh)"));

		QStringList list;	
		list = filename.split(QChar('/'));

		filename = list.last();

		static int value = 0;
		std::stringstream ss;
		ss << value;

		ogreWidget->ent3 = ogreWidget->mSceneMgr->createEntity(
			Ogre::String(filename.toStdString().c_str()) + Ogre::String(ss.str().c_str()), Ogre::String(filename.toStdString().c_str()));
		
		ogreWidget->node3 = ogreWidget->mSceneMgr->getRootSceneNode()->
			createChildSceneNode("NODO_"  + Ogre::String(ss.str().c_str()) + Ogre::String(filename.toStdString().c_str()));

		ogreWidget->node3->translate(-2, 0, 0);
		ogreWidget->node3->scale(0.5f, 0.5f, 0.5f);
		ogreWidget->node3->attachObject(ogreWidget->ent3);

		++value;
		ogreWidget->updateGL();
	}

	void destroyEntity()
	{
		if(ogreWidget->g_node)
		{
			Ogre::String str_ent = ((Ogre::Entity*)(ogreWidget->g_node->getAttachedObject(0)))->getName();
			Ogre::String str_node = ogreWidget->g_node->getName();
			std::cout << str_ent << std::endl;
			std::cout << str_node << std::endl;

			ogreWidget->mSceneMgr->destroyEntity(str_ent);
			ogreWidget->mSceneMgr->destroySceneNode(str_node);

			ogreWidget->g_node = nullptr;

			ogreWidget->updateGL();
		}
	}

	void rotate()
	{
		ogreWidget->rotate = true;
		ogreWidget->updateGL();
	}

	void scaleUp()
	{
		ogreWidget->scaleUp = true;
		ogreWidget->updateGL();
	}

	void scaleDown()
	{
		ogreWidget->scaleDw = true;
		ogreWidget->updateGL();
	}

	void pick_one()
	{
		if(ogreWidget->g_node)
		{
			ogreWidget->g_node->showBoundingBox(false);
			ogreWidget->g_node = nullptr;
		}
		ogreWidget->g_node = ogreWidget->node;
		ogreWidget->g_node->showBoundingBox(true);
		ogreWidget->updateGL();
	}

	void debug_row()
	{
		if(ogreWidget->g_node)
		{
			ogreWidget->g_node->showBoundingBox(false);
			ogreWidget->g_node = nullptr;
		}
		std::cout << ui.listWidget->currentRow() << std::endl;
		ogreWidget->g_node = ogreWidget->obj[ui.listWidget->currentRow()].second;
		ogreWidget->g_node->showBoundingBox(true);
		ogreWidget->updateGL();
	}
};