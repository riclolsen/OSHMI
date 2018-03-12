pergola.extend(pergola.geom, {

// Intersection point between two lines.
// To test if point is on segments, test result with pointOnLine()
	intersectionPoint : function (a1, a2, b1, b2) {
    var A1 = a2.y - a1.y,
        B1 = a1.x - a2.x,
        C1 = A1 * a1.x + B1 * a1.y,
        A2 = b2.y - b1.y,
        B2 = b1.x - b2.x,
        C2 = A2 * b1.x + B2 * b1.y,
        det = A1 * B2 - A2 * B1;

    if (det != 0) {
      return {
        x : (B2 * C1 - B1 * C2) / det,
        y : (A1 * C2 - A2 * C1) / det
      }
    }
    return false;
  },

// Intersection test (source: Kevin Lindsay intersection library (adapted))
  intersect : function (a1, a2, b1, b2) {
    var ua_t = (b2.x - b1.x) * (a1.y - b1.y) - (b2.y - b1.y) * (a1.x - b1.x),
        ub_t = (a2.x - a1.x) * (a1.y - b1.y) - (a2.y - a1.y) * (a1.x - b1.x),
        u_b  = (b2.y - b1.y) * (a2.x - a1.x) - (b2.x - b1.x) * (a2.y - a1.y);

    if ( u_b != 0 ) {
      var ua = ua_t / u_b,
          ub = ub_t / u_b;

      if (0 <= ua && ua <= 1 && 0 <= ub && ub <= 1) return true;
    }
    return false;
  },

// Tests if point is between two points
  pointBetween : function (a, b, c) {
    var larger = (a >= b) ? a : b;
    var smaller = (a != larger) ? a : b;

    return c <= larger && c >= smaller;
  },

// Tests if point is on a segment
  pointOnLine : function (p, a, b) {
    if(b.x.trim(6) - a.x.trim(6) == 0) { // vertical line
      if (pergola.geom.pointBetween(a.y, b.y, p.y) && p.x.trim(6) == a.x.trim(6)) return true;
      return false;
    }
    if(b.y.trim(6) - a.y.trim(6) == 0) { // horizontal line
      if (pergola.geom.pointBetween(a.x, b.x, p.x) && p.y.trim(6) == a.y.trim(6)) return true;
      return false;
    }

    var Ax = (p.x - a.x) / (b.x - a.x);
    var Ay = (p.y - a.y) / (b.y - a.y);

    return Math.abs(Ax - Ay) < 0.000001 && Ax >= 0 && Ax <= 1;
  },

// Shortest distance from point to line
  getPDistance : function (x, y, x1, y1, x2, y2) {
    var A = x - x1,
        B = y - y1,
        C = x2 - x1,
        D = y2 - y1,
        dot = A * C + B * D,
        len_sq = C * C + D * D,
        param = dot / len_sq,
        xx, yy, dx, dy;

    if (param < 0 || (x1 == x2 && y1 == y2)) {
      xx = x1;
      yy = y1;
    }
    else if (param > 1) {
      xx = x2;
      yy = y2;
    }
    else {
      xx = x1 + param * C;
      yy = y1 + param * D;
    }

    dx = x - xx;
    dy = y - yy;
    return Math.sqrt(dx * dx + dy * dy);
  },

// Perpendicular point on line from point
  getPPoint : function (p, l){
    var x1 = l.x1,
        y1 = l.y1,
        x2 = l.x2,
        y2 = l.y2,
        px = x2 - x1,
        py = y2 - y1,
        d = px * px + py * py,
        u = ((p.x - x1) * px + (p.y - y1) * py) / d;

    return {x : x1 + u * px, y : y1 + u * py};
  },

// Distance between two points
  getP2Pdistance : function (a, b) {
    return Math.sqrt(Math.pow((b.x - a.x), 2) + Math.pow((b.y - a.y), 2));
  },

// True centroid (source: Paul Burke)
  centroid : function (points, area) {
    var nPts = points.length,
        x = 0,
        y = 0,
        f,
        j = nPts - 1,
        p1, p2;

    for (var i = 0; i < nPts; j = i++) {
      p1 = points[i];
      p2 = points[j];
      f = p1.x * p2.y - p2.x * p1.y;
      x += (p1.x + p2.x) * f;
      y += (p1.y + p2.y) * f;
    }

    f = area * 6;
    return {x: Math.abs(x / f), y : Math.abs(y / f)};
  },

  crossProduct : function (v1, v2) {
    return v1.x * v2.y - v2.x * v1.y;
  },

  angle : {
// Get angle of a line in degrees
    degrees : function (p1, p2) {
      var dy = p2.y - p1.y,
          dx = p2.x - p1.x;
      return Math.atan2(dy, dx) * 180 / Math.PI;
    },
// Get angle of point in radians
    radians : function (dy, dx) {
      return Math.atan2(dy, dx) + Math.PI / 2;
    },
// Converts radians (number or string) to degrees
// (or use Number.toDeg();)
    toDegrees : function (radians) {
      return Number(radians) * 180 / Math.PI;
    },
// Converts degrees (number or string) to radians
// (or use Number.toRad();)
    toRadians : function (degrees) {
      return Number(degrees) * Math.PI / 180;
    },
// Get angle between two lines
    angleBetween2Lines : function (a, b) {
      var angle1 = Math.atan2(a.y1 - a.y2, a.x1 - a.x2),
          angle2 = Math.atan2(b.y1 - b.y2, b.x1 - b.x2);

      return angle1 - angle2;
    },
// Constrain line to fixed angles
    constrain : function (angle, cardinals) {
      var a = 0,
          cardinals = cardinals || 16,     // 22.5 degrees. Use 8 for chunks of 45 degrees, 12 for 30 degrees, etc.
          d = 180 / cardinals;

      function constrain () {
        for (var i = 0; i < cardinals / 2; i ++) {
          if (angle >= a && angle <= (a += d * 2)) return true;
        }
      }

      if (angle >= (a - d) && angle <= (a += d)) return 0;                // E
      if (constrain()) return a - d;                                      // E to W clockwise

      if (angle >= (a = 180 - d) || angle <= (a *= ~0X0)) return 180;     // W
      if (constrain()) return a - d;                                      // W to E clockwise
    }
  },

  area : {
    triangle : function (s) {
// based on sides
      var a = s[0],
          b = s[1],
          c = s[2],
          p = (a + b + c) / 2;

      return Math.sqrt(p * (p - a) * (p - b) * (p - c));
    },

    polygon : function (X, Y, n) {
// based on vertices (clockwise or counterclockwise)
      var area = 0,
          j = n - 1;

      for (i = 0; i < n; j = i++) {
        area += (X[j] + X[i]) * (Y[j] - Y[i]);
      }

      return area / 2;
    }
  }

});