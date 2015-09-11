#include <Eigen/StdVector>
#include <cmath>
#include <iostream>
#include <fstream>

#include <tclap/CmdLine.h>

#include "TwoDScene.h"
#include "Force.h"
#include "SpringForce.h"
#include "ExplicitEuler.h"
#include "TwoDimensionalDisplayController.h"
#include "TwoDSceneRenderer.h"
#include "TwoDSceneXMLParser.h"
#include "TwoDSceneSerializer.h"
#include "StringUtilities.h"
#include "MathDefs.h"
#include "TimingUtilities.h"
#include "RenderingUtilities.h"

///////////////////////////////////////////////////////////////////////////////
// Rendering State
TwoDimensionalDisplayController g_display_controller(512,512);
TwoDSceneRenderer* g_scene_renderer = NULL;
renderingutils::Color g_bgcolor(0.0,0.0,0.0);
bool g_rendering_enabled = true;

double g_sec_per_frame;
double g_last_time = timingutils::seconds();

///////////////////////////////////////////////////////////////////////////////
// Parser state
std::string g_xml_scene_file;
TwoDSceneXMLParser g_xml_scene_parser;
std::string g_description;

///////////////////////////////////////////////////////////////////////////////
// Scene input/output/comparison state
TwoDSceneSerializer g_scene_serializer;

bool g_save_to_binary = false;
std::string g_binary_file_name;
std::ofstream g_binary_output;

///////////////////////////////////////////////////////////////////////////////
// Simulation state
bool g_paused = true;
scalar g_dt = 0.0;
int g_num_steps = 0;
int g_current_step = 0;
TwoDScene g_scene;
SceneStepper* g_scene_stepper = NULL;
bool g_simulation_ran_to_completion = false;


std::string g_scene_tag = "";


///////////////////////////////////////////////////////////////////////////////
// Simulation functions

void miscOutputCallback();
void sceneScriptingCallback();

void stepSystem()
{
  assert( g_scene_stepper != NULL );
  
  // Determine if the simulation is complete
  if( g_current_step >= g_num_steps )
  {
    std::cout << std::endl;
    std::cout << outputmod::startpink << "FOSSSim message: " << outputmod::endpink << "Simulation complete at time " << g_current_step*g_dt << ". Exiting." << std::endl;
    g_simulation_ran_to_completion = true;
    exit(0);
  }

  // Step the simulated scene forward
  g_scene_stepper->stepScene( g_scene, g_dt );
  
  // Check for obvious problems in the simulated scene
  #ifdef DEBUG
    g_scene.checkConsistency();
  #endif

  //g_t += g_dt;
  g_current_step++;
  
  // If saving the simulation output, do it!
  if( g_save_to_binary ) g_scene_serializer.serializeScene( g_scene, g_binary_output );
  
  sceneScriptingCallback();
  miscOutputCallback();
}

void headlessSimLoop()
{
  scalar nextpercent = 0.02;
  std::cout << outputmod::startpink << "Progress: " << outputmod::endpink;
  for( int i = 0; i < 50; ++i ) std::cout << "-";
  //std::cout << "]" << std::endl;
  std::cout << std::endl;
  std::cout << "          ";
  while( true )
  {
    scalar percent_done = ((double)g_current_step)/((double)g_num_steps);
    if( percent_done >= nextpercent )
    {
      nextpercent += 0.02;
      std::cout << "." << std::flush;
    }
    stepSystem();
  }
}




///////////////////////////////////////////////////////////////////////////////
// Rendering and UI functions

void reshape( int w, int h ) {
  g_display_controller.reshape(w,h);
  assert( renderingutils::checkGLErrors() );
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glLoadIdentity();

  g_display_controller.updateViewMatrix();
  g_scene_renderer->renderScene();

  glPopMatrix();
  glutSwapBuffers();
  assert( renderingutils::checkGLErrors() );
}

void keyboard( unsigned char key, int x, int y )
{
  g_display_controller.keyboard(key,x,y);

  if( key == 27 || key == 'q' )
  {
    exit(0);
  }
  else if( key == 's' || key =='S' )
  {
    stepSystem();
    g_scene_renderer->updateState();
    glutPostRedisplay();
  }
  else if( key == ' ' )
  {
    g_paused = !g_paused;
  }

  assert( renderingutils::checkGLErrors() );
}

// Proccess 'special' keys
void special( int key, int x, int y )
{
  g_display_controller.special(key,x,y);
  
  assert( renderingutils::checkGLErrors() );
}

void mouse( int button, int state, int x, int y )
{
  g_display_controller.mouse(button,state,x,y);
  
  assert( renderingutils::checkGLErrors() );
}

void motion( int x, int y ) 
{
  g_display_controller.motion(x,y);
  
  assert( renderingutils::checkGLErrors() );
}

