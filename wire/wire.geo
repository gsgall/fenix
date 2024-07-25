SetFactory("OpenCASCADE");

inner_radius = 0.25;
radius = 1;
length = 2;

Circle(1) = {0, 0, -1, inner_radius, 0, 2*Pi};
Circle(2) = {0, 0, -1, radius, 0, 2*Pi};

Line Loop(1) = {1};
Line Loop(2) = {2};

Plane Surface(1) = {1};
Plane Surface(2) = {1, 2};

Extrude {0, 0, length} {
  Surface{1}; // Extrude surface from Circle 1
}
Extrude {0, 0, length} {
  Surface{2}; // Extrude surface from Circle 2
}

// Combine volumes into a single volume
// BooleanUnion{ Volume{1}; Volume{2}; }

Physical Surface("boundary") = {1, 2, 4, 6, 7};
Physical Volume("vacuum") = {1, 2};

Field[2] = MathEval;
Field[2].F = "0.05 + 0.1 * Sqrt(x^2 + y^2)";

Background Field = 2;
