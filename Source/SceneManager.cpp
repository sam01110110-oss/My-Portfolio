///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadTorusMesh();
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();

	// Load scene textures
	CreateGLTexture("Debug/Textures/links_rug.jpg", "rug");
	CreateGLTexture("Debug/Textures/links_floor.jpg", "floor");
	CreateGLTexture("Debug/Textures/light.jpg", "light");
	CreateGLTexture("Debug/Textures/brass.jpg", "brass");
	CreateGLTexture("Debug/Textures/links_wall.jpg", "wall");
	CreateGLTexture("Debug/Textures/rustic_wood.jpg", "rustic");
	CreateGLTexture("Debug/Textures/rustic_wood_2.jpg", "rustic2");
	CreateGLTexture("Debug/Textures/rustic_wood_3.jpg", "rustic3");
	CreateGLTexture("Debug/Textures/clay_0.jpg", "clay0");
	CreateGLTexture("Debug/Textures/clay_1.jpg", "clay1");
	CreateGLTexture("Debug/Textures/petal.jpg", "petal");
	CreateGLTexture("Debug/Textures/stem.jpg", "stem");
	CreateGLTexture("Debug/Textures/bedding_0.jpg", "bedding");
	CreateGLTexture("Debug/Textures/bedding_1.jpg", "blanket");
	CreateGLTexture("Debug/Textures/pot_texture.jpg", "pot");
	CreateGLTexture("Debug/Textures/cloth_texture.jpg", "cloth");

	// Bind textures to texture slots
	BindGLTextures();

	// Load Lights
	SetupSceneLights();

	// Load Materials
	DefineObjectMaterials();
}

/***********************************************************
 *  DefineObjectMaterials()
 *
 *  This method is used for configuring the various material
 *  settings for all of the objects within the 3D scene.
 ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	/*** STUDENTS - add the code BELOW for defining object materials. ***/
	/*** There is no limit to the number of object materials that can ***/
	/*** be defined. Refer to the code in the OpenGL Sample for help  ***/

	// Define Wood Material
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.0f, 0.0f, 0.0f);
	woodMaterial.ambientStrength = 0.2f;
	woodMaterial.diffuseColor = glm::vec3(0.15f, 0.14f, 0.14f);
	woodMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	woodMaterial.shininess = 0.3;
	woodMaterial.tag = "wood";

	m_objectMaterials.push_back(woodMaterial);

	// Define Neutral Material
	OBJECT_MATERIAL neutralMaterial;
	neutralMaterial.ambientColor = glm::vec3(0.0f, 0.0f, 0.0f);
	neutralMaterial.ambientStrength = 0.2f;
	neutralMaterial.diffuseColor = glm::vec3(0.1f, 0.1f, 0.1f);
	neutralMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	neutralMaterial.shininess = 0.3;
	neutralMaterial.tag = "neutral";

	m_objectMaterials.push_back(neutralMaterial);

	// Define Clay Material
	OBJECT_MATERIAL clayMaterial;
	clayMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
	clayMaterial.ambientStrength = 0.3f;
	clayMaterial.diffuseColor = glm::vec3(0.4f, 0.4f, 0.5f);
	clayMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.4f);
	clayMaterial.shininess = 2.4;
	clayMaterial.tag = "clay";

	m_objectMaterials.push_back(clayMaterial);

	// Define Glass Material
	OBJECT_MATERIAL glassMaterial;
	glassMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
	glassMaterial.ambientStrength = 0.3f;
	glassMaterial.diffuseColor = glm::vec3(1.5f, 1.5f, 1.5f);
	glassMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.4f);
	glassMaterial.shininess = 3.0;
	glassMaterial.tag = "glass";

	m_objectMaterials.push_back(glassMaterial);

	// Define Metal Material
	OBJECT_MATERIAL metalMaterial;
	metalMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.3f);
	metalMaterial.ambientStrength = 0.3f;
	metalMaterial.diffuseColor = glm::vec3(0.4f, 0.4f, 0.5f);
	metalMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.4f);
	metalMaterial.shininess = 0.9;
	metalMaterial.tag = "metal";

	m_objectMaterials.push_back(metalMaterial);
}

