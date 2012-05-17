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
#include <QStringList>
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

		connect(ogreWidget,SIGNAL(setComboAnimBox(const QStringList&)),this,SLOT(SetComboAnim(const QStringList&)));
		connect(ogreWidget,SIGNAL(ClearAnimBox()),this,SLOT(ClearComboAnim()));
		connect(ui.comboBox_Animation, SIGNAL(currentIndexChanged(int)), this, SLOT(test_comboAnim(int)));

		connect(ui.actionEnable_Shadow,SIGNAL(triggered()),this,SLOT(SetShadow()));
		connect(ui.actionEnable_Animation,SIGNAL(triggered()),this,SLOT(SetAnim()));
		connect(ui.actionAddEntity,SIGNAL(triggered()),this,SLOT(test()));
		connect(ui.actionDeleteEntity,SIGNAL(triggered()),this,SLOT(destroyEntity()));
		connect(ui.actionCreate_Light,SIGNAL(triggered()),this,SLOT(CreateNewLight()));

		connect(ogreWidget,SIGNAL(SetEntityPosLabel()),this,SLOT(SetPositionSelctedEntity()));

		ConnectScaleTraRotCounter();

		connect(ui.listWidget, SIGNAL(itemClicked( QListWidgetItem *)), this, SLOT(debug_row()));


		ui.actionExit->setShortcuts(QKeySequence::Quit);
		ui.actionExit->setStatusTip(tr("Exit the application"));
		connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));

	}

	void ConnectScaleTraRotCounter()
	{
		connect(ui.CounterScaleX,SIGNAL(valueChanged ( double )),this,SLOT(changeScaleCounterValueX(double)));
		connect(ui.CounterScaleY,SIGNAL(valueChanged ( double )),this,SLOT(changeScaleCounterValueY(double)));
		connect(ui.CounterScaleZ,SIGNAL(valueChanged ( double )),this,SLOT(changeScaleCounterValueZ(double)));

		connect(ui.CounterRotX,SIGNAL(valueChanged ( double )),this,SLOT(changeRotCounterValueX(double)));
		connect(ui.CounterRotY,SIGNAL(valueChanged ( double )),this,SLOT(changeRotCounterValueY(double)));
		connect(ui.CounterRotZ,SIGNAL(valueChanged ( double )),this,SLOT(changeRotCounterValueZ(double)));

		connect(ui.CounterTraX,SIGNAL(valueChanged ( double )),this,SLOT(changeTraCounterValueX(double)));
		connect(ui.CounterTraY,SIGNAL(valueChanged ( double )),this,SLOT(changeTraCounterValueY(double)));
		connect(ui.CounterTraZ,SIGNAL(valueChanged ( double )),this,SLOT(changeTraCounterValueZ(double)));
	}

	void cameraMove(const Ogre::Vector3 &moveVector)
	{
		ogreWidget->mCamera->moveRelative(moveVector);
		//ogreWidget->updateGL();
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
			//ogreWidget->updateGL();
			break;
		case Qt::Key_Down:
			std::cout << "Key_Down PRESSED" << std::endl;
			if(ogreWidget->g_node != nullptr)
				ogreWidget->g_node->translate(0, -0.1, 0);
			//ogreWidget->updateGL();
			break;
		case Qt::Key_Left:
			std::cout << "Key_Left PRESSED" << std::endl;
			if(ogreWidget->g_node != nullptr)
				ogreWidget->g_node->translate(-0.1, 0, 0);
			//ogreWidget->updateGL();
			break;
		case Qt::Key_Right:
			std::cout << "Key_Right PRESSED" << std::endl;
			if(ogreWidget->g_node != nullptr)
				ogreWidget->g_node->translate(0.1, 0, 0);
			//ogreWidget->updateGL();
			break;
		case Qt::Key_PageUp:
			std::cout << "Key_PageUp" << std::endl;
			if(ogreWidget->g_node != nullptr)
				ogreWidget->g_node->translate(0, 0, -0.1);
			//ogreWidget->updateGL();
			break;
		case Qt::Key_PageDown:
			std::cout << "Key_PageDown" << std::endl;
			if(ogreWidget->g_node != nullptr)
				ogreWidget->g_node->translate(0, 0, 0.1);
			//ogreWidget->updateGL();
			break;

		case Qt::Key_B:
			std::cout << "Key_B" << std::endl;
			changeColour();
			//ogreWidget->updateGL();
			break;

		case  Qt::Key_Control:
			ogreWidget->mCtrlPress = true;
			break;
		case  Qt::Key_Alt:
			ogreWidget->mAltPress = true;
			break;
		case  Qt::Key_Shift:
			ogreWidget->mShiftPress = true;
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
		case  Qt::Key_Alt:
			ogreWidget->mAltPress = false;
			break;
		case  Qt::Key_Shift:
			ogreWidget->mShiftPress = false;
			break;
		}
	}

	private slots:

		void changeScaleCounterValueX(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getScale();
				ogreWidget->g_node->setScale(v,tmp.y,tmp.z);
			}
		}

		void changeScaleCounterValueY(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getScale();
				ogreWidget->g_node->setScale(tmp.x,v,tmp.z);
			}
		}

		void changeScaleCounterValueZ(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getScale();
				ogreWidget->g_node->setScale(tmp.x,tmp.y,v);
			}
		}

		void changeRotCounterValueX(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{

				ogreWidget->g_node->rotate(Ogre::Vector3::UNIT_Y,Ogre::Degree(v));
			}
		}

		void changeRotCounterValueY(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{

				ogreWidget->g_node->rotate(Ogre::Vector3::UNIT_X,Ogre::Degree(v));
			}
		}

		void changeRotCounterValueZ(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{

				ogreWidget->g_node->rotate(Ogre::Vector3::UNIT_Z,Ogre::Degree(v));
			}
		}

		void changeTraCounterValueX(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getPosition();
				ogreWidget->g_node->setPosition(v,tmp.y,tmp.z);
			}
		}

		void changeTraCounterValueY(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getPosition();
				ogreWidget->g_node->setPosition(tmp.x,v,tmp.z);
			}
		}

		void changeTraCounterValueZ(double v)
		{
			if(ogreWidget->g_node != nullptr)
			{
				Ogre::Vector3 tmp= ogreWidget->g_node->getPosition();
				ogreWidget->g_node->setPosition(tmp.x,tmp.y,v);
			}
		}

		void test_comboAnim(int v)
		{
			if(ogreWidget->g_node == nullptr)
				return;
			static int value = 0;
			std::stringstream ss;
			ss << value;

			std::cout << v << std::endl;
			std::cout << ui.comboBox_Animation->itemText(v).toStdString() << std::endl;
			if(ogreWidget->mAnimStateThread != nullptr)
			{
				ogreWidget->mAnimStateThread = new MyEntityAnim((Ogre::Entity*)(ogreWidget->g_node->getAttachedObject(0)),ui.comboBox_Animation->itemText(v).toStdString());
				ogreWidget->mAnimStateThread->setAnimation();
			}
		}

		void ClearComboAnim()
		{
			ui.comboBox_Animation->clear();
		}

		void changeColour()
		{
			if(miao)
			{
				miao = false;
				ogreWidget->mViewport->setBackgroundColour( Ogre::ColourValue( 0.5 , 0.5 , 0.5 ) );
				//ogreWidget->updateGL();
			}
			else
			{
				miao = true;
				ogreWidget->mViewport->setBackgroundColour( Ogre::ColourValue( 1.0 , 0.0 , 0.0 ) );
				//ogreWidget->updateGL();
			}
		}

		void test()
		{
			QFile file("Objects.txt");
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
				std::cout << "NO" << std::endl;

			QString line;
			QTextStream in(&file);

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
			//ogreWidget->updateGL();
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

				//ogreWidget->updateGL();
			}
		}

		void CreateNewLight()
		{
			ogreWidget->mCreateLight = true;
			//ogreWidget->updateGL();
		}

		void SetShadow()
		{
			ogreWidget->mSetShadow = !ogreWidget->mSetShadow;
			//ogreWidget->updateGL();
		}

		void SetComboAnim(const QStringList& texts)
		{
			ui.comboBox_Animation->clear();
			ui.comboBox_Animation->addItems(texts);
		}

		void SetAnim()
		{
			ogreWidget->mSetAnim = !ogreWidget->mSetAnim;
			//ogreWidget->updateGL();
		}

		void SetPositionSelctedEntity()
		{
			Ogre::Vector3 p = ogreWidget->g_node->getPosition();
			Ogre::Vector3 s = ogreWidget->g_node->getScale();
			std::stringstream tmp;
			tmp << "[x=" <<p.x<<"; y="<<p.y<<"; z="<<p.z<<";]";
			ui.labelCurrentPos->setText(QString(tmp.str().c_str()));
			ui.CounterTraX->setValue(p.x);
			ui.CounterTraY->setValue(p.y);
			ui.CounterTraZ->setValue(p.z);
			std::stringstream tmp2;
			tmp2 << "[x=" <<s.x<<"; y="<<s.y<<"; z="<<s.z<<";]";
			ui.labelCurrentScale->setText(QString(tmp2.str().c_str()));
			ui.CounterScaleX->setValue(s.x);
			ui.CounterScaleY->setValue(s.y);
			ui.CounterScaleZ->setValue(s.z);
		}

		void rotate()
		{
			ogreWidget->rotate = true;
			//ogreWidget->updateGL();
		}

		void scaleUp()
		{
			ogreWidget->scaleUp = true;
			//ogreWidget->updateGL();
		}

		void scaleDown()
		{
			ogreWidget->scaleDw = true;
			//ogreWidget->updateGL();
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
			//ogreWidget->updateGL();
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
			//ogreWidget->updateGL();
		}
};