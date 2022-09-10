function pitch(z, r) { return Math.asin(z / r); }
function yaw(x, r) { return Math.acos(x / r); }

function toInt(hi, lo) {
  var i = hi * 256 + lo;
  return i > 32767 ? i - 65536 : i;
}

function scalar(a, b) {
  return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

function matrix_transpose(m) {
  var a = [m[0][0], m[1][0], m[2][0]];
  var b = [m[0][1], m[1][1], m[2][1]];
  var c = [m[0][2], m[1][2], m[2][2]];
  return [a, b, c];
}

function vector_matrix(v, m) {
  return [scalar(v, m[0]), scalar(v, m[1]), scalar(v, m[2])];
}

function matrix_vector(m, v) {
  return [scalar(m[0], v), scalar(m[1], v), scalar(m[2], v)];
}

function matrix_matrix(a, b) {
  b = matrix_transpose(b);
  return [vector_matrix(a[0], b), vector_matrix(a[1], b), vector_matrix(a[2], b)];
}

function yaw_m(angle) {
  cos = Math.cos(angle);
  sin = Math.sin(angle);
//  return [[1, 0, 0], [0, cos, -sin], [0, sin, cos]];
  return [[cos, 0, sin], [0, 1, 0], [-sin, 0, cos]];
}

function pitch_m(angle) {
  cos = Math.cos(angle);
  sin = Math.sin(angle);
  return [[cos, -sin, 0], [sin, cos, 0], [0, 0, 1]];
}
