#include <iostream>
#include "ogrewidget.h"

#define THIS OgreWidget


void THIS::init( std::string plugins_file,
	std::string ogre_cfg_file,
	std::string ogre_log )
{
	// create the main ogre object
	mOgreRoot = new Ogre::Root( plugins_file, ogre_cfg_file, ogre_log );

	

	//Ogre::ResourceGroupManager::getSingleton()
	//	.addResourceLocation("C:\\Users\\Alessandro\\Desktop\\Engine\\OgreSRC_v1-8-0\\Samples\\Media\\materials\\fonts", "FileSystem", "Popular");
	//
	//Ogre::ResourceGroupManager::getSingleton()
	//	.addResourceLocation("C:\\Users\\Alessandro\\Desktop\\Engine\\OgreSRC_v1-8-0\\Samples\\Media\\materials\\programs", "FileSystem", "Popular");
	//
	///*Ogre::ResourceGroupManager::getSingleton()
	//	.addResourceLocation("C:\\Users\\Alessandro\\Desktop\\scripts", "FileSystem", "Popular");
	//*/
	//Ogre::ResourceGroupManager::getSingleton()
	//	.addResourceLocation("C:\\Users\\Alessandro\\Desktop\\Engine\\OgreSRC_v1-8-0\\Samples\\Media\\materials\\textures", "FileSystem", "Popular");

	//Ogre::ResourceGroupManager::getSingleton()
	//	.addResourceLocation("C:\\Users\\Alessandro\\Desktop\\Engine\\OgreSRC_v1-8-0\\Samples\\Media\\packs\\Sinbad.zip","Zip", "Popular");

	//Ogre::ResourceGroupManager::getSingleton()
	//	.initialiseAllResourceGroups();	

	// setup a renderer
	//const Ogre::RenderSystemList &renderers = mOgreRoot->getAvailableRenderers();
	//assert( !renderers.empty() ); // we need at least one renderer to do anything useful

	//Ogre::RenderSystem *renderSystem;
	//renderSystem = chooseRenderer( const_cast<Ogre::RenderSystemList*> (&renderers ));

	//assert( renderSystem ); // user might pass back a null renderer, which would be bad!

	Ogre::RenderSystem *renderSystem = mOgreRoot->getAvailableRenderers()[0];
	mOgreRoot->setRenderSystem( renderSystem );

	//QString dimensions = QString( "%1x%2" )
	//	.arg(this->width())
	//	.arg(this->height());

	////renderSystem->setConfigOption( "Video Mode", dimensions.toStdString() );

	//// initialize without creating window
	//mOgreRoot->getRenderSystem()->setConfigOption( "Full Screen", "Yes" ); 
	//mOgreRoot->saveConfig();

	mOgreRoot->initialise(false); // don't create a window	
}

