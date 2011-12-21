#ifndef __TWO_DIMENSIONAL_DISPLAY_CONTROLLER_H__
#define __TWO_DIMENSIONAL_DISPLAY_CONTROLLER_H__

#include <cmath>
#include <iostream>
#include <cassert>

#include "MathDefs.h"
#include "RenderingUtilities.h"

class TwoDimensionalDisplayController
{
public:
  TwoDimensionalDisplayController( int width, int height );
  
  void reshape( int w, int h );
  
  void keyboard( unsigned char key, int x, int y );
  void special( int key, int x, int y );
  void mouse( int button, int state, int x, int y );
  void motion( int x, int y );
  
  int getWindowWidth() const;  
  int getWindowHeight() const;

  void setCamera(float theta, float phi, float r);

  void updateViewMatrix();
  
private:
  void updateCameraLocation();
  void setDefaultCamera();

  Vector3s cameraLocation;
  Vector3s lookAt;
  float cameraR;
  float cameraTheta;
  float cameraPhi;

  // Width of the window in pixels
  int m_window_width;
  // Height of the window in pixels
  int m_window_height;
};

#endif
