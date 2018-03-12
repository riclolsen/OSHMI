
/*
 * This set of extensions is used internally.
 * The function names should not be in conflict with 
 * larger extension libraries that you may need to use.
 * 
 * Number.toRad and Number.toDeg are standard
 * and if they are overrridden by valid equivalents
 * and Pergola hangs at runtime, then that means
 * the overriding functions are faulty.     
*/



pergola.extend(Number.prototype, {

  evenToOdd : function () {
    return this - (this % parseInt(this / 2) == 0) | 0;
  },

  oddToEven : function () {
    return this - this % parseInt(this / 2);
  },

  isEven : function () {
    return this % 2 == 0;
  },

  trim : function (p) {
    return parseFloat(this.toFixed(p));
  },

  toRad : function () {
    return this * Math.PI / 180;
  },

  toDeg : function () {
    return this * 180 / Math.PI;
  }

});




pergola.extend(String.prototype, {

  xmlNameStartChar : /^["-""."0-9\u00B7\u0300-\u036F\u203F-\u2040]/,

  xmlNameChar : /[^A-Za-z":""_"\u002D\u00C0-\u00D6\u00D8-\u00F6\u00F8-\u02FF\u0370-\u037D\u037F-\u1FFF\u200C-\u200D\u2070-\u218F\u2C00-\u2FEF\u3001-\uD7FF\uF900-\uFDCF\uFDF0-\uFFFD\u10000-\uEFFFF]/g,

  toXML : function () {
    return this.replace(this.xmlNameStartChar, "_").replace(this.xmlNameChar, "_");
  },

  xmlNameGenerator : function () {
    var xml = pergola.xmlName,
        n;

    function generate() {
      n = Math.random().toString(16).toUpperCase();
      return "XML_" + n.substring(n.indexOf(".") + 1);
    }

    while (true) {
      if (!xml.exist(n = generate())) return xml.add(this + n); 
    }
  },

  width : function (fS, fF, fW, lS, k) {
    var proto = pergola.Class.prototype,
				t, w;

    fS = fS || proto['font-size'];  
    fF = fF || proto['font-family'];
    fW = fW || "normal";
    lS = lS || "normal";
    k = k || "auto";

    t = $C({element : "text", "font-size" : fS, "font-family" : fF, "font-weight" : fW, 'letter-spacing' : lS, kerning : k, textNode : this, appendTo : pergola.doc});
    w = Math.round(t.getBBox().width);
    pergola.doc.removeChild(t);

    return w;
  },

  height : function (fS, fF) {
    var proto = pergola.Class.prototype;
    fS = fS || proto['font-size'];  
    fF = fF || proto['font-family'];
    var t = $C({element : "text", "font-size" : fS, "font-family" : fF, textNode : this, appendTo : pergola.doc});
// FF and ASV return the height of the highest glyph of the string, Opera and Chrome return the line height.
    var h = Math.round(t.getBBox().height);
    pergola.doc.removeChild(t);
    return h;
  },

  trim : function (n) {
    return this.substring(0, (this.length - n));
  },

  truncate : function (width, fS, fF, fW, lS, k) {
    var proto = pergola.Class.prototype,
        string = this,
        tWidth;

    fS = fS || proto['font-size'];
    fF = fF || proto['font-family'];
    fW = fW || "normal";
    lS = lS || "normal";
    k = k || "auto";
    tWidth = this.width(fS, fF, fW, lS, k);
    while (tWidth > width) {
//      string = string.substring(0, (string.length - 4)) + "...";
      string = string.trim(4) + "...";
      tWidth = string.width(fS, fF, fW, lS, k);
    }
    return string;
  },

  centerVertical : function (height, fS, fF) {
    var proto = pergola.Class.prototype,
        h;

    fS = fS || proto['font-size'];  
    fF = fF || proto['font-family'];
    h = this.height(fS, fF);
    return (height + h) / 2;
  },

// Applies to color strings (any valid format).
// If you try this on ordinary text it won't get any brighter :)
  brighten : function (f) {
    var hex = pergola.color.rgbToHex,
        c = pergola.color.convert(this);

    if (!c) return this;
		f = (f || .5) + 1;
    return "#" + hex(Math.round(c[0] * f)) + hex(Math.round(c[1] * f)) + hex(Math.round(c[2] * f));
  },

// Applies to color strings (any valid format)
  darken : function (f) {
    var hex = pergola.color.rgbToHex,
        c = pergola.color.convert(this);

    if (!c) return this;
    f = f || .5;
    return "#" + hex(Math.round(c[0] * f)) + hex(Math.round(c[1] * f)) + hex(Math.round(c[2] * f));
  }

});
