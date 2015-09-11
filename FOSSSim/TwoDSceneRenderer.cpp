#include "TwoDSceneRenderer.h"
using namespace std;

TwoDSceneRenderer::TwoDSceneRenderer( const TwoDScene& scene, const std::vector<renderingutils::Color>& particle_colors, const std::vector<renderingutils::Color>& edge_colors, const std::vector<renderingutils::ParticlePath>& particle_paths )
: m_scene(scene)
, m_particle_colors(particle_colors)
, m_edge_colors(edge_colors)
, m_particle_paths(particle_paths)
{
  assert( (int) m_particle_colors.size() == m_scene.getNumParticles() );
  assert( (int) m_edge_colors.size() == m_scene.getNumEdges() );
}

void TwoDSceneRenderer::updateState()
{
  const VectorXs& x = m_scene.getX();
  for( std::vector<renderingutils::ParticlePath>::size_type i = 0; i < m_particle_paths.size(); ++i )
    m_particle_paths[i].addToPath(x.segment<3>(3*m_particle_paths[i].getParticleIdx()));
}


void TwoDSceneRenderer::renderSolidCircle( const Eigen::Vector3d& center, double radius ) const
{  
  glPushMatrix();
  glTranslatef(center[0], center[1], center[2]);
  glutSolidSphere(radius, 40, 40);
  glPopMatrix();
}

void TwoDSceneRenderer::renderCircle( const Eigen::Vector3d& center, double radius ) const
{

}

void TwoDSceneRenderer::renderSweptEdge( const Eigen::Vector3d& x0, const Eigen::Vector3d& x1, double radius ) const
{
}

void TwoDSceneRenderer::renderScene() const
{
  const VectorXs& x = m_scene.getX();
  assert( x.size()%3 == 0 );
  assert( 3*m_scene.getNumParticles() == x.size() );

  assert( renderingutils::checkGLErrors() );
  for( std::vector<renderingutils::ParticlePath>::size_type i = 0; i < m_particle_paths.size(); ++i )
  {
    const std::list<Vector3s>& ppath = m_particle_paths[i].getPath();
    const renderingutils::Color& pathcolor = m_particle_paths[i].getColor();

    GLfloat color[4] = {pathcolor.r, pathcolor.g, pathcolor.b, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    GLfloat arr[3] = {1., 1., 1.};
    glMaterialfv(GL_FRONT, GL_SPECULAR, arr);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glBegin(GL_LINE_STRIP);
    for( std::list<Vector3s>::const_iterator itr = ppath.begin(); itr != ppath.end(); ++itr )
    {
      glVertex3d(itr->x(),itr->y(), itr->z());
    }
    glEnd();
  }
  assert( renderingutils::checkGLErrors() );
  
  // Render edges
  GLfloat color[4] = {0.4, 0.4, 0.4, 1};
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
  GLfloat arr[3] = {1., 1., 1.};
  glMaterialfv(GL_FRONT, GL_SPECULAR, arr);
  glMateriali(GL_FRONT, GL_SHININESS, 128);

  const std::vector<std::pair<int,int> >& edges = m_scene.getEdges();
  const std::vector<scalar>& edgeradii = m_scene.getEdgeRadii();
  assert( edgeradii.size() == edges.size() );
  for( std::vector<std::pair<int,int> >::size_type i = 0; i < edges.size(); ++i )
  {
    assert( edges[i].first >= 0 );  assert( edges[i].first < m_scene.getNumParticles() );
    assert( edges[i].second >= 0 ); assert( edges[i].second < m_scene.getNumParticles() );
    glColor3d(m_edge_colors[i].r,m_edge_colors[i].g,m_edge_colors[i].b);
    glLineWidth(10);
    glBegin(GL_LINE);
    glVertex3d(x[3*edges[i].first], x[3*edges[i].first+1], x[3*edges[i].first+2]);
    glVertex3d(x[3*edges[i].second], x[3*edges[i].second+1], x[3*edges[i].second+2]);
    glEnd();
  }
  assert( renderingutils::checkGLErrors() );

  // Render particles
  const std::vector<scalar>& radii = m_scene.getRadii();
  assert( (int) radii.size() == m_scene.getNumParticles() );
  for( int i = 0; i < m_scene.getNumParticles(); ++i ) 
  {
    GLfloat color[4] = {
        m_particle_colors[i].r, m_particle_colors[i].g,
        m_particle_colors[i].b, 1};

    GLfloat arr[3] = {1., 1., 1.};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, arr);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    renderSolidCircle( x.segment<3>(3*i), radii[i] );
  }  
  assert( renderingutils::checkGLErrors() );
}

void TwoDSceneRenderer::circleMajorResiduals( const TwoDScene& oracle_scene, const TwoDScene& testing_scene, scalar eps ) const
{
  /*
  assert(   oracle_scene.getNumParticles() == testing_scene.getNumParticles() );
  assert( 3*oracle_scene.getNumParticles() == oracle_scene.getX().size() );
  assert( 3*oracle_scene.getNumParticles() == testing_scene.getX().size() );
  assert( 3*oracle_scene.getNumParticles() == oracle_scene.getV().size() );
  assert( 3*oracle_scene.getNumParticles() == testing_scene.getV().size() );

  const VectorXs& oracle_x = oracle_scene.getX();
  const VectorXs& testing_x = testing_scene.getX();

  const VectorXs& oracle_v = oracle_scene.getV();
  const VectorXs& testing_v = testing_scene.getV();

  glColor3d(1.0,0.0,0.0);
  for( int i = 0; i < oracle_scene.getNumParticles(); ++i )
  {
    scalar x_resid = (oracle_x.segment<3>(3*i)-testing_x.segment<3>(3*i)).norm();
    scalar v_resid = (oracle_v.segment<3>(3*i)-testing_v.segment<3>(3*i)).norm();
    if( x_resid > eps || v_resid > eps )
      renderCircle( oracle_x.segment<3>(3*i), 3.0*oracle_scene.getRadius(i) );
  }
  */
}

std::vector<renderingutils::Color>& TwoDSceneRenderer::getParticleColors()
{
  return m_particle_colors;
}

const std::vector<renderingutils::Color>& TwoDSceneRenderer::getParticleColors() const
{
  return m_particle_colors;
}

