#ifndef __VORTEX_FORCE_H__
#define __VORTEX_FORCE_H__

#include <iostream>
#include <Eigen/Core>
#include "Force.h"
#include "StringUtilities.h"

class VortexForce : public Force
{
public:

  VortexForce( const std::pair<int,int>& particles, const scalar& kbs, const scalar& kvc );

  virtual ~VortexForce();

  virtual void addEnergyToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, scalar& E );

  virtual void addGradEToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, VectorXs& gradE );

  virtual void addHessXToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE );

  virtual void addHessVToTotal( const VectorXs& x, const VectorXs& v, const VectorXs& m, MatrixXs& hessE );

  virtual Force* createNewCopy();

private:
  std::pair<int,int> m_particles;
  // 'Biot-Savart' constant
  scalar m_kbs;
  // Viscosity constant
  scalar m_kvc;
};

#endif
