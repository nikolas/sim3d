#include "VortexForce.h"

VortexForce::VortexForce( const std::pair<int,int>& particles, const scalar& kbs, const scalar& kvc )
: Force()
, m_particles(particles)
, m_kbs(kbs)
, m_kvc(kvc)
{
  assert( m_particles.first >= 0 );
  assert( m_particles.second >= 0 );
  assert( m_particles.first != m_particles.second );
}

VortexForce::~VortexForce()
{}
  
void VortexForce::addEnergyToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, scalar& E )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size()%3 == 0 );
  assert( m_particles.first >= 0 );  assert( m_particles.first < x.size()/3 );
  assert( m_particles.second >= 0 ); assert( m_particles.second < x.size()/3 );

  std::cerr << outputmod::startred << "ERROR IN VORTEXFORCE: " << outputmod::endred << "No energy defined for VortexForce." << std::endl;
  exit(1);
}

void VortexForce::addGradEToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, VectorXs& gradE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == gradE.size() );
  assert( x.size()%3 == 0 );
  assert( m_particles.first >= 0 );  assert( m_particles.first < x.size()/3 );
  assert( m_particles.second >= 0 ); assert( m_particles.second < x.size()/3 );

  Vector3s rhat = x.segment<3>(3*m_particles.second)-x.segment<3>(3*m_particles.first); 
  scalar r = rhat.norm(); 
  assert( r != 0.0 ); 
  rhat /= r;
  rhat *= m_kbs;
  // Rotate rhat 90 degrees clockwise
  scalar temp = rhat.x();
  rhat.x() = -rhat.y();
  rhat.y() = temp;
  
  rhat -= v.segment<3>(3*m_particles.second)-v.segment<3>(3*m_particles.first);
  
  rhat /= r*r;
  rhat *= m_kvc;

  gradE.segment<3>(3*m_particles.first)  -= -rhat;
  gradE.segment<3>(3*m_particles.second) += -rhat;
}

void VortexForce::addHessXToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );

  std::cerr << outputmod::startred << "ERROR IN VORTEXFORCE: " << outputmod::endred << "No addHessXToTotal defined for VortexForce." << std::endl;
  exit(1);
}

void VortexForce::addHessVToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );

  std::cerr << outputmod::startred << "ERROR IN VORTEXFORCE: " << outputmod::endred << "No addHessXToTotal defined for VortexForce." << std::endl;
  exit(1);
}

Force* VortexForce::createNewCopy()
{
  return new VortexForce(*this);
}