void THIS::loadResource()
{
	Ogre::ConfigFile cf;
	cf.load("resources_d.cfg");

	Ogre::ConfigFile::SectionIterator sectionIter = cf.getSectionIterator();
	
	Ogre::String sectionName, typeName, dataname;
	while (sectionIter.hasMoreElements())
	{
		sectionName = sectionIter.peekNextKey();
		
		Ogre::ConfigFile::SettingsMultiMap *settings = sectionIter.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			dataname = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataname, typeName, sectionName);
		}	
	}

	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void THIS::initializeGL()
{
	//== Creating and Acquiring Ogre Window ==//

	// Get the parameters of the window QT created
	Ogre::String winHandle;
#ifdef WIN32
	// Windows code
	winHandle += Ogre::StringConverter::toString((unsigned long)(this->parentWidget()->winId()));
#elif MACOS
	// Mac code, tested on Mac OSX 10.6 using Qt 4.7.4 and Ogre 1.7.3
	Ogre::String winHandle  = Ogre::StringConverter::toString(winId());
#else
	// Unix code
	QX11Info info = x11Info();
	winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
	winHandle += ":";
	winHandle += Ogre::StringConverter::toString((unsigned int)(info.screen()));
	winHandle += ":";
	winHandle += Ogre::StringConverter::toString((unsigned long)(this->parentWidget()->winId()));
#endif


	Ogre::NameValuePairList params;
#ifndef MACOS
	// code for Windows and Linux
	params["parentWindowHandle"] = winHandle;
	params["FSAA"] = "8";
	mOgreWindow = mOgreRoot->createRenderWindow( "QOgreWidget_RenderWindow",
		789,
		519,
		false,
		&params );

	mOgreWindow->setActive(true);
	WId ogreWinId = 0x0;
	mOgreWindow->getCustomAttribute( "WINDOW", &ogreWinId );

	assert( ogreWinId );

	// bug fix, extract geometry
	QRect geo = this->frameGeometry ( );

	// create new window
	this->create( ogreWinId );

	// set geometrie infos to new window
	this->setGeometry (geo);

#else
	// code for Mac
	params["externalWindowHandle"] = winHandle;
	params["macAPI"] = "cocoa";
	params["macAPICocoaUseNSView"] = "true";
	mOgreWindow = mOgreRoot->createRenderWindow("QOgreWidget_RenderWindow",
		width(), height(), false, &params);
	mOgreWindow->setActive(true);
	makeCurrent();
#endif

	/*setAttribute( Qt::WA_PaintOnScreen, true );
	setAttribute( Qt::WA_NoBackground );*/

	//== Ogre Initialization ==//
	Ogre::SceneType scene_manager_type = Ogre::ST_EXTERIOR_CLOSE;

	mSceneMgr = mOgreRoot->createSceneManager( scene_manager_type );
	mSceneMgr->setAmbientLight( Ogre::ColourValue(1,1,1) );

	mCamera = mSceneMgr->createCamera( "QOgreWidget_Cam" );
	mCamera->setPosition(0,10,20);
	mCamera->lookAt( Ogre::Vector3(0,0,0) );
	mCamera->setNearClipDistance( 1.0 );

	mViewport = mOgreWindow->addViewport( mCamera );
	mViewport->setBackgroundColour( Ogre::ColourValue( 1.0,0,0 ) );
	mOgreWindow->windowMovedOrResized();

	loadResource();

	ent = mSceneMgr->createEntity("1", "Sinbad.mesh");

	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NODO_1");
	node->translate(-6, 0, 0);
	node->attachObject(ent);

	count++;
	obj.push_back(std::make_pair("Sinbad_001", node));

	ent2 = mSceneMgr->createEntity("2", "Sinbad.mesh");

	node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("NODO_2");
	node2->translate(6, 0, 0);
	node2->attachObject(ent2);

	//ent2->setMaterialName("YellowHighlight");

	count++;
	obj.push_back(std::make_pair("Sinbad_002", node2));

	//node2->showBoundingBox(true);
	createStaticGeometry();

	//mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	//// create terrain manager
	//mTerrainMgr = new ET::TerrainManager(mSceneMgr);
	//mTerrainMgr->setLODErrorMargin(2, mCamera->getViewport()->getActualHeight());
	//mTerrainMgr->setUseLODMorphing(true, 0.2, "morphFactor");

	//// create a fresh, mid-high terrain for editing
	//ET::TerrainInfo terrainInfo (129, 129, std::vector<float>(129*129, 0.5f));
	//// set position and size of the terrain
	//terrainInfo.setExtents(Ogre::AxisAlignedBox(0, 0, 0, 1500, 300, 1500));
	//// now render it
	//mTerrainMgr->createTerrain(terrainInfo);

	//// create the splatting manager
	//mSplatMgr = new ET::SplattingManager("ETSplatting", "ET", 128, 128, 3);
	//// specify number of splatting textures we need to handle
	//mSplatMgr->setNumTextures(6);

	//// create a manual lightmap texture
	//Ogre::TexturePtr lightmapTex = Ogre::TextureManager::getSingleton().createManual(
	//	"ETLightmap", "ET", Ogre::TEX_TYPE_2D, 128, 128, 1, Ogre::PF_BYTE_RGB);
	//Ogre::Image lightmap;
	//ET::createTerrainLightmap(terrainInfo, lightmap, 128, 128, Ogre::Vector3(1, -1, 1), Ogre::ColourValue::White,
	//	Ogre::ColourValue(0.3, 0.3, 0.3));
	//lightmapTex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));

	//// load the terrain material and assign it
	//Ogre::MaterialPtr material (Ogre::MaterialManager::getSingleton().getByName("ETTerrainMaterial"));
	//mTerrainMgr->setMaterial(material);

	//mTerrainMgr->destroyTerrain();
	//Ogre::Image image;
	//image.load("ETterrain.png", "ET");
	//ET::TerrainInfo info;
	//ET::loadHeightmapFromImage(info, image);
	//info.setExtents(Ogre::AxisAlignedBox(0, 0, 0, 1500, 300, 1500));
	//mTerrainMgr->createTerrain(info);
	////mTerrainInfo = & mTerrainMgr->getTerrainInfo();

	//// now load the splatting maps
	//for (uint i = 0; i < mSplatMgr->getNumMaps(); ++i)
	//{
	//	image.load("ETcoverage."+Ogre::StringConverter::toString(i)+".png", "ET");
	//	mSplatMgr->loadMapFromImage(i, image);
	//}

	// update the lightmap

	//Ogre::Image lightmap;
	//ET::createTerrainLightmap(*mTerrainInfo, lightmap, 128, 128, Vector3(1, -1, 1), ColourValue(1,1,1),
	//	ColourValue(0.3, 0.3,  0.3));

	//// get our dynamic texture and update its contents
	//TexturePtr tex = TextureManager::getSingleton().getByName("ETLightmap");
	//tex->getBuffer(0, 0)->blitFromMemory(lightmap.getPixelBox(0, 0));
}