void idle()
{
  //std::cout << "g_last_time: " << g_last_time << std::endl;
  // Trigger the next timestep
  double current_time = timingutils::seconds();
  //std::cout << "current_time: " << current_time << std::endl;
  //std::cout << "g_sec_per_frame: " << g_sec_per_frame << std::endl;
  if( !g_paused && current_time-g_last_time >= g_sec_per_frame ) 
  {
    g_last_time = current_time;
    stepSystem();
    g_scene_renderer->updateState();
    glutPostRedisplay();
  }
  
  assert( renderingutils::checkGLErrors() );
}

void initializeOpenGLandGLUT( int argc, char** argv )
{
  // Initialize GLUT
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
  glutInitWindowSize(g_display_controller.getWindowWidth(),g_display_controller.getWindowHeight());
  glutCreateWindow("Forty One Sixty Seven Sim");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  // Initialize light 0
  glEnable(GL_LIGHT0);
  GLfloat arr1[3] = {.2, .2, .2};
  GLfloat arr2[3] = {1., 1., 1.};
  GLfloat arr3[3] = {1, 1, 1};
  GLfloat arr4[3] = {10., 10., 10.};
  glLightfv(GL_LIGHT0, GL_AMBIENT, arr1);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, arr2);
  glLightfv(GL_LIGHT0, GL_SPECULAR, arr3);
  glLightfv(GL_LIGHT0, GL_POSITION, arr4);

  
  // Initialize OpenGL
	reshape(
      g_display_controller.getWindowWidth(),
      g_display_controller.getWindowHeight());
  glClearColor(g_bgcolor.r, g_bgcolor.g, g_bgcolor.b, 0.0);
  
  assert( renderingutils::checkGLErrors() );
}


///////////////////////////////////////////////////////////////////////////////
// Parser functions

void loadScene( const std::string& file_name )
{
  assert( g_scene_stepper == NULL );
  
  // TODO: Just pass scene renderer to xml scene parser
  scalar max_time;
  std::vector<renderingutils::Color> particle_colors;
  std::vector<renderingutils::Color> edge_colors;
  std::vector<renderingutils::ParticlePath> particle_paths;
  scalar steps_per_sec_cap = 100;
  g_xml_scene_parser.loadSceneFromXML( file_name, g_scene, &g_scene_stepper, g_dt, max_time, steps_per_sec_cap, particle_colors, edge_colors, particle_paths, g_bgcolor, g_description, g_scene_tag, g_display_controller );
    
  g_sec_per_frame = 1.0/steps_per_sec_cap;
  
  if( g_rendering_enabled )
  {
    g_scene_renderer = new TwoDSceneRenderer(g_scene,particle_colors,edge_colors,particle_paths);
    g_scene_renderer->updateState();
  }

  g_num_steps = ceil(max_time/g_dt);
  g_current_step = 0;
  
  assert( g_scene_stepper != NULL );
  assert( g_dt > 0.0 );
}

void parseCommandLine( int argc, char** argv )
{
  try 
  {
    TCLAP::CmdLine cmd("Forty One Sixty Seven Sim");
    
    // XML scene file to load
    TCLAP::ValueArg<std::string> scene("s", "scene", "Simulation to run; an xml scene file", true, "", "string", cmd);
    
    // Begin the scene paused or running
    TCLAP::ValueArg<bool> paused("p", "paused", "Begin the simulation paused if 1, running if 0", false, true, "boolean", cmd);
    
    // Run the simulation with rendering enabled or disabled
    TCLAP::ValueArg<bool> display("d", "display", "Run the simulation with display enabled if 1, without if 0", false, true, "boolean", cmd);
    
    // These cannot be set at the same time
    // File to save output to
    TCLAP::ValueArg<std::string> output("o", "outputfile", "Binary file to save simulation state to", false, "", "string", cmd);
    
    cmd.parse(argc, argv);

    assert( scene.isSet() );
    g_xml_scene_file = scene.getValue();
    g_paused = paused.getValue();
    g_rendering_enabled = display.getValue();
    
    if( output.isSet() )
    {
      g_save_to_binary = true;
      g_binary_file_name = output.getValue();
    }
  } 
  catch (TCLAP::ArgException& e) 
  {
    std::cerr << "error: " << e.what() << std::endl;
    exit(1);
  }
}



///////////////////////////////////////////////////////////////////////////////
// Various support functions

void miscOutputFinalization();

void cleanupAtExit()
{
  if( g_scene_renderer != NULL )
  {
    delete g_scene_renderer;
    g_scene_renderer = NULL;
  }
  
  if( g_scene_stepper != NULL )
  {
    delete g_scene_stepper;
    g_scene_stepper = NULL;
  }
  
  if( g_binary_output.is_open() )
  {
    std::cout << outputmod::startpink << "FOSSSim message: " << outputmod::endpink << "Saved simulation to file." << std::endl;
    g_binary_output.close();
  }

  miscOutputFinalization();
}

