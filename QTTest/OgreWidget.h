#ifndef __OGREWIDGET_H__
#define __OGREWIDGET_H__

#include <Ogre.h>
#include <QGLWidget>
#include <QX11Info>
#include <qpushbutton.h>
#include <QMessageBox>
#include <QMouseEvent>
#include <vector>
#include <string>
#include <utility>
#include <QThread>
#include <QTimer>

#include "ETTerrainManager.h"
#include "ETTerrainInfo.h"
#include "ETBrush.h"
#include "ETSplattingManager.h"

enum GizmoType
{
	RED,
	GREEN,
	BLUE
};

class Gizmo
{

public:
	bool selected(Ogre::SceneNode* i_node)
	{
		return (i_node == m_node) ? true : false;
	}

	void makeEqualTo(Gizmo* i_gizmo)
	{
		m_gt = i_gizmo->m_gt;
		m_entity = i_gizmo->m_entity;
		m_node = i_gizmo->m_node;
	}

	void setMaterial(GizmoType i_gt)
	{
		if(i_gt == RED)
			m_entity->setMaterialName("RedHighlight");
		else if(i_gt == GREEN)
			m_entity->setMaterialName("GreenHighlight");
		else if(i_gt == BLUE)
			m_entity->setMaterialName("BlueHighlight");
	}


	GizmoType m_gt;
	Ogre::Entity* m_entity;
	Ogre::SceneNode* m_node;
};

class MyEntityAnim;

class OgreWidget : public QGLWidget
{
	Q_OBJECT;

public:
	Ogre::Viewport *mViewport;
	Ogre::Root *mOgreRoot;
	Ogre::RenderWindow *mOgreWindow;
	Ogre::Camera *mCamera;
	Ogre::SceneManager *mSceneMgr;

	Ogre::Entity* ent;
	Ogre::SceneNode* node;
	MyEntityAnim* mAnimStateThread;

	Ogre::Entity* ent2;
	Ogre::SceneNode* node2;

	Ogre::Entity* ent3;
	Ogre::SceneNode* node3;

	Ogre::SceneNode* g_node;

	QPointF lastPos;

	Gizmo* red_gizmo;
	Gizmo* green_gizmo;
	Gizmo* blue_gizmo;

	Gizmo* sel_gizmo;


	bool dxMouse;

	std::vector<std::pair<std::string, Ogre::SceneNode*>> obj;

	int count;

	bool rotate;
	bool scaleUp;
	bool scaleDw;

	bool mCtrlPress;
	bool mAltPress;
	bool mShiftPress;

	float rotX;
	float rotY;
	float rotZ;

	float rotNodeX;
	float rotNodeY;
	float rotNodeZ;

	float mTranslateX;

	bool mCreateLight;
	bool mSetShadow;
	bool mSetAnim;

	
	bool isRightMousePress;

	ET::TerrainManager* mTerrainMgr;
	ET::SplattingManager* mSplatMgr;
	float mTranslateY;
	float mTranslateZ;

	QTimer* timer;

	OgreWidget( QWidget *parent=0 ) : QGLWidget( parent ), mOgreWindow(NULL)
	{
		//setFocusPolicy(Qt::FocusPolicy::ClickFocus);

		init( "plugins_d.cfg", "ogre.cfg", "ogre.log" );
		
		mCtrlPress = false;
		mAltPress =false;
		mShiftPress = false;

		rotate = false;
		scaleUp = false;
		scaleDw = false;

		isRightMousePress = false;

		mCreateLight = false;
		mSetShadow = false;
		mSetAnim = false;

		mTranslateX = 0.0f;
		mTranslateY = 0.0f;
		mTranslateZ = 0.0f;

		rotX = 0.0f;
		rotY = 0.0f;
		rotZ = 0.0f;

		count = 0;

		dxMouse = false;

		g_node = nullptr;
		sel_gizmo = nullptr;

		red_gizmo = new Gizmo();
		green_gizmo = new Gizmo();
		blue_gizmo = new Gizmo();

		this->setMouseTracking(true);
		timer = new QTimer();
		timer->setInterval(30);
		timer->start();
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	}

	virtual ~OgreWidget()
	{
		mOgreRoot->shutdown();
		delete mOgreRoot;
		destroy();
	}

	void createStaticGeometry();
	void loadResource();
	
	void cameraMove()
	{
		mCamera->moveRelative(Ogre::Vector3(0, 0, -0.2));
	}
protected slots:

	void update()
	{
		updateGL();
	}

protected:
	virtual void initializeGL();
	virtual void resizeGL( int, int );
	virtual void paintGL();

	bool CreateNodeLight();
	bool setShadow();
	bool unsetShadow();

	static void qNormalizeAngle(int &angle)
	{
		while (angle < 0)
			angle += 360 * 16;
		while (angle > 360 * 16)
			angle -= 360 * 16;
	}

	/*void keyPressEvent(QKeyEvent *e)
	{
		switch( e->key() )
		{
		case Qt::Key_L:
			std::cout << "L PRESSED" << std::endl;
			break;
		}
	}*/

