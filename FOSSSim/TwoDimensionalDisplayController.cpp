#include "TwoDimensionalDisplayController.h"

TwoDimensionalDisplayController::TwoDimensionalDisplayController( int width, int height )
: m_window_width(width)
, m_window_height(height)
{
  setDefaultCamera();
}

void TwoDimensionalDisplayController::setDefaultCamera() {
  cameraR = 2;
  cameraTheta = M_PI / 2;
  cameraPhi = M_PI / 2;
  lookAt = Vector3s(0, 0, 0);
  updateCameraLocation();
}

void TwoDimensionalDisplayController::updateCameraLocation() {
  cameraLocation = Vector3s(
      cameraR * sin(cameraTheta) * cos(cameraPhi),
      cameraR * cos(cameraTheta),
      cameraR * sin(cameraTheta) * sin(cameraPhi));
}

void TwoDimensionalDisplayController::setCamera(
    float theta, float phi, float r) {
  cameraR = r;
  cameraTheta = theta;
  cameraPhi = phi;
  updateCameraLocation();
}

void TwoDimensionalDisplayController::updateViewMatrix() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(
      90., (float) m_window_width / (float) m_window_height, .1, 1000.);
  gluLookAt(cameraLocation[0], cameraLocation[1], cameraLocation[2],
      lookAt[0], lookAt[1], lookAt[2], 0, 1, 0);
}

void TwoDimensionalDisplayController::reshape( int w, int h ) 
{
  assert( renderingutils::checkGLErrors() );

  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);

  // Record the new width and height
  m_window_width = w;
  m_window_height = h;
  // Reset the coordinate system before modifying
  updateViewMatrix();

  // Render the scene
  glutPostRedisplay();
  assert( renderingutils::checkGLErrors() );
}

void TwoDimensionalDisplayController::keyboard( unsigned char key, int x, int y )
{
  if (key == '=') {
    cameraR -= .1;
  } else if (key == '-') {
    cameraR += .1;
  } else if (key == 'c') {
    setDefaultCamera();
  }

  updateCameraLocation();
  glutPostRedisplay();
}

void TwoDimensionalDisplayController::special( int key, int x, int y )
{
  if (key == GLUT_KEY_LEFT) {
    cameraPhi += M_PI/180;
  } else if (key == GLUT_KEY_RIGHT) {
    cameraPhi -= M_PI/180;
  } else if (key == GLUT_KEY_UP) {
    cameraTheta -= M_PI/180;
  } else if (key == GLUT_KEY_DOWN) {
    cameraTheta += M_PI/180;
  }

  /* gluPerspective gets a bit angry if we ask it to put things on the y-axis,
   * because +y is our up-vector, so make sure there's a bit of an epsilon
   * between the y axis and the camera. */
  if (cameraTheta >= M_PI) {
    cameraTheta = M_PI - 1e-7;
  } else if (cameraTheta <= 0) {
    cameraTheta = 0 + 1e-7;
  }

  updateCameraLocation();
  glutPostRedisplay();
}

void TwoDimensionalDisplayController::mouse(
    int button, int state, int x, int y ) {}

void TwoDimensionalDisplayController::motion( int x, int y ) {}

int TwoDimensionalDisplayController::getWindowWidth() const
{
  return m_window_width;
}

int TwoDimensionalDisplayController::getWindowHeight() const
{
  return m_window_height;
}
