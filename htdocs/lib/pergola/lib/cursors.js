
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * CURSORS LIBRARY
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


pergola.cursors = function () {
  var defs = pergola.defs;
  var path = pergola.path + "lib/cursors/";

  $C({
    element : "cursor",
    id : "lensCursor",
    "xlink:href" : path + "cursor_lens_empty.png",
    x : 6,
    y : 6,
    appendTo : defs
  });

  $C({
    element : "cursor",
    id : "minusCursor",
    "xlink:href" : path + "cursor_lens_minus.png",
    x : 6,
    y : 6,
    appendTo : defs
  });

  $C({
    element : "cursor",
    id : "plusCursor",
    "xlink:href" : path + "cursor_lens_plus.png",
    x : 6,
    y : 6,
    appendTo : defs
  });

};
