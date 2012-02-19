  float
_gfortran_pow_r4_i4 (float a, int b)
{
  float pow, x;
  int n, u;

  n = b;
  x = a;
  pow = 1;
  if (n != 0)
  {
    if (n < 0)
    {

      n = -n;
      x = pow / x;
    }
    u = n;
    for (;;)
    {
      if (u & 1)
	pow *= x;
      u >>= 1;
      if (u)
	x *= x;
      else
	break;
    }
  }
  return pow;
}

