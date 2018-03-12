
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * FILTERS LIBRARY
 *
 * Copyright (C) 2009-2014  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


pergola.filter = {

  blur : function (o) {
  	o = pergola.extend({
			x : -.5,
			y : -.5,
			stdDeviation : 2,
			dx : 3.2,
			dy : 3.2,
			"flood-color" : "#000000",
			"flood-opacity" : .4
			}, o);

    var f,
				id = (
					"blur" +
					(o.x = parseFloat(o.x) + "%") +
					(o.y = parseFloat(o.y) + "%") +
					o.stdDeviation +
					o.dx +
					o.dy +
					o["flood-color"] +
					o["flood-opacity"]
				)
				.toXML();

    if (!document.getElementById(id)) {
      f = $C({element : "filter", id : id, x : o.x, y : o.y, appendTo : pergola.defs});
      $C({element : "feGaussianBlur", stdDeviation : o.stdDeviation, appendTo : f});
      $C({element : "feOffset", result : "offsetBlur", dx : o.dx, dy : o.dy, appendTo : f});
      $C({element : "feFlood", "flood-color" : o["flood-color"], "flood-opacity" : o["flood-opacity"], appendTo : f});
      $C({element : "feComposite", in2 : "offsetBlur", result : "shadow", operator : "in", appendTo : f});
      $C({element : "feComposite", "in" : "SourceGraphic", in2 : "shadow", appendTo : f});
    }

    return "url(#" + id + ")";
  },



  specular : function (o) {
  	o = pergola.extend({
			x : 0,
			y : 0,
			stdDeviation : 2.5,
			dx : 0,
			dy : 0,
			"lighting-color" : "aliceblue",
			surfaceScale : 5,
			specularExponent : 1,
			specularConstant : 1.1,
			"fePointLight" : {x : -10000, y : -10000, z : -5000}
			}, o);

    var id = (
				"specular" +
				(o.x = parseFloat(o.x) + "%") +
				(o.y = parseFloat(o.y) + "%") +
				o.stdDeviation +
				o.dx +
				o.dy +
				o["lighting-color"] +
				o.surfaceScale +
				o.specularExponent +
				o.specularConstant +
				o["fePointLight"].x +
				o["fePointLight"].y +
				o["fePointLight"].z 
				)
				.toXML(),
				f,
				fe;

    if (!document.getElementById(id)) {
      f = $C({element : "filter", id : id, x : o.x, y : o.y, width : "100%", height : "100%", appendTo : pergola.defs});
      $C({element : "feGaussianBlur", "in" : "SourceAlpha", stdDeviation : o.stdDeviation, result : "blurredAlpha", appendTo : f});
      $C({element : "feOffset", "in" : "blurredAlpha", dx : o.dx, dy :o.dy, result : "offsetBlur", appendTo : f});
      fe = $C({element : "feSpecularLighting", "lighting-color" : o["lighting-color"], surfaceScale : o.surfaceScale, specularExponent : o.specularExponent, specularConstant : o.specularConstant, appendTo : f});
      $C({element : "fePointLight", x : o["fePointLight"].x, y : o["fePointLight"].y, z : o["fePointLight"].z, appendTo : fe});
      fe = $C({element : "feMerge", appendTo : f});
      $C({element : "feMergeNode", "in" : "offsetBlur", appendTo : fe});
      $C({element : "feMergeNode", "in" : "litPaint", appendTo : fe});
    }

    return "url(#" + id + ")";
  },


  noise : function (o) {
  	o = pergola.extend({
			x : 0,
			y : 0,
			baseFrequency : ".5 .5",
			numOctaves : 8,
			seed : 0,
			values : "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0 0 0 0 1",
			tableValues : {R : ".15 0", G : ".015 0", B : "0 0"}
			}, o);

    var id = (
				"noise" +
				(o.x = parseFloat(o.x) + "%") +
				(o.y = parseFloat(o.y) + "%") +
				o.baseFrequency +
				o.numOctaves +
				o.seed +
				o.values +
				o.tableValues.R +
				o.tableValues.G +
				o.tableValues.B 
				)
				.toXML(),
				f,
				fe;

    if (!document.getElementById(id)) {
      f = $C({element : "filter", id : id, x : o.x, y : o.y, width : "100%", height : "100%", appendTo : pergola.defs});
      $C({element : "feTurbulence", type : "fractalNoise", baseFrequency : o.baseFrequency, numOctaves : o.numOctaves, seed : o.seed, stitchTiles : "noStitch", appendTo : f});
      $C({element : "feColorMatrix", type : "matrix", values :  o.values, appendTo : f});
      fe = $C({element : "feComponentTransfer", appendTo : f});
      $C({element : "feFuncR", type : "table", tableValues : o.tableValues.R, appendTo : fe});
      $C({element : "feFuncG", type : "table", tableValues : o.tableValues.G, appendTo : fe});
      $C({element : "feFuncB", type : "table", tableValues : o.tableValues.B, appendTo : fe});
    }

    return "url(#" + id + ")";
  },



  turbulence : function (o) {
  	o = pergola.extend({
			x : 0,
			y : 0,
			baseFrequency : ".5 .5",
			numOctaves : 8,
			seed : 0,
			values : "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0 0 0 0 1",
			tableValues : {R : ".5 0", G : ".5 0", B : ".5 0"}
			}, o);

    var id = (
				"turbulence" +
				(o.x = parseFloat(o.x) + "%") +
				(o.y = parseFloat(o.y) + "%") +
				o.baseFrequency +
				o.numOctaves +
				o.seed +
				o.values +
				o.tableValues.R +
				o.tableValues.G +
				o.tableValues.B 
				)
				.toXML(),
				f,
				fe;

    if (!document.getElementById(id)) {
      f = $C({element : "filter", id : id, x : o.x, y : o.y, width : "100%", height : "100%", appendTo : pergola.defs});
      $C({element : "feTurbulence", type : "turbulence", baseFrequency : o.baseFrequency, numOctaves : o.numOctaves, seed : o.seed, stitchTiles : "noStitch", appendTo : f});
      $C({element : "feColorMatrix", type : "matrix", values : o.values, appendTo : f});
      fe = $C({element : "feComponentTransfer", appendTo : f});
      $C({element : "feFuncR", type : "table", tableValues : o.tableValues.R, appendTo : fe});
      $C({element : "feFuncG", type : "table", tableValues : o.tableValues.G, appendTo : fe});
      $C({element : "feFuncB", type : "table", tableValues : o.tableValues.B, appendTo : fe});
    }

    return "url(#" + id + ")";
  }

};