std::ostream& fosssim_header( std::ostream& stream )
{
  stream << outputmod::startgreen << 
  "------------------------------------------    " << std::endl <<
  "  _____ ___  ____ ____ ____  _                " << std::endl <<
  " |  ___/ _ \\/ ___/ ___/ ___|(_)_ __ ___      " << std::endl <<
  " | |_ | | | \\___ \\___ \\___ \\| | '_ ` _ \\ " << std::endl <<
  " |  _|| |_| |___) |__) |__) | | | | | | |     " << std::endl << 
  " |_|   \\___/|____/____/____/|_|_| |_| |_|    " << std::endl <<
  "------------------------------------------    " 
  << outputmod::endgreen << std::endl;
  
  return stream;
}

std::ofstream g_debugoutput;

void miscOutputInitialization()
{
  //g_debugoutput.open("debugoutput.txt");
  //g_debugoutput << "# Time   PotentialEnergy   KineticEnergy   TotalEnergy" << std::endl;
  //g_debugoutput << g_current_step*g_dt << "\t" << g_scene.computePotentialEnergy() << "\t" << g_scene.computeKineticEnergy() << "\t" << g_scene.computeTotalEnergy() << std::endl;
}

void miscOutputCallback()
{
  //g_debugoutput << g_current_step*g_dt << "\t" << g_scene.computePotentialEnergy() << "\t" << g_scene.computeKineticEnergy() << "\t" << g_scene.computeTotalEnergy() << std::endl;
}

void miscOutputFinalization()
{
  //g_debugoutput.close();
}

// Called at the end of each timestep. Intended for adding effects to creative scenes.
void sceneScriptingCallback()
{
  // If the scene is one we wish to 'script'
  if( g_scene_tag == "ParticleFountain" )
  {
    // Get the particle tags
    const std::vector<std::string>& tags = g_scene.getParticleTags();
    // Get the particle positions
    VectorXs& x = g_scene.getX();
    // Get the particle velocities
    VectorXs& v = g_scene.getV();
    // Get the particle colors
    std::vector<renderingutils::Color>& pcolors = g_scene_renderer->getParticleColors();

    // If any particles are tagged for teleportation and fall below -1.25
    for( std::vector<std::string>::size_type i = 0; i < tags.size(); ++i ) if( tags[i] == "teleport" && x(2*i+1) < -1.25 )
    {
      // Return this particle to the origin
      x.segment<2>(2*i).setZero();
      // Give this particle some random upward velocity
      double vx = 0.2*(((double)rand())/((double)RAND_MAX)-0.5);
      double vy = 0.15*((double)rand())/((double)RAND_MAX);
      v.segment<2>(2*i) << vx, vy;
      // Assign the particle a random color
      pcolors[i].r = ((double)rand())/((double)RAND_MAX);
      pcolors[i].g = ((double)rand())/((double)RAND_MAX);
      pcolors[i].b = ((double)rand())/((double)RAND_MAX);
    }
  }
}

int main( int argc, char** argv )
{  
  // Parse command line arguments
  parseCommandLine( argc, argv );

  // Function to cleanup at progarm exit
  atexit(cleanupAtExit);

  // Load the user-specified scene
  loadScene(g_xml_scene_file);

  // If requested, open the binary output file
  if( g_save_to_binary )
  {
    g_binary_output.open(g_binary_file_name.c_str());
    if( g_binary_output.fail() ) 
    {
      std::cerr << outputmod::startred << "ERROR IN INITIALIZATION: "  << outputmod::endred << "Failed to open binary output file: " << " `" << g_binary_file_name << "`   Exiting." << std::endl;
      exit(1);
    }
    // Save the initial conditions
    g_scene_serializer.serializeScene( g_scene, g_binary_output );
  }

  // Initialization for OpenGL and GLUT
  if( g_rendering_enabled ) initializeOpenGLandGLUT(argc,argv);

  // Print a header
  std::cout << fosssim_header << std::endl;
    
  // Print some status info about this FOSSSim build
  #ifdef FOSSSIM_VERSION
    std::cout << outputmod::startblue << "FOSSSim Version: "  << outputmod::endblue << FOSSSIM_VERSION << std::endl;
  #endif
  #ifdef CMAKE_BUILD_TYPE
    std::cout << outputmod::startblue << "Build type: " << outputmod::endblue << CMAKE_BUILD_TYPE << std::endl;
  #endif
  #ifdef EIGEN_VECTORIZE
    std::cout << outputmod::startblue << "Vectorization: " << outputmod::endblue << "Enabled" << std::endl;
  #else
    std::cout << outputmod::startblue << "Vectorization: " << outputmod::endblue << "Disabled" << std::endl;
  #endif
  
  std::cout << outputmod::startblue << "Scene: " << outputmod::endblue << g_xml_scene_file << std::endl;
  std::cout << outputmod::startblue << "Integrator: " << outputmod::endblue << g_scene_stepper->getName() << std::endl;
  std::cout << outputmod::startblue << "Description: " << outputmod::endblue << g_description << std::endl;
  
  if( g_save_to_binary ) std::cout << outputmod::startpink << "FOSSSim message: "  << outputmod::endpink << "Saving simulation to: " << g_binary_file_name << std::endl;
  
  miscOutputInitialization();
  
  if( g_rendering_enabled ) glutMainLoop();
  else headlessSimLoop();

  return 0;
}
