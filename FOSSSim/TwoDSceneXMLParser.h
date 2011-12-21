#ifndef __TWO_D_SCENE_XML_PARSER_H__
#define __TWO_D_SCENE_XML_PARSER_H__

#include <Eigen/StdVector>

#include <iostream>
#include <fstream>
#include <limits>

#include "rapidxml.hpp"

#include "TwoDScene.h"

#include "ExplicitEuler.h"
#include "SymplecticEuler.h"
#include "LinearizedImplicitEuler.h"

#include "SpringForce.h"
#include "GravitationalForce.h"
#include "SimpleGravityForce.h"
#include "DragDampingForce.h"
#include "VortexForce.h"

#include "StringUtilities.h"
#include "RenderingUtilities.h"

#include "TwoDimensionalDisplayController.h"

// TODO: Write method for computing number of a given property
// TODO: Add some additional error checking for repeat properties, etc
// TODO: Abstract out common code
// TODO: Check for invalid properties

class TwoDSceneXMLParser
{
public:
  void loadSceneFromXML( const std::string& filename, TwoDScene& twodscene, SceneStepper** scenestepper, scalar& dt, scalar& max_t, scalar& maxfreq, std::vector<renderingutils::Color>& particle_colors, std::vector<renderingutils::Color>& edge_colors, std::vector<renderingutils::ParticlePath>& particle_paths, renderingutils::Color& bgcolor, std::string& description, std::string& scenetag, TwoDimensionalDisplayController& controller);

private:
  void loadXMLFile( const std::string& filename, std::vector<char>& xmlchars, rapidxml::xml_document<>& doc );

  bool loadTextFileIntoString( const std::string& filename, std::string& filecontents );

  void loadCameraLocation( rapidxml::xml_node<>*, TwoDimensionalDisplayController&);
  
  void loadParticles( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadSceneTag( rapidxml::xml_node<>* node, std::string& scenetag );
  
  void loadEdges( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadSpringForces( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadGravitationalForces( rapidxml::xml_node<>* node, TwoDScene& twodscene );
  
  void loadSimpleGravityForces( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadDragDampingForces( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadVorexForces( rapidxml::xml_node<>* node, TwoDScene& twodscene );

  void loadIntegrator( rapidxml::xml_node<>* node, SceneStepper** scenestepper, scalar& dt );
  
  void loadMaxTime( rapidxml::xml_node<>* node, scalar& max_t );
  
  void loadMaxSimFrequency( rapidxml::xml_node<>* node, scalar& max_freq );
  
  void loadBackgroundColor( rapidxml::xml_node<>* node, renderingutils::Color& color );
  
  void loadParticleColors( rapidxml::xml_node<>* node, std::vector<renderingutils::Color>& particle_colors );

  void loadEdgeColors( rapidxml::xml_node<>* node, std::vector<renderingutils::Color>& edge_colors );
  
  void loadParticlePaths( rapidxml::xml_node<>* node, double dt, std::vector<renderingutils::ParticlePath>& particle_paths );
  
  void loadSceneDescriptionString( rapidxml::xml_node<>* node, std::string& description_string );
};

#endif
