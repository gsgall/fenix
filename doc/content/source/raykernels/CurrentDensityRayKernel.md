# CurrentDensityRayKernel.md

!syntax description /RayKernels/CurrentDensityRayKernel

## Overview

For instances where the system of equations of interest include the current density, $\vec{J}$, a charge conservative current density scheme is used, this scheme was originally proposed by [!cite](eastwood1991virtual) and then later generalized by [!cite](pinto2014charge). FENIX uses the formulation described by [!cite](pinto2014charge). The instantaneous current density, at the $n^\text{th}$ time step from particles, $\vec{J}_n$, is given by,

\begin{equation}
  \vec{J}_n =
  \sum_{i=1}^N
  \omega_i q_i \vec{v}_i(t_n)
  \delta
  \left(\vec{r} - \vec{r}_i \right)
\end{equation}

where the subscript $i$ denotes the $i^{\text{th}}$ computational particle, $\omega_i$ is the computational particle weight, in a cartesian coordinate system this represents the number of real particles per $[m^{d -1}]$ where $d$ represents the dimension of the mesh, $q_i$ is the charge of the $\vec{v}_i(t_n)$ is the velocity of the particle, $\delta$ denotes the dirac delta function, and $\vec{r}_i(t_n)$ is the position the particle at the time the $n^\text{th}$ time step occurs. In order to satisfy the charge continuity equation,

\begin{equation}
  \frac{\partial \rho}{\partial t}
  +
  \nabla \cdot \vec{J}
  = 0,
\end{equation}

a time avereraged current density, $\vec{J}_{n + 1/2}$ is used

\begin{equation}
  \vec{J}_{n+1/2}
  =
  \frac{1}{\Delta t}
  \sum_{i=1}^N
  \int_{t_n}^{t_{n+1}}
  \omega_i q_i \vec{v}_i(t)
  \delta
  \left(\vec{r} - \vec{r}_i \right)
  dt.
\end{equation}

Taking the inner product of the time averaged current density with the test functions yields the variational current density

\begin{equation}
  \left<
    \vec{J}_{n+1/2}
    \vec{\psi}
  \right>
  =
  \frac{1}{\Delta t}
  \int_{t_n}^{t_{n+1}}
  \omega_i q_i \vec{v}_i(t)
  \cdot
  \vec{\psi}(\vec{r}_i(t))\;
  dt
\end{equation}

At this point we utilize a change of variables to transform this integral from a time integral to a path integral along the path of the i$^{th}$ particle.

\begin{equation}
  \left<
    \vec{J}_h^{\;n+1/2},
    \vec{\psi}
  \right>
  =
  \frac{1}{\Delta t}
  \sum_i^N
  \int_{\vec{r}_{i,n}}^{\vec{r}_{i,n+1}}
  \omega_i q_i
  \vec{\psi}(\vec{r}\,)\;
  d\Omega
\end{equation}

This spatial integral formulation of the time averaged current density is what is implemented in this RayKernel.

## Charge Conservation

For computational particles which are considered to be point particles, utilizing the dirac delta function for the particle shape function, the discrete variational formulation of the charge continituity equation is given by

\begin{equation}
  \left<
    \rho_{n+1} - \rho_n,
    \psi
  \right>
  =
  \Delta t
  \left<
    \vec{J}_{n+1/2},
    \nabla
    \psi
  \right>
\end{equation}

FENIX demonstrates that this statement of charge conservation is satisfied with the following test

!listing test/tests/raykernels/current_density/conservation.i

## Example Input Syntax

!listing test/tests/raykernels/current_density/particle_current_density.i block=RayKernels

!syntax parameters /RayKernels/CurrentDensityRayKernel

!syntax inputs /RayKernels/CurrentDensityRayKernel

!syntax children /RayKernels/CurrentDensityRayKernel

!bibtex bibliography
