#!/usr/bin/env python

PARTICLES = 300
MAX_R = 20
PATHLENGTH = .1

import random
import math

def format_particle(i, m, x, y, z, vx, vy, r, cr, cg, cb, fixed=False):
  return '''
  <particle m="%f" px="%f" py="%f" pz="%f" vx="%f" vy="%f" vz="0" fixed="%d" r="%f"/>
  <particlecolor i="%d" r="%f" g="%f" b="%f"/>
  <particlepath i="%d" duration="%f" r="%f" g="%f" b="%f"/>
  <vortexforce i="0" j="%d" kbs="10" kvc="1000.0"/>
  ''' % (
  m, x, y, z, vx, vy, fixed and 1 or 0, r, i, cr, cg, cb, i, 
  PATHLENGTH, cr, cg, cb, i)

print '''
<scene>
  <duration time="100.0"/>
  <integrator type="symplectic-euler" dt="0.01"/>
  <maxsimfreq max="100.0"/>
  <backgroundcolor r="0." g="0." b="0."/>
  <particle m="100000000.0" px="0." py="0." pz="0" vz="0" vx="0." vy="0." fixed="0"
  radius="0."/>
  <particle m="0.1" px="-50" py="-50" pz="0" vz="0" vx="0" vy="0" fixed="1" radius="0."/>
  <particle m="0.1" px="50" py="50" pz="0" vz="0" vx="0" vy="0" fixed="1" radius="0."/>
  <particlecolor i="0" r="0" g="0" b="0"/>
  <particlecolor i="1" r="0" g="0" b="0"/>
  <particlecolor i="2" r="0" g="0" b="0"/>
'''

for i in range(3, PARTICLES):
  m = 10
  radius = (random.random() ** 4) * MAX_R + (0.01 * MAX_R)
  theta = random.random() * math.pi * 2
  phi = random.random() * math.pi * 2
  vx = 0
  vy = 0
  r = 0.0001
  cr = (random.random() / 4 + 0.75) * 0.9
  cg = 1
  cb = (random.random() / 2 + 0.5) * 0.9

  x = radius * math.sin(theta) * math.cos(phi)
  y = radius * math.cos(theta)
  z = radius * math.sin(theta) * math.sin(phi)
  print(format_particle(i, m, x, y, z, vx, vy, r, cr, cg, cb));
  
print '''
</scene>
'''
