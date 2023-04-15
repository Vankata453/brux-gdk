# <center>**Brux Scripting Reference Manual**</center>
## <center>Maths</center>



&nbsp;

* <a name="randInt"></a>**`randInt( max )`**

  Returns a random integer between 0 and `max`.

* <a name="randFloat"></a>**`randFloat( max )`**

  Returns a random float between 0 and `max`.

* <a name="lendirX"></a>**`lendirX( length, direction )`**

  Returns the X coordinate of a vector.

* <a name="lendirY"></a>**`lendirY( length, direction )`**

  Returns the Y coordinate of a vector.

* <a name="distance2"></a>**`distance2( x1, y1, x2, y2 )`**

  Returns the distance between two points in two-dimensional space.

* <a name="distance3"></a>**`distance3( x1, y1, z1, x2, y2, z2 )`**

  Returns the distance between two points in three-dimensional space.

* <a name="inDistance2"></a>**`inDistance2( x1, y1, x2, y2, distance )`**

  Checks if the distance between two points is less than or equal to the given distance. Faster than `distance2()`, since it does not need to return the actual distance, so it can find it without using square root.

* <a name="distance3"></a>**`distance3( x1, y1, z1, x2, y2, z2 )`**

  Returns the distance between two points in three-dimensional space.

* <a name="wrap"></a>**`wrap( num, low, high )`**

  Takes `num` and wraps it around so that it is in between `low` and `high`, then returns it as a float.

* <a name="pointAngle"></a>**`pointAngle( x1, y1, x2, y2 )`**

  Returns the angle between two points in degrees.

* <a name="round"></a>**`round( num )`**

  Returns `num` rounded to the nearest integer.

* <a name="ceil"></a>**`ceil( num )`**

  Returns `num` rounded to the higher integer.

* <a name="floor"></a>**`floor( num )`**

  Returns `num` rounded to the lower integer.

* <a name="abs"></a>**`abs( num )`**

  Returns the absolute value of `num`.

* <a name="binstr"></a>**`binstr( num )`**

  Returns `num` in binary notation as a string.