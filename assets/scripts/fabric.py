#!/usr/bin/env python

GRID_SIZE = 10
PATHLENGTH = 0
SPRING_K = 50

import random
import math

def format_particle(i, m, x, y, z, vx, vy, vz, r, cr, cg, cb, fixed=False):
  return '''
  <particle m="%f" px="%f" py="%f" pz="%f" vx="%f" vy="%f" vz="%f" fixed="%d" r="%f"/>
  <particlecolor i="%d" r="%f" g="%f" b="%f"/>
  <particlepath i="%d" duration="%f" r="%f" g="%f" b="%f"/>
  ''' % (
  m, x, y, z, vx, vy, vz, fixed and 1 or 0, r, i, cr, cg, cb, i, 
  PATHLENGTH, cr, cg, cb)

def ix(i, j):
  return GRID_SIZE*i+j

print '''
<scene>
  <duration time="100.0"/>
  <integrator type="symplectic-euler" dt="0.01"/>
  <maxsimfreq max="100.0"/>
  <backgroundcolor r="0." g="0." b="0."/>
  <camera r="10" theta="90" phi="45"/>
  <simplegravity fx="0" fy="-2" fz="0"/>
'''

for i in range(GRID_SIZE):
  for j in range(GRID_SIZE):
    m = 1
    vx = 0
    vy = 0
    vz = 0
    r = 0.01
    cr = 0.4
    cg = 0.4
    cb = 0.4

    x = i - (GRID_SIZE / 2)
    y = j - (GRID_SIZE / 2)
    z = -j / 1.5

    fixed = j == GRID_SIZE-1 and (i == 0 or i == GRID_SIZE-1)
    print(format_particle(ix(i,j), m, x, y, z, vx, vy, vz, r, cr, cg, cb, fixed));

k=0
for i in range(GRID_SIZE-1):
  for j in range(GRID_SIZE-1):
    print '''
    <edge i="%d" j="%d"/>
    <springforce edge="%d" k="%f" l0="1"/>''' % (
        ix(i,j), ix(i,j+1), k, SPRING_K)
    k += 1

    print '''
    <edge i="%d" j="%d"/>
    <springforce edge="%d" k="%f" l0="1"/>''' % (
        ix(i,j), ix(i+1,j), k, SPRING_K)
    k += 1

  print '''
  <edge i="%d" j="%d"/>
  <springforce edge="%d" k="%d" l0="1"/>''' % (
      ix(i,GRID_SIZE-1), ix(i+1,GRID_SIZE-1), k, SPRING_K)
  k += 1

  print '''
  <edge i="%d" j="%d"/>
  <springforce edge="%d" k="%d" l0="1"/>''' % (
      ix(GRID_SIZE-1, i), ix(GRID_SIZE-1, i+1), k, SPRING_K)
  k += 1
  
print '''
</scene>
'''
