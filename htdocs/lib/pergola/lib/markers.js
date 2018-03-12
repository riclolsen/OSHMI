
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * MARKERS LIBRARY
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


/*
 * markers list:
 *
 *   arrow1
 *   dot
 *   terminalStart
 *   terminalEnd
 *   chevron
 *   chevronDbl
 *   secant
 *   verticalBarDbl
 *   intersect
 *   cross
 *   arrowBaseArc
 *   arrowBaseArrow
 *   bullseye
 *   marker
*/



pergola.marker = {

  arrow1 : function () {
    var id = "arrow1_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 10 10",
        refX : 0,
        refY : 5,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M0,0L10,5L0,10z", fill : "#000000", stroke : "none"}));
    }

    return "url(#" + id + ")";
  },

  dot : function (p) {
    var p = p || {},
    		r = p.radius || 4,
    		fill = p.fill || "red",
    		stroke = p.stroke || "none",
				s = (stroke != "none"),
				id = "dot" + r + fill + stroke + "_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 12 12",
        viewBox : (-.5 * s) + " " + (-.5 * s) + " " + (12 + s) + " " + (12 + s),
        refX : 6,
        refY : 6,
        markerWidth : r,
        markerHeight : r,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "circle", cx : 6, cy : 6, r : 6, fill : fill, stroke : stroke}));
    }

    return "url(#" + id + ")";
  },

  terminalStart : function () {
    var id = "terminalStart_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 6 22",
        refX : 2,
        refY : 11,
        markerUnits : "userSpaceOnUse",
        markerWidth : 6,
        markerHeight : 22,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M0,0h2v8l4,3l-4,3v8h-2z", fill :"#404040", stroke : "none"}));
    }

    return "url(#" + id + ")";
  },

  terminalEnd : function () {
    var id = "terminalEnd_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 5 22",
        refX : 0,
        refY : 11,
        markerUnits : "userSpaceOnUse",
        markerWidth : 5,
        markerHeight : 22,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M1,0v22M4,0v22", fill : "none", stroke : "#404040", "stroke-width" : 2}));
    }

    return "url(#" + id + ")";
  },

  chevron : function () {
    var id = "chevron_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 9 15",
        refX : 0,
        refY : 7.5,
        markerWidth : 5,
        markerHeight : 5,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M1.5,1.5l6,6l-6,6", fill : "none", stroke : "#002068", "stroke-width" : 3, "stroke-linecap" : "round", "stroke-linejoin" : "round"}));
    }

    return "url(#" + id + ")";
  },

  chevronDbl : function () {
    var id = "chevronDbl_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 15.5 15",
        refX : 0,
        refY : 7.5,
        markerWidth : 5,
        markerHeight : 5,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M1.5,1.5l6,6l-6,6 M8,1.5l6,6l-6,6", fill : "none", stroke : "#002068", "stroke-width" : 3, "stroke-linecap" : "round", "stroke-linejoin" : "round"}));
    }

    return "url(#" + id + ")";
  },

  secant : function () {
    var id = "secant_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 12 24",
        refX : 6,
        refY : 12,
        markerUnits : "userSpaceOnUse",
        markerWidth : 12,
        markerHeight : 24,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M11.5,0L.5,24", fill : "none", stroke : "red", "stroke-width" : 1}));
    }

    return "url(#" + id + ")";
  },

  verticalBarDbl : function () {
    var id = "verticalBarDbl_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 8 24",
        refX : 4,
        refY : 12,
        markerUnits : "userSpaceOnUse",
        markerWidth : 8,
        markerHeight : 24,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M1,0v24 M6,0v24", fill : "none", stroke : "red", "stroke-width" : 2}));
    }

    return "url(#" + id + ")";
  },

  intersect : function () {
    var id = "intersect_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 12 12",
        refX : 6,
        refY : 6,
        markerUnits : "userSpaceOnUse",
        markerWidth : 12,
        markerHeight : 12,
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M12,0L0,12 M0,0L12,12", fill : "none", stroke : "brown", "stroke-width" : .25}));
    }

    return "url(#" + id + ")";
  },

  cross : function () {
    var id = "cross_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 12 12",
        refX : 6,
        refY : 6,
        markerUnits : "userSpaceOnUse",
        markerWidth : 12,
        markerHeight : 12,
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M12,0L0,12 M0,0L12,12", fill : "none", stroke : "red", "stroke-width" : 1.25}));
    }

    return "url(#" + id + ")";
  },

  arrowBaseArc : function () {
    var id = "arrowBaseArc_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 20 12",
        refX : 8,
        refY : 6,
        markerWidth : 8,
        markerHeight : 8,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M0,0c0,0 8,6 0,12 l20,-6z", fill : "#B03030", stroke : "none"}));
    }

    return "url(#" + id + ")";
  },

  arrowBaseArcSmall : function () {
    var id = "arrowBaseArc_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 20 12",
        refX : 8,
        refY : 6,
        markerWidth : 8,
        markerHeight : 8,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M0,0c0,0 8,6 0,12 l20,-6z", fill : "#303030", stroke : "none"}));
    }

    return "url(#" + id + ")";
  },

  arrowBaseArrow : function () {
    var id = "arrowBaseArrow_pMarker";

    if (!document.getElementById(id)) {
      $C({
        element : "marker",
        id : id,
        viewBox : "0 0 18 12",
        refX : 9,
        refY : 6,
        markerWidth : 8,
        markerHeight : 8,
        orient : "auto",
        appendTo : pergola.defs
      }).appendChild($C({element : "path", d : "M0,0l6,6l-6,6l18,-6z", fill : "#208020", stroke : "none"}));
    }

    return "url(#" + id + ")";
  },

  bullseye : function () {
    var id = "bullseye_pMarker";

    if (!document.getElementById(id)) {
      var marker = $C({
        element : "marker",
        id : id,
        viewBox : "0 0 40 40",
        refX : 20,
        refY : 20,
        markerWidth : 5,
        markerHeight : 5,
        appendTo : pergola.defs
      });
      $C({element : "circle", cx : 20, cy : 20, r : 16, fill : "none", stroke : "#3E3E3E", "stroke-width" : 2, appendTo : marker});
      $C({element : "circle", cx : 20, cy : 20, r : 11.5, fill : "none", stroke : "#3E3E3E", "stroke-width" : 2, appendTo : marker});
      $C({element : "path", d : "M0,20.5h8M20.5,0v8M32,20.5h8M20.5,32v8", fill : "none", stroke : "#000000", "stroke-width" : 1, appendTo : marker});
    }

    return "url(#" + id + ")";
  },

  marker : function () {
    var id = "marker_pMarker";

    if (!document.getElementById(id)) {
      var marker = $C({
        element : "marker",
        id : id,
        viewBox : "0 0 38.5 64.5",
        refX : 0,
        refY : 64,
        markerUnits : "userSpaceOnUse",
        markerWidth : 38,
        markerHeight : 64,
        appendTo : pergola.defs
      });
      $C({element : "path", d : "M28.47,2.67c0,0,0.69-1.28,1.09-2.02c0.57-1.06,9.43,3.88,8.91,4.84c-0.4,0.74-1.09,2.02-1.09,2.02L28.47,2.67z", fill : "url(#marker1)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M28.4,2.63c0,0-1.7-0.46-2.36,0.75L5.04,42.25l12.28,6.67l20.95-38.76c0.71-1.46-0.88-2.67-0.88-2.67L28.4,2.63z", fill : "url(#marker2)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M4.98,42.37l-2.75,5.1l12.28,6.67l2.81-5.2", fill : "url(#marker3)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M2.81,47.9l-0.9,1.75l10.78,5.85l1-1.84", fill : "url(#marker3)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M1.85,49.71l1.27,5.79l3.85,2.1l5.74-2.03", fill : "url(#marker3)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M3.1,55.62L0.8,59.86l3.76,2.04l2.29-4.31", fill : "url(#marker3)", stroke : "#000000", "stroke-width" : .25, appendTo : marker});
      $C({element : "path", d : "M1.37,60.33c0,0-1.48,3.61-0.57,4.11c1.01,0.55,3.07-2.77,3.07-2.77", fill : "#554400", stroke : "#000000", "stroke-width" : 1, appendTo : marker});
    }

    return "url(#" + id + ")";
  }

};



