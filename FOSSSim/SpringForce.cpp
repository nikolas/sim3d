#include "SpringForce.h"

SpringForce::SpringForce( const std::pair<int,int>& endpoints, const scalar& k, const scalar& l0, const scalar& b )
: Force()
, m_endpoints(endpoints)
, m_k(k)
, m_l0(l0)
, m_b(b)
{
  assert( m_endpoints.first >= 0 );
  assert( m_endpoints.second >= 0 );
  assert( m_endpoints.first != m_endpoints.second );
  assert( m_k >= 0.0 );
  assert( m_l0 >= 0.0 );
  assert( m_b >= 0.0 );
}

SpringForce::~SpringForce()
{}
  
void SpringForce::addEnergyToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, scalar& E )
{
  assert( x.size() == v.size() );
  assert( x.size()%3 == 0 );
  assert( m_endpoints.first >= 0 );  assert( m_endpoints.first < x.size()/3 );
  assert( m_endpoints.second >= 0 ); assert( m_endpoints.second < x.size()/3 );

  scalar l = (x.segment<3>(3*m_endpoints.second)-x.segment<3>(3*m_endpoints.first)).norm();
  E += 0.5*m_k*(l-m_l0)*(l-m_l0);
}

void SpringForce::addGradEToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, VectorXs& gradE )
{
  assert( x.size() == v.size() );
  assert( x.size() == gradE.size() );
  assert( x.size()%3 == 0 );
  assert( m_endpoints.first >= 0 );  assert( m_endpoints.first < x.size()/3 );
  assert( m_endpoints.second >= 0 ); assert( m_endpoints.second < x.size()/3 );

  // Compute the elastic component
  Vector3s nhat =
    x.segment<3>(3*m_endpoints.second) - x.segment<3>(3*m_endpoints.first); 

  scalar l = nhat.norm(); 
  assert( l != 0.0 ); 
  nhat /= l;
  Vector3s fdamp = nhat;
  nhat *= m_k*(l-m_l0);
  gradE.segment<3>(3*m_endpoints.first)  -= nhat;
  gradE.segment<3>(3*m_endpoints.second) += nhat;

  // Compute the internal damping
  // Remember we are computing minus the force here
  fdamp *= m_b*fdamp.dot(v.segment<3>(3*m_endpoints.second)-v.segment<3>(3*m_endpoints.first));
  gradE.segment<3>(3*m_endpoints.first)  -= fdamp;
  gradE.segment<3>(3*m_endpoints.second) += fdamp;
}

void SpringForce::addHessXToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert(0 && "Spring forces cannot be used with implicit integrators.");
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );
  assert( m_endpoints.first >= 0 );  assert( m_endpoints.first < x.size()/3 );
  assert( m_endpoints.second >= 0 ); assert( m_endpoints.second < x.size()/3 );

  Vector3s n_hat = x.segment<3>(3*m_endpoints.second)-x.segment<3>(3*m_endpoints.first); 
  scalar l = n_hat.norm();
  n_hat.normalize();

  Matrix3s n_hat_outer = n_hat * n_hat.transpose();
  Matrix3s K = -m_k * (n_hat_outer + 
      ((l - m_l0) / l) * (Matrix3s::Identity() - n_hat_outer));

  Vector3s v_diff = v.segment<3>(3 * m_endpoints.first) -
    v.segment<3>(3 * m_endpoints.second);
  Matrix3s K_damp = -(m_b / l) * (n_hat.dot(v_diff) * Matrix3s::Identity() +
      n_hat * v_diff.transpose()) * (Matrix3s::Identity() - n_hat_outer);

  hessE.block<3,3>(m_endpoints.first * 3, m_endpoints.first * 3) += K + K_damp;
  hessE.block<3,3>(m_endpoints.second * 3, m_endpoints.second * 3) += K + K_damp;
  hessE.block<3,3>(m_endpoints.second * 3, m_endpoints.first * 3) -= K + K_damp;
  hessE.block<3,3>(m_endpoints.first * 3, m_endpoints.second * 3) -= K + K_damp;
}

void SpringForce::addHessVToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE )
{
  assert(0 && "Spring forces cannot be used with implicit integrators.");
  assert( x.size() == v.size() );
  assert( x.size() == m.size() );
  assert( x.size() == hessE.rows() );
  assert( x.size() == hessE.cols() );
  assert( x.size()%3 == 0 );
  assert( m_endpoints.first >= 0 );  assert( m_endpoints.first < x.size()/3 );
  assert( m_endpoints.second >= 0 ); assert( m_endpoints.second < x.size()/3 );

  Vector3s n_hat = x.segment<3>(3*m_endpoints.second)-x.segment<3>(3*m_endpoints.first); 
  n_hat.normalize();
  Matrix3s B = m_b * n_hat * n_hat.transpose();

  hessE.block<3,3>(m_endpoints.first * 3, m_endpoints.first * 3) -= B;
  hessE.block<3,3>(m_endpoints.second * 3, m_endpoints.second * 3) -= B;
  hessE.block<3,3>(m_endpoints.second * 3, m_endpoints.first * 3) += B;
  hessE.block<3,3>(m_endpoints.first * 3, m_endpoints.second * 3) += B;
}

Force* SpringForce::createNewCopy()
{
  return new SpringForce(*this);
}