	void mouseMoveEvent(QMouseEvent *event)
	{
		std::cout << "EVENT TYPE: " << event->type() << std::endl;

		float dx = event->posF().rx() - lastPos.x();
		float dy = event->posF().ry() - lastPos.y();
		isRightMousePress = event->buttons() & Qt::RightButton;
		if (event->buttons() & Qt::RightButton) {
			rotX = 0.025 * dx;
			rotY = -0.025 * dy;
			
			//updateGL();
		}
		else if (mCtrlPress && !mAltPress && !mShiftPress) {
			mTranslateX = 0.025*dx;
			mTranslateY = -0.025*dy;
			//updateGL();
		}
		else if (mAltPress && mCtrlPress && !mShiftPress) {
			mTranslateX = 0.025*dx;
			mTranslateZ = -0.025*dy;
			//updateGL();
		}
		else if (!mAltPress && mCtrlPress && mShiftPress) {
			rotNodeX = 0.035 * dx;
			rotNodeY = -0.035 * dy;
			//updateGL();
		}
		lastPos = event->pos();
	}

	void mousePressEvent(QMouseEvent *event)
	{
		lastPos = event->pos();
		if (event->buttons() & Qt::LeftButton) {
			if(g_node != nullptr)
				g_node->showBoundingBox(false);
			
			float x = (float)((lastPos.x())/(float)(this->width()));
			float y = (float)((lastPos.y())/(float)(this->height()));

			/*
			QString string;
			string.sprintf("\nX = %d - <0.0 to 1.0 normalized> %f\nY = %d - <0.0 to 1.0 normalized> %f\n", 
			lastPos.x(),
			x,
			lastPos.y(),
			y);
			QMessageBox::information(this, "PICK AT", string);
			*/

			Ogre::Ray ray = mCamera->getCameraToViewportRay(x, y);

			Ogre::RaySceneQuery* mRayScnQuery = mSceneMgr->createRayQuery(Ogre::Ray());

			mRayScnQuery->setRay(ray);
			mRayScnQuery->setSortByDistance(true);

			Ogre::RaySceneQueryResult &result = mRayScnQuery->execute();
			Ogre::RaySceneQueryResult::iterator iter = result.begin();

			if(iter != result.end() && iter->movable)
			{
				g_node = const_cast<Ogre::SceneNode*>(iter->movable->getParentSceneNode());
				g_node->showBoundingBox(true);
				std::cout << "DEBUG POINT: " << ray.getPoint(result.begin()->distance) << std::endl;

				////	if(g_node->getName().compare(red_gizmo->m_node->getName()) == 0)
				////	{
				////		//((Ogre::Entity*)(g_node->getAttachedObject(0)))->setMaterialName("YellowHighlight");
				////		if(sel_gizmo) sel_gizmo->setMaterial(sel_gizmo->m_gt);
				////		red_gizmo->m_entity->setMaterialName("YellowHighlight");
				////		sel_gizmo = red_gizmo;
				////	}
				////	else if(g_node->getName().compare(green_gizmo->m_node->getName()) == 0)
				////	{
				////		if(sel_gizmo) sel_gizmo->setMaterial(sel_gizmo->m_gt);
				////		green_gizmo->m_entity->setMaterialName("YellowHighlight");
				////		sel_gizmo = green_gizmo;
				////	}
				////	else if(g_node->getName().compare(blue_gizmo->m_node->getName()) == 0)
				////	{
				////		if(sel_gizmo) sel_gizmo->setMaterial(sel_gizmo->m_gt);
				////		blue_gizmo->m_entity->setMaterialName("YellowHighlight");
				////		sel_gizmo = blue_gizmo;
				////	}
			}
			else
			{
				g_node = nullptr;
				//if(sel_gizmo) sel_gizmo->m_entity->setVisible(false);
			}
		}
		//this->updateGL();
	}

	void wheelEvent(QWheelEvent *event)
	{
		if(g_node != nullptr && g_node->getName().compare("RED_ROT_GIZMO_NODE"))
		{
			event->delta() > 0 ? g_node->scale(1.02, 1.02, 1.02) : g_node->scale(0.98, 0.98, 0.98);
			//updateGL();
		}
		else
		{
			event->delta() > 0 ? mCamera->moveRelative(Ogre::Vector3(0, 0, -0.2)) : mCamera->moveRelative(Ogre::Vector3(0, 0, 0.2));
			//updateGL();
		}
	}

	void init( std::string, std::string, std::string );

	virtual Ogre::RenderSystem* chooseRenderer( Ogre::RenderSystemList* );

};

class MyEntityAnim : public QThread
{
	Ogre::AnimationState* _aniState;
	unsigned long mMilliSec;
	bool mIsStarted;
	OgreWidget* mOgrePtr;

public:
	MyEntityAnim(Ogre::Entity* ptr,Ogre::String animName, OgreWidget* ptrOgreW)
		:QThread()
		,_aniState(ptr->getAnimationState(animName))
		,mMilliSec(25)
		,mOgrePtr(ptrOgreW)
		,mIsStarted(false)
	{

	}

	void setAnimation()
	{
		_aniState->setEnabled(true);
		_aniState->setLoop(true);
	}

	void setAnimStep(unsigned long time)
	{
		mMilliSec = time;
	}

	bool isStarted()
	{
		return mIsStarted;
	}

	bool Stop()
	{
		mIsStarted = false;
	}

	void run()
	{
		mIsStarted = true;
		while(1)
		{
			msleep(mMilliSec);
			if(mOgrePtr->mSetAnim)
			{
				_aniState->addTime(0.01);
				//mOgrePtr->updateGL();
			}
			else
			{
				mIsStarted = false;
				break;
			}
		}
	}
};

#endif