/*********************************************************
*  Lighting
* 
* 
* 
***********************************************************/
void SceneManager::SetupSceneLights()
{


	/*********************************************************
	*  Enable Lighting										 *
	*********************************************************/
	m_pShaderManager->setIntValue("bUseLighting", true);


	/*********************************************************
	*  LIGHT 0 - Lantern Light								 *
	*********************************************************/
	m_pShaderManager->setVec3Value("lightSources[0].position", 0.0f, 9.0f, -8.5f);

	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.30f, 0.24f, 0.14f);

	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 1.15f, 0.95f, 0.65f);

	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 1.0f, 0.9f, 0.7f);

	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 24.0f);

	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 1.0f);


	
	/*********************************************************
	*  LIGHT 1 - Secondary Fill Light						 *
	*********************************************************/
	m_pShaderManager->setVec3Value("lightSources[1].position", 0.0f, 15.0f, 10.0f);

	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.05f, 0.05f, 0.05f);

	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.15f, 0.15f, 0.18f);

	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.1f, 0.1f, 0.1f);

	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 8.0f);

	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.15f);


	/*********************************************************
	*  LIGHT 2 - Bounce Fill								 *
	*********************************************************/
	m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 10.0f, 20.0f);

	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.03f, 0.03f, 0.03f);

	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.05f, 0.05f, 0.05f);

	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.05f, 0.05f, 0.05f);

	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 4.0f);

	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.05f);


	/*********************************************************
	*  LIGHT 3 - Ambience									 *
	*********************************************************/
	m_pShaderManager->setVec3Value("lightSources[3].position", 20.0f, 10.0f, 0.0f);

	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.02f, 0.02f, 0.02f);

	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.03f, 0.03f, 0.03f);

	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.05f, 0.05f, 0.05f);

	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 4.0f);

	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.05f);


}




