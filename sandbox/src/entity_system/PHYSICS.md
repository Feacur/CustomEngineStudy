* https://en.wikipedia.org/wiki/List_of_moments_of_inertia
* https://en.wikipedia.org/wiki/List_of_second_moments_of_area
* https://en.wikipedia.org/wiki/Polygon
* https://allenchou.net/game-physics-series/
* https://hyperphysics.phy-astr.gsu.edu/hbase/rke.html
* https://www.youtube.com/watch?v=7Ik2vowGcU0
* https://gafferongames.com/post/integration_basics/
* https://github.com/erincatto/box2d-lite
* https://github.com/RandyGaul/ImpulseEngine
* https://en.wikipedia.org/wiki/Coefficient_of_restitution
* https://www.youtube.com/watch?v=fdZfddO7YTs
* https://en.wikipedia.org/wiki/Conservation_of_momentum
* https://en.wikipedia.org/wiki/Conservation_of_energy
* https://en.wikipedia.org/wiki/Angular_momentum
* https://en.wikipedia.org/wiki/Momentum
* https://en.wikipedia.org/wiki/Friction
* https://en.wikipedia.org/wiki/Stiction
* https://en.wikipedia.org/wiki/Angular_velocity
* https://en.wikipedia.org/wiki/Angular_acceleration
* https://stackoverflow.com/questions/31106438/calculate-moment-of-inertia-given-an-arbitrary-convex-2d-polygon
* https://stackoverflow.com/questions/41592034/computing-tensor-of-inertia-in-2d
* https://en.wikipedia.org/wiki/Moment_of_inertia
* https://en.wikipedia.org/wiki/Second_moment_of_area
* https://www.gdcvault.com/play/1017646/Physics-for-Game-Programmers:-The-Separating-Axis-Test-between-Convex-Polyhedra-by-Dirk-Gregorius
* http://www.dyn4j.org/2010/01/sat/
* http://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
* http://www.dyn4j.org/2010/04/gjk-distance-closest-points/
* http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
* http://www.dyn4j.org/2011/11/contact-points-using-clipping/

```
from here I shorten `coefficient of restitution` to `restitution`
the same goes for `friction` and `stiction`
```

# simplest collision response against a static object looks like
```
velocity = reflect(velocity, normal, 1 + restitution);
but general case is a tiniest bit more complex:
reflection factors depend on the plane [or line for 2d] of impact, masses, and relative velocity
```

# restitution; normal direction separation
```
	masses:                   m1, m2; constant
	linear velocities before: v1, v2
	linear velocities after:  u1, u2
	linear accelerations:     a1, a2
	linear momentums:         P1, P2
	linear forces:            F1, F2
	---- ---- ---- ----
	shape: point; other shapes experience conversion of a portion of *impulses* into `angular momentum`;
			only impact in the direction normal to the surfaces is accounted in these formulas, so that we
			can compute separation impulses, disregarding tangential motion; or rather postponing its computation
	---- ---- ---- ----
	P   = m*v
	F   = m*a
	Ekl = m*v*v/2
	---- ---- ---- ----
	---- ---- ---- ----
>> express an equation for `energy conservation`
	m1*v1*v1/2 + m2*v2*v2/2 + other_energies = m1*u1*u1/2 + m2*u2/2 + other_energies + kinetic_energy_change
	---- ---- ---- ----
	kinetic_energy_change * (m*v*v/2) = (m*u*u/2)
	kinetic_energy_change = (u*u) / (v*v)
	---- ---- ---- ----
	---- ---- ---- ----
>> express an equation for `momentum conservation`
	m1*v1 + m2*v2 = m1*u1 + m2*u2
	---- ---- ---- ----
	m1*v1 + deformation_impulse = m1*v_impact
	m2*v2 - deformation_impulse = m2*v_impact
	m1*v_impact + restoration_impulse = m1*u1
	m2*v_impact - restoration_impulse = m2*u2
	---- ---- ---- ----
>> express an equation for `coefficient of restitution`
	restitution = square_root(kinetic_energy_change)
	restitution ~ |u| / |v|
	---- ---- ---- ----
	restitution = restoration_impulse / deformation_impulse
	restitution = (u1 - v_impact) / (v_impact - v1)
	restitution = (v_impact - u2) / (v2 - v_impact)
	---- ---- ---- ----
	restitution = (restoration_impulse + restoration_impulse) / (deformation_impulse + deformation_impulse)
	restitution = ((u1 - v_impact) + (v_impact - u2)) / ((v_impact - v1) + (v2 - v_impact))
	restitution * (v2 - v1) = (u1 - u2)
	---- ---- ---- ----
	---- ---- ---- ----
>> use the equations of `coefficient of restitution` and `momentum conservation`
	u1 = (m1*v1 + m2*v2 - m2*u2) / m1
	u2 = u1 + restitution*(v1 - v2)
	---- ---- ---- ----
	u1 = (m1*v1 + m2*v2 - m2*restitution*(v1 - v2)) / (m1 + m2)
	u1 = v1 +  m2 * (v2 - v1) * (1 + restitution) / (m1 + m2)
	---- ---- ---- ----
	linear separation impulse = (v2 - v1) * (1 + restitution) / (m1 + m2)
```

# use inverse masses for computation:
```
the reason is presence of infinite mass objects; you do want to avoid doing `inf / inf`;
additionally, each object operates an impulse and its mass only
impulse = (v2 - v1) * (1 + restitution) / (1 / m1 + 1 / m2)
u1 = v1 + impulse * (1 / m1)
u2 = v2 - impulse * (1 / m2)
```

# friction, stiction; tangential direction separation
```
friction force:   Ff
stiction force:   Fs
normal force:     Fn
tangential force: Ft
---- ---- ---- ----
---- ---- ---- ----
- friction force is proportional to normal force and is directed opposite to the tangential motion vector
- ... which is `Ff = Fn * friction`
- also we can account Coulomb's law: if tangential force is lower than some portion of normal force, then
- ... no motion occurs in that direction until some threshold met
```

# application of angular momentum, etc.
```
moment of inertia:         I1, I2; constant; depends on the shape
angular velocities before: w1, w2
angular velocities after:  f1, f2
angular accelerations:     @1, @2
angular momentums:         L1, L2
angular forces:            T1, T2; or `torque`
some radii:                r1, r2
tangent vilocities:        V1, V2
---- ---- ---- ----
L   = I*w
T   = I*@
Ekr = I*w*w/2
V   = w*r
---- ---- ---- ----
---- ---- ---- ----
```
