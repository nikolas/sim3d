#include "GravitationalForce.h"

GravitationalForce::GravitationalForce( const std::pair<int,int>& particles, const scalar& G )
: Force()
, m_particles(particles)
, m_G(G)
{
  assert( m_particles.first >= 0 );
  assert( m_particles.second >= 0 );
  assert( m_particles.first != m_particles.second );
  assert( m_G >= 0.0 );
}

GravitationalForce::~GravitationalForce()
{}
  
void GravitationalForce::addEnergyToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, scalar& E )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size()%3 == 0 );
  assert( m_particles.first >= 0 );  assert( m_particles.first < x.size()/3 );
  assert( m_particles.second >= 0 ); assert( m_particles.second < x.size()/3 );

  scalar r = (x.segment<3>(3*m_particles.second)-x.segment<3>(3*m_particles.first)).norm();
  scalar m1 = m(3*m_particles.second);
  scalar m2 = m(3*m_particles.first);
  E += -m_G*m1*m2/r;
}

void GravitationalForce::addGradEToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, VectorXs& gradE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == gradE.size() );
  assert( x.size()%3 == 0 );
  assert( m_particles.first >= 0 );  assert( m_particles.first < x.size()/3 );
  assert( m_particles.second >= 0 ); assert( m_particles.second < x.size()/3 );

  scalar m1 = m(3*m_particles.second);
  scalar m2 = m(3*m_particles.first);

  Vector3s nhat = x.segment<3>(3*m_particles.second)-x.segment<3>(3*m_particles.first); 
  scalar r = nhat.norm(); 
  assert( r != 0.0 ); 
  nhat /= r;
  nhat *= m_G*m1*m2/(r*r);
  
  gradE.segment<3>(3*m_particles.first)  -= nhat;
  gradE.segment<3>(3*m_particles.second) += nhat;
}

void GravitationalForce::addHessXToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );

  Vector3s nhat =
    x.segment<3>(3 * m_particles.first) - x.segment<3>(3 * m_particles.second); 
  scalar l = nhat.norm(); 
  nhat.normalize();

  Matrix3s K = -(m_G * m(3 * m_particles.first) * m(3 * m_particles.second) /
      pow(l, 3)) * (Matrix3s::Identity() - 3 * nhat * nhat.transpose());

  hessE.block<3,3>(m_particles.first * 3, m_particles.first * 3) += K;
  hessE.block<3,3>(m_particles.second * 3, m_particles.second * 3) += K;
  hessE.block<3,3>(m_particles.second * 3, m_particles.first * 3) -= K;
  hessE.block<3,3>(m_particles.first * 3, m_particles.second * 3) -= K;
}

void GravitationalForce::addHessVToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );
  // Nothing to do.
}

Force* GravitationalForce::createNewCopy()
{
  return new GravitationalForce(*this);
}