/**
* @brief render a frame
* @author Kito Berg-Taylor
*/
void THIS::paintGL()
{
	assert( mOgreWindow );

	if(rotate && g_node)
	{
		g_node->yaw(Ogre::Degree(10));
		rotate = false;
	}
	else
		rotate = false;

	if(scaleUp && g_node)
	{
		g_node->scale(1.1, 1.1, 1.1);
		scaleUp = false;
	}
	else
		scaleUp = false;

	if(scaleDw && g_node)
	{
		g_node->scale(0.9, 0.9, 0.9);
		scaleDw = false;
	}
	else
		scaleDw = false;

	mCamera->yaw(-Ogre::Degree(rotX));
    mCamera->pitch(-Ogre::Degree(rotY));
	if(g_node != nullptr && mCtrlPress)
		g_node->translate(mTranslateX,mTranslateY,0.0f);
	mOgreRoot->renderOneFrame();
}

/**
* @brief resize the GL window
* @author Kito Berg-Taylor
*/
void THIS::resizeGL( int width, int height )
{
	assert( mOgreWindow );

	mOgreWindow->windowMovedOrResized();
}

/**
* @brief choose the right renderer
* @author Kito Berg-Taylor
*/
Ogre::RenderSystem* THIS::chooseRenderer( Ogre::RenderSystemList *renderers )
{
	// It would probably be wise to do something more friendly 
	// that just use the first available renderer
	return *renderers->begin();
}

