
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * PATTERNS LIBRARY
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * license: http://www.dotuscomus.com/pergola/licenses
*/



pergola.pattern = {

  catseye : function (o) {
  	var id, p;

  	o = pergola.extend({
		      colors : ["#E08000", "#F0A000"],
					size : 5,
					opacity : 1,
					transform : ""
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("catseye_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.size, height : o.size, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.size, height : o.size, fill : o.colors[0], appendTo : p});
      $C({element : "path", d : ("M 0," + o.size + "h" + o.size + "v-" + o.size), fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  pirelli : function (o) {
  	var id, p;

  	o = pergola.extend({
					colors : ["#507060", "#608070"],
					size : 6,
					opacity : 1,
					transform : ""
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("pirelli_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.size, height : o.size, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.size, height : o.size, fill : o.colors[0], appendTo : p});
      $C({element : "circle", cx : o.size / 2, cy : o.size / 2, r : o.size / 2 - 1, fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  line : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["black"],
		  		size : 1,
					spacing : 1,
					width : 8,
		      opacity : 1,
					transform : ""
				}, o);

		o.height = o.spacing + o.size;

    if (!document.getElementById((id = pergola.pattern.id("line_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "line", y1 : o.height / 2 - .5, x2 : 8, y2 : o.height / 2 - .5, stroke : o.colors[0], "stroke-width" : o.size, opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  grid : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["#D8D8D8"],
		  		size : 1,
					spacing : 4,
					opacity : 1,
					transform : ""
				}, o);

		o.width = o.height = o.spacing * 2;

    if (!document.getElementById((id = pergola.pattern.id("grid_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "line", x1 : 0, y1 : o.spacing - .5, x2 : o.spacing * 2, y2 : o.spacing - .5, stroke : o.colors[0], "stroke-width" : o.size, opacity : o.opacity, appendTo : p});
      $C({element : "line", x1 : o.spacing - .5, y1 : 0, x2 : o.spacing - .5, y2 : o.spacing * 2, stroke : o.colors[0], "stroke-width" : o.size, opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  bar : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["white", "gray"],
					size : 8,
					spacing : 8,
					opacity : 1,						
					transform : ""						
				}, o);
		o.width = o.height = o.spacing + o.size;

    if (!document.getElementById((id = pergola.pattern.id("bar_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.width, height : o.height, fill : o.colors[0], appendTo : p});
      $C({element : "rect", x : o.spacing / 2, width : o.size, height : o.height, fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  grille : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["#507060", "#608070"],
					size : 6,
					opacity : 1,			
					transform : ""			
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("grille_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.size, height : o.size, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.size, height : o.size, fill : o.colors[0], appendTo : p});
      $C({element : "rect", rx : 1, width : o.size - 1, height : o.size - 1, fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  pied_de_poule : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["#EEEBEB", "#463232"],
					width : 10,
		      height : 10,
		      opacity : 1,      
		      transform : ""      
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("pied_de_poule_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.width, height : o.height, fill : o.colors[0], appendTo : p});
      pergola.use({
        parent : p,
        shape : pergola.shapes.pied_de_poule,
        attributes : {fill : o.colors[1], opacity : o.opacity}
      });
    }

    return "url(#" + id + ")";
  },


  pied_de_poule1 : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["#F4F4F4", "#282828"],
					width : 12,
		      height : 12,
		      opacity : 1,      
		      transform : ""      
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("pied_de_poule1_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.width, height : o.height, fill : o.colors[0], appendTo : p});
      $C({element : "path", d : "M0,3l3-3h3l-6,6h3l3-3v3h3l-3,3v3l6-6v3l-3,3h-9z", fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  checkers : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["white", "black"],
					size : 32,
		      opacity : 1,
					transform : ""			
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("checkers_pPattern_", o)))) {
    	var half = o.size / 2;
      p = $C({element : "pattern", id : id, width : o.size, height : o.size, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.size, height : o.size, fill : o.colors[0], appendTo : p});
      $C({element : "rect", width : half, height : half, fill : o.colors[1], opacity : o.opacity, appendTo : p});
      $C({element : "rect", x : half, y : half, width : half, height : half, fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  dot : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["none", "#404040"],
					size : 3,
					spacing : 9,
		      opacity : 1,
					transform : ""			
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("dot_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.spacing, height : o.spacing, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.spacing, height : o.spacing, fill : o.colors[0], appendTo : p});
      $C({element : "circle", cx : o.spacing / 2, cy : o.spacing / 2, r : o.size, fill : o.colors[1], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  hive : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["gold", "goldenrod"],
					width : 48,
		      height : 27.713,
		      "stroke-width" : 2,
		      opacity : 1,
		      transform : ""
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("hive_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : o.width, height : o.height, patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.width, height : o.height, fill : o.colors[0], appendTo : p});
      $C({element : "polyline", points : "32,13.856 24,27.713 8,27.713 0,13.856 8,0 24,0 32,13.856 48,13.856", fill : "none", stroke : o.colors[1], "stroke-width" : o["stroke-width"], "stroke-linecap" : "square", opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },


  ring : function (o) {
  	var id, p;

  	o = pergola.extend({
		  		colors : ["none", "#303030", "#303030"],
					size : 6,
					spacing : 10,
		      "stroke-width" : 2,
		      opacity : 1,
		      transform : ""
				}, o);

    if (!document.getElementById((id = pergola.pattern.id("ring_pPattern_", o)))) {
      p = $C({element : "pattern", id : id, width : (o.width || o.spacing * 2), height : (o.height || o.spacing * 2), patternUnits : "userSpaceOnUse", patternTransform : o.transform, appendTo : pergola.defs});
      $C({element : "rect", width : o.spacing * 2, height : o.spacing * 2, fill : o.colors[0], appendTo : p});
      $C({element : "circle", cx : 0, cy : o.spacing, r : o.size, fill : "none", stroke : o.colors[1], "stroke-width" : o["stroke-width"], opacity : o.opacity, appendTo : p});
      $C({element : "circle", cx : o.spacing * 2, cy : o.spacing, r : o.size, fill : "none", stroke : o.colors[2], "stroke-width" : o["stroke-width"], opacity : o.opacity, appendTo : p});
    }

    return "url(#" + id + ")";
  },



  id : function (id, o) {
    for (var p in o) {
			id += o[p];
		}
		return id.toXML();
  }

};