/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// declare the variables for the transformations
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	/*** Set needed transformations before drawing the basic mesh.  ***/
	/*** This same ordering of code should be used for transforming ***/
	/*** and drawing all the basic 3D shapes.						***/
	/******************************************************************/



	/***********************************************************
	*  Floor												   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(25.0f, 1.0f, 40.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.0f, -10.0f, 0.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("floor");

	// set UV scale
	SetTextureUVScale(8.0f, 8.0f);

	// set material
	SetShaderMaterial("wood");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();



	/***********************************************************
	*  Back Wall											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(20.0f, 1.0f, 15.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 5.0f, -10.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("wall");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawPlaneMesh();



	/***********************************************************
	*  Side Wall											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(15.0f, 1.0f, 40.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(20.0f, 5.0f, 0.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("wall");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawPlaneMesh();



	/***********************************************************
	*  LANTERN OBJECT GROUP									   *
	***********************************************************/

	/***********************************************************
	*  Lantern Shade/Hood							   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 0.5f, 2.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 2.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 11.5f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawSphereMesh();



	/***********************************************************
	*  Lantern Base - Lower									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.1f, 0.3f, 1.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 8.8f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Lantern Base - Upper									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.1f, 0.3f, 1.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 9.4f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Inner Light								               *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 2.5f, 0.6f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 9.0f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("light");

	// set UV scale
	SetTextureUVScale(4.0f, 4.0f);

	// set material
	SetShaderMaterial("glass");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Handle Support - Right							       *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.6f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.0f, 10.9f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle Support - Left								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.6f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-1.0f, 10.9f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle Bar											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 2.3f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.15f, 13.0f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Nail Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.25f, 2.3f, 0.25f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 12.7f, -9.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Nail Part B											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.35f, 1.0f, 0.35f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 90.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(0.0f, 12.7f, -9.6f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("brass");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("metal");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();











	/***********************************************************
	*  Rug OBJECT 											   *
	***********************************************************/

	/***********************************************************
	*  Rug													   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(4.0f, 1.0f, 5.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-1.0f, -9.9f, 4.0f);

	// set the transformations into memory to be used on the drawn meshes
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rug");
	
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw the mesh with transformation values
	m_basicMeshes->DrawPlaneMesh();













	/***********************************************************
	*  Nightstand OBJECT GROUP								   *
	***********************************************************/

	/***********************************************************
	*  Support Left											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 11.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-6.7f, -9.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Support Right										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 11.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(1.2f, -9.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Top													   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(9.7f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -3.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Top Drawer Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -4.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Top Drawer Part B									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 1.4f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -5.2f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle												   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -4.4f, -7.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Bottom Drawer Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -6.3f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Bottom Drawer Part B									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 1.4f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -7.4f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle												   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -6.7f, -7.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Bottom Support										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(7.0f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -8.6f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Cloth Under Vase										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(2.2f, 1.0f, 1.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -2.49f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("cloth");

	// set UV scale
	SetTextureUVScale(0.5f, 3.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawPlaneMesh();










	/***********************************************************
	*  Vase OBJECT GROUP									   *
	***********************************************************/

	/***********************************************************
	*  Vase Bottom Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.85f, 1.2f, 0.85f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 180.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -2.2f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("clay0");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawConeMesh();


	
	/***********************************************************
	*  Vase Bottom Part B									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.92f, 2.0f, 0.92f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -2.2f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("clay1");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawConeMesh();


	/***********************************************************
	*  Vase Stem											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.4f, 1.8f, 0.4f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -1.8f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("clay1");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Vase Opening/Lip										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.42f, 0.42f, 0.42f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 0.07f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("clay1");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawTorusMesh();













	/***********************************************************
	*  Flower Object										   *
	***********************************************************/

	/***********************************************************
	*  Dirt													   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.4f, 0.02f, 0.4f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 0.09f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	SetShaderColor(0.20f, 0.129f, 0.110f, 1.0f);

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Flower Stem											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.1f, 3.6f, 0.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, -1.8f, -8.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("stem");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Flower Petal - 1										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -55.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();



	/***********************************************************
	*  Flower Petal - 2										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 75.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();



	/***********************************************************
	*  Flower Petal - 3										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 125.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();



	/***********************************************************
	*  Flower Petal - 4										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -105.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();



	/***********************************************************
	*  Flower Petal - 5										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 10.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();



	/***********************************************************
	*  Flower Petal - 6										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.6f, 1.5f, 0.08f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -5.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 190.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-2.7f, 1.8f, -8.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("petal");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawConeMesh();


	









	/***********************************************************
	*  Stool OBJECT GROUP									   *
	***********************************************************/

	/***********************************************************
	*  Stool Seat											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(2.2f, 1.0f, 2.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-11.5f, -7.2f, -7.3f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Front Left Leg								     	   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -10.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -10.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-13.4f, -11.2f, -5.6f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Back Left Leg										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 10.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -10.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-13.4f, -11.2f, -8.6f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Front Right Leg										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -10.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 10.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-9.6f, -11.2f, -5.6);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Back Right Leg										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.3f, 4.5f, 0.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 10.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 10.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(-9.6f, -11.2f, -8.6f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();














	/***********************************************************
	*  Bed OBJECT GROUP										   *
	***********************************************************/

	/***********************************************************
	*  Back Left Leg/Pillar									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 15.0f, 1.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 1.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.15f, -9.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Back Right Leg/Pillar								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 15.0f, 1.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -1.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.65f, -9.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Front Left Leg/Pillar								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 6.2f, 1.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 1.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.15f, -9.0f, 12.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Front Right Leg/Pillar								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.5f, 6.2f, 1.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -1.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.65f, -9.0f, 12.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  W-Frame Part A										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.3f, 2.5f, 1.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 87.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(7.0f, -2.5f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  W-Frame Part B										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.3f, 4.0f, 1.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 96.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(10.1f, -2.4f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  W-Frame Part C										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.3f, 4.0f, 1.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 84.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(13.7f, -2.4f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  W-Frame Part D										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.3f, 2.5f, 1.3f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 93.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(16.8f, -2.5f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Boxspring Frame Part A								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.1f, 23.0f, 2.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.15f, -8.0f, 0.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Boxspring Frame Part B							       *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.1f, 23.0f, 2.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 90.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.65f, -8.0f, 0.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Boxspring Frame Part C								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(14.0f, 2.5f, 1.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.65f, -8.0f, 12.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Boxspring Frame Part D								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(14.0f, 2.5f, 1.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.65f, -8.0f, -9.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic3");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Mattress												   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(12.8f, 4.5f, 19.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.85f, -7.5f, 1.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("bedding");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Blanket	Part A										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(13.0f, 0.65f, 16.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.75f, -5.0f, 3.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);
	

	// set texture
	SetShaderTexture("blanket");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Blanket Part B										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.5f, 0.5f, 16.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.35f;
	ZrotationDegrees = 84.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(5.3f, -6.5f, 3.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);


	// set texture
	SetShaderTexture("blanket");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Blanket Part C										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.5f, 3.5f, 12.95f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = -0.6f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.85f, -6.5f, 11.05f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);


	// set texture
	SetShaderTexture("blanket");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Blanket Part D										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.5f, 0.5f, 16.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -0.5f;
	ZrotationDegrees = 96.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.3f, -6.5f, 3.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);


	// set texture
	SetShaderTexture("blanket");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Pillow												   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(6.25f, 0.75f, 2.75f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(11.85f, -5.0f, -6.7f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("bedding");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("neutral");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();










	/***********************************************************
	*  Nightstand OBJECT GROUP								   *
	***********************************************************/

	/***********************************************************
	*  Support Left											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 11.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -9.0f, 15.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Drawer Partition	1									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 2.3f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.5f, 19.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Drawer Partition	2									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 2.3f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.5f, 28.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Support Middle										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 11.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -9.0f, 24.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Support Right										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 11.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -9.0f, 33.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Top													   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(19.4f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -3.0f, 24.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();


	
	/***********************************************************
	*  Bottom Support										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(18.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -8.6f, 24.1f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();


	/***********************************************************
	*  Drawer Part A										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(18.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -6.6f, 24.1f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();
	


	/***********************************************************
	*  Drawer Green Piece 1									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.6f, 17.3f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Drawer Green Piece 2								       *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.6f, 21.7f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Drawer Green Piece 3								       *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.6f, 26.2f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Drawer Green Piece 4								       *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(3.8f, 1.0f, 4.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -7.6f, 30.7f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle 1 											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(15.75f, -7.2f, 17.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();


	/***********************************************************
	*  Handle 2 											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = -17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(15.75f, -7.2f, 21.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle 3 											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = 17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(15.75f, -7.2f, 26.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Handle 4 											   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.65f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 90.0f;
	ZrotationDegrees = -17.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(15.75f, -7.2f, 30.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("rustic");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Cloth Under Photos									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(1.0f, 1.0f, 8.5f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -2.49f, 24.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("cloth");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawPlaneMesh();



	/***********************************************************
	*  Picture 1 OBJECT GROUP								   *
	***********************************************************/

	/***********************************************************
	*  Picture Frame Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.6f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.7f, -2.4f, 19.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part B									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.62f, -2.0f, 18.8f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part C									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.0f, -2.0f, 20.2f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part D									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.63f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.95f, -1.5f, 19.47f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Photo										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.05f, 1.0f, 1.62f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.85f, -2.0f, 19.48f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set shader color
	SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// draw mesh
	m_basicMeshes->DrawBoxMesh();














	/***********************************************************
	*  Picture Frame 2 OBJECT GROUP							   *
	***********************************************************/

	/***********************************************************
	*  Picture Frame Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.69f, -2.42f, 22.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part B									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.8f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -2.0f, 22.1f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part C									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.8f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -2.0f, 22.9f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part D									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.0f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.01f, -1.2f, 22.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Photo										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.05f, 1.8f, 0.95f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.8f, -2.0f, 22.5f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set shader color
	SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// draw mesh
	m_basicMeshes->DrawBoxMesh();









	/***********************************************************
	*  Small Pot OBJECT GROUP								   *
	***********************************************************/

	/***********************************************************
	*  Small Pot Opening									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.1f, 0.5f, 0.1f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.1f, -1.9f, 26.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("pot");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();



	/***********************************************************
	*  Small Pot Bottom Part A								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.82f, 0.35f, 0.82f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.1f, -1.9f, 26.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("pot");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawSphereMesh();



	/***********************************************************
	*  Small Pot Bottom Part B								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.75f, 0.25f, 0.75f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.1f, -2.2f, 26.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("pot");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();


	/***********************************************************
	*  Small Pot Bottom Part C								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.70f, 0.25f, 0.70f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.1f, -2.35f, 26.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("pot");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();


	/***********************************************************
	*  Small Pot Bottom Part D								   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.70f, 0.25f, 0.70f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = 0.0f;
	ZrotationDegrees = 0.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.1f, -2.5f, 26.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// set texture
	SetShaderTexture("pot");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("clay");

	// draw mesh
	m_basicMeshes->DrawCylinderMesh();

















	/***********************************************************
	*  Picture Frame 3 OBJECT GROUP							   *
	***********************************************************/

	/***********************************************************
	*  Picture Frame Part A									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.6f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.7f, -2.4f, 29.98f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part B								   	   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.62f, -2.0f, 30.68f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part C									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 1.0f, 0.2f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(18.0f, -2.0f, 29.25f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Frame Part D									   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.2f, 0.2f, 1.67f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.95f, -1.5f, 30.0f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);

	// Set texture
	SetShaderTexture("rustic2");

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// set material
	SetShaderMaterial("wood");

	// draw mesh
	m_basicMeshes->DrawBoxMesh();



	/***********************************************************
	*  Picture Photo										   *
	***********************************************************/

	// set the XYZ scale for the mesh
	scaleXYZ = glm::vec3(0.05f, 1.0f, 1.62f);

	// set the XYZ rotation for the mesh
	XrotationDegrees = 0.0f;
	YrotationDegrees = -15.0f;
	ZrotationDegrees = -15.0f;

	// set the XYZ position for the mesh
	positionXYZ = glm::vec3(17.85f, -2.0f, 29.95f);

	// set transformations into memory
	SetTransformations(
		scaleXYZ,
		XrotationDegrees,
		YrotationDegrees,
		ZrotationDegrees,
		positionXYZ);


	// set shader color
	SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

	// set UV scale
	SetTextureUVScale(1.0f, 1.0f);

	// draw mesh
	m_basicMeshes->DrawBoxMesh();


}