void OgreWidget::createStaticGeometry()
{
	//Ogre::ManualObject* obj=new Ogre::ManualObject("ManualObject");
	//obj->begin("Side", Ogre::RenderOperation::OperationType::OT_TRIANGLE_LIST);
	//// Front
	//obj->position(5, 0, 0);               obj->textureCoord(1,1);
	//obj->position(5, 5, 0);             obj->textureCoord(1,0);
	//obj->position(0, 0, 0);                 obj->textureCoord(0,1);
	//obj->position(0, 5, 0);               obj->textureCoord(0,0);
	//obj->index(0);
	//obj->index(1);
	//obj->index(2);
	//obj->index(1);
	//obj->index(3);
	//obj->index(2);
	//obj->end();
	//
	//
	//obj->convertToMesh("Mesh");
	//Ogre::Entity* ent = mSceneMgr->createEntity("Entity", "Mesh");
	//ent->setMaterialName("YellowHighlight");

	/////*Ogre::StaticGeometry* sg=mSceneMgr->createStaticGeometry("StaticGeometry");
	////sg->addEntity(ent, Ogre::Vector3(0, 0, 0));
	////sg->build();*/
	//Ogre::SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("NODO_KKK");
	//node->attachObject(ent);

	//

	//Ogre::ManualObject * circle = mSceneMgr->createManualObject("circle_name");
 //
 //   float const radius = 3,
 //               thickness = 0.1, // Of course this must be less than the radius value.
 //               accuracy = 100;
 //
 //   circle->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
 //
 //   unsigned point_index = 0;
 //   for(float theta = 0; theta <= 2 * Ogre::Math::PI; theta += Ogre::Math::PI / accuracy) {
 //       circle->position(radius * cos(theta),
 //                        0,
 //                        radius * sin(theta));
 //       circle->position(radius * cos(theta - Ogre::Math::PI / accuracy),
 //                        0,
 //                        radius * sin(theta - Ogre::Math::PI / accuracy));
 //       circle->position((radius - thickness) * cos(theta - Ogre::Math::PI / accuracy),
 //                        0,
 //                        (radius - thickness) * sin(theta - Ogre::Math::PI / accuracy));
 //       circle->position((radius - thickness) * cos(theta),
 //                        0,
 //                        (radius - thickness) * sin(theta));
 //       // Join the 4 vertices created above to form a quad.
 //       circle->quad(point_index, point_index + 1, point_index + 2, point_index + 3);
 //       point_index += 4;
 //   }
 //
 //   circle->end();

	//circle->convertToMesh("Mesh");

	
	//red_gizmo->m_gt = GizmoType::RED;
	//red_gizmo->m_entity = mSceneMgr->createEntity("RED_ROT_GIZMO", "ring3.mesh");
	//red_gizmo->m_entity->setMaterialName("RedHighlight");

//	red_gizmo->m_node = mSceneMgr->getRootSceneNode()->createChildSceneNode("RED_ROT_GIZMO_NODE");
//	red_gizmo->m_node->attachObject(red_gizmo->m_entity);
//
//	std::cout << "POS RED GIZMO: " << red_gizmo->m_node->getPosition() << std::endl;
//
//	//green_gizmo->m_gt = GizmoType::GREEN;
//	//green_gizmo->m_entity = mSceneMgr->createEntity("GREEN_ROT_GIZMO", "ring3.mesh");
//	//green_gizmo->m_entity->setMaterialName("GreenHighlight");
//
//
//	green_gizmo->m_node = mSceneMgr->getRootSceneNode()->createChildSceneNode("GREEN_ROT_GIZMO_NODE");
//	green_gizmo->m_node->attachObject(green_gizmo->m_entity);
//	green_gizmo->m_node->pitch(Ogre::Radian(Ogre::Math::PI / 2));
//
//	//green_gizmo->m_node->roll(Ogre::Degree(-45));
//
//	//blue_gizmo->m_gt = GizmoType::BLUE;
//	//blue_gizmo->m_entity = mSceneMgr->createEntity("BLUE_ROT_GIZMO", "ring3.mesh");
//	//blue_gizmo->m_entity->setMaterialName("BlueHighlight");
//
//	blue_gizmo->m_node = mSceneMgr->getRootSceneNode()->createChildSceneNode("BLUE_ROT_GIZMO_NODE");
//	blue_gizmo->m_node->attachObject(blue_gizmo->m_entity);
//	blue_gizmo->m_node->roll(Ogre::Radian(Ogre::Math::PI / 2));
//	//blue_gizmo->m_node->roll(Ogre::Degree(45));
//
///*
//	ent = mSceneMgr->createEntity("GREEN_ROT_GIZMO", "ring3.mesh");
//	ent->setMaterialName("GreenHighlight");
//
//	Ogre::SceneNode *n = mSceneMgr->getRootSceneNode()->createChildSceneNode("GREEN_ROT_GIZMO_NODE");
//	n->attachObject(ent);
//	n->pitch(Ogre::Radian(Ogre::Math::PI / 2));
//	n->roll(Ogre::Degree(-45));
//
//	ent = mSceneMgr->createEntity("BLUE_ROT_GIZMO", "ring3.mesh");
//	ent->setMaterialName("BlueHighlight");
//
//	Ogre::SceneNode *n2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("BLUE_ROT_GIZMO_NODE");
//	n2->attachObject(ent);
//	n2->pitch(Ogre::Radian(Ogre::Math::PI / 2));
//	n2->roll(Ogre::Degree(45));
//
//*/
//
//	
//

	/*count++;
	this->obj.push_back(std::make_pair("Plane_001", node));*/
}