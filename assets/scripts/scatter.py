#!/usr/bin/env python3

PATHLENGTH=0
MAX_X=1.0
MAX_Y=1.0
MAX_VX=1.0
PARTICLES=200

import random

def format_particle(i, m, x, y, z, vx, vy, vz, r, cr, cg, cb, fixed=False):
  return '''  <particle m="%f" px="%f" py="%f" pz="%f" vz="%f" vx="%f" vy="%f" fixed="%d" r="%f"/>
  <particlecolor i="%d" r="%f" g="%f" b="%f"/>
  <particlepath i="%d" duration="%f" r="%f" g="%f" b="%f"/>''' % (
  m, x, y, z, vz, vx, vy, fixed and 1 or 0, r, i, cr, cg, cb, i, PATHLENGTH, cr, cg, cb)

def main():
  print('''
<scene>
  <duration time="6.0"/>
  <maxsimfreq max="24.0"/>
  <integrator type="explicit-euler" dt="0.05"/>
  <simplegravity fx="0.3" fy="-3.0" fz="0"/>
  <backgroundcolor r="0.0" g="0.0" b="0.0"/>''')
  for i in range(PARTICLES):
    m = 10
    x = random.random() * random.choice([-1., 1.]) * MAX_X
    y = random.random() * random.choice([-1., 1.]) * MAX_Y
    z = random.random() * random.choice([-1., 1.]) * MAX_X
    vx = random.random() * random.choice([-1., 1.]) * MAX_VX
    vz = random.random() * random.choice([-1., 1.]) * MAX_VX
    vy = 2 + random.random()
    r = 3
    cr = 1
    cg = (1 / PARTICLES) * float(i)
    cb = 0

    print(format_particle(i, m, x, y, z, vx, vy, vz, r, cr, cg, cb));
  print('</scene>')

if __name__ == '__main__':
  main()
