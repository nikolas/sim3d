#include "SymplecticEuler.h"

SymplecticEuler::SymplecticEuler()
: SceneStepper()
{}

SymplecticEuler::~SymplecticEuler()
{}

bool SymplecticEuler::stepScene( TwoDScene& scene, scalar dt )
{
  VectorXs& x = scene.getX();
  VectorXs& v = scene.getV();
  const VectorXs& m = scene.getM();
  
  // Compute forces using start-of-step state
  VectorXs F(x.size());
  F.setZero();
  scene.accumulateGradU(F);
  // Force is negative the energy gradient
  F *= -1.0;
  
  // Zero the force for fixed DoFs
  for (int i = 0; i < scene.getNumParticles(); ++i) {
    if (scene.isFixed(i)) F.segment<3>(3*i).setZero();
  }
  
  // Step velocities forward based on start-of-step forces
  F.array() /= m.array();
  v += dt*F;
  
  // Step positions forward based on new velocities
  x += dt*v;
  
  return true;
}
  
std::string SymplecticEuler::getName() const
{
  return "Symplectic Euler";
}  
