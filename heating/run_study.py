import astropy.units as u
import astropy.constants as c
import numpy as np
import os
import random
from random import randint
import mooseutils

mpi=1
executeable="../fenix-opt"
supress_output = True

family = ["LAGRANGE", "HIERARCHIC", "BERNSTEIN", "HERMITE"]
order = ["SECOND"]
resolutions = [0.5, 1, 2, 4, 6, 8, 10]
end_times = [0.01, 0.01, 0.01, 0.007, 0.007, 0.007,0.007]

eps_0 = c.eps0
m = c.m_e
e = c.e.si
n = 5.5e7 / u.m**3
T = 1 * u.eV / c.k_B
lambda_D = np.sqrt(eps_0 * c.k_B * T / (n * e**2)).to(u.m)
omega_p = np.sqrt(n * e**2 / (m * eps_0)).to(1 / u.s)
sigma = np.sqrt(c.k_B * T / m).to((u.m / u.s))

num_elems = 10
num_trials = 10

print("lambda_D    : ", lambda_D)
print("omega_p     : ", omega_p)
print("sigma       : ", sigma)


random.seed(10)
upper_dir = "heating-data-all-particles-eps-real"

if not os.path.exists(upper_dir):
  os.mkdir(upper_dir)

for fam in family:
  family_dir = f"{upper_dir:s}/{fam:s}"
  if not os.path.exists(family_dir):
    os.mkdir(family_dir)

  if fam == "LAGRANGE":
    order = ["FIRST", "SECOND"]
    elem_type = "EDGE2"
  else:
    order = ["SECOND"]
    elem_type = "EDGE3"

  for o in order:
    if fam == "HERMITE":
      o = "THIRD"
      elem_type = "EDGE3"
    else:
      if o == "FIRST":
        elem_type = "EDGE2"
      else:
        elem_type = "EDGE3"

    order_dir = f"{family_dir:s}/{o:s}"
    print(f"{fam:s}-{o:s}")
    if not os.path.exists(order_dir):
      os.mkdir(order_dir)

    for end_time, r in zip(end_times, resolutions):
      res_dir = f"{order_dir:s}/h-lambda-{r:0.1f}"

      if not os.path.exists(res_dir):
        os.mkdir(res_dir)

      h = lambda_D * r
      DT_opt = min(1/2 * h /lambda_D, 1) / omega_p

      x_max = h * num_elems

      for i in range(num_trials):
        seed1 = randint(0, 10000)
        seed2 = randint(0, 10000)

        data_file = f"{res_dir:s}/trial-{i:d}"
        print(f"  Trial: {i:d}")
        a = ["-i", "heating.i","--allow-test-objects",
            f"Mesh/gmg/elem_type={elem_type:s}", f"Variables/phi/family={fam:s}", f"Variables/phi/order={o:s}",
            f"Mesh/gmg/xmax={x_max.value:0.4e}", f"Distributions/boltz/standard_deviation={sigma.value:0.4e}", f"UserObjects/electrons/seed={seed1:d}", f"UserObjects/ions/seed={seed2:d}", f"Executioner/dt={DT_opt.value:0.4e}", f"Executioner/end_time={end_time:0.4e}",f"Outputs/file_base={data_file:s}"]
        mooseutils.run_executable(executeable, *a, mpi=mpi, suppress_output=supress_output)



