pergola.extend(pergola.color, {

	rgbTable : [],

  rgbTablePopulate : {

    basic : function () {
      var table = pergola.color.rgbTable,
          b = 255,
          ratio,
          idx = 0,
          s = 42,
          l = 43,
          sl = s + l,
          sl2 = sl << 1,
          aX,
          rnd = Math.round,
          x, y;
    
      for (y = 0; y < b + 1; y++) {
        ratio = (b - y) / b * 6;
    
        for (x = 0, aX = l; x < aX; x++) {
          table[idx++] = [b, y + rnd(x * ratio), y];
        }
        for (aX += l; x < aX; x++) {
          table[idx++] = [y + rnd((sl - x) * ratio), b , y];
        }
        for (aX += s; x < aX; x++) {
          table[idx++] = [y, b, y + rnd((x - sl) * ratio)];
        }
        for (aX += l; x < aX; x++) {
          table[idx++] = [y, y + rnd((sl2 - x) * ratio), b];
        }
        for (aX += s; x < aX; x++) {
          table[idx++] = [y + rnd((x - sl2) * ratio), y, b];
        }
        for (aX += l; x < aX; x++) {
          table[idx++] = [b, y, y + rnd((b - x) * ratio)];
        }
      }
    },

    extended : function () {
      var table = pergola.color.rgbTable,
          b = 255,
          l = b + 1,
          ratio,
          idx = 0,
          rnd = Math.round,
          x, y;

      for (y = 0; y < l; y++) {
        ratio = (b - y) / b;
    
        for (x = 0; x < l; x++) {
          table[idx++] = [b, y + rnd(x * ratio), y];
        }
        for (x = 0; x < l; x++) {
          table[idx++] = [y + rnd((b - x) * ratio), b , y];
        }
        for (x = 0; x < l; x++) {
          table[idx++] = [y, b, y + rnd(x * ratio)];
        }
        for (x = 0; x < l; x++) {
          table[idx++] = [y, y + rnd((b - x) * ratio), b];
        }
        for (x = 0; x < l; x++) {
          table[idx++] = [y + rnd(x * ratio), y, b];
        }
        for (x = 0; x < l; x++) {
          table[idx++] = [b, y, y + rnd((b - x) * ratio)];
        }
      }
    }

  },

  convert : function (c, format) {
    var r, g, b,
        isKeyword = false,
				conv = pergola.color.rgbToHex;

    for (var a in pergola.color.keywordNames) {
      if (c == a) {
        c = pergola.color.keywordNames[a];
        isKeyword = true;
        break;
      }
    }
    if (!isKeyword) {
      for (var a in pergola.customColorNames) {
        if (c == a) {
          c = pergola.customColorNames[a];
          break;
        }
      }
    }

    if (c.charAt(0) == "#") {
      c = pergola.color.hexToRGB(c);
      r = c[0];
      g = c[1];
      b = c[2];
    }
    else if (c.indexOf("rgb") != -1) {
    	if (c.indexOf("%") != -1) {
	      c = c.slice(4, c.length - 1).split(",");
	      r = Math.round(parseInt(c[0]) * 2.55);
	      g = Math.round(parseInt(c[1]) * 2.55);
	      b = Math.round(parseInt(c[2]) * 2.55);
	    }
	    else {			    
	      c = c.slice(4, c.length - 1).split(",");
	      r = Number(c[0]);
	      g = Number(c[1]);
	      b = Number(c[2]);
	    }	      
    }
    else if (c.indexOf("hsl") != -1) {
    	c = c.slice(4, c.length - 1).split(",");
			c = pergola.color.hslToRgb(parseInt(c[0]), parseInt(c[1]), parseInt(c[2]));
			r = c[0];
      g = c[1];
      b = c[2];
		}
    else {
      alert(pergola.messages.wrongColorFormat);
      return [0, 0, 0];
    }
    if (format) {
      if (format == "rgb") return ("rgb(" + r + " " + g + " " + b + " " + ")");
      else if (format == "hex") {
        return ("#" + conv(r) + conv(g) + conv(b));
      }
    }
    else return [r, g, b];
  },

	hslToRgb: function (h, s, l) {

		h /= 360;
    s /= 100;
    l /= 100;

    var r, g, b,
		    i = Math.floor(h * 6),
		    f = h * 6 - i,
		    p = l * (1 - s),
		    q = l * (1 - f * s),
		    t = l * (1 - (1 - f) * s);

    switch (i % 6) {
      case 0: r = l, g = t, b = p; break;
      case 1: r = q, g = l, b = p; break;
      case 2: r = p, g = l, b = t; break;
      case 3: r = p, g = q, b = l; break;
      case 4: r = t, g = p, b = l; break;
      case 5: r = l, g = p, b = q; break;
    }

    return [Math.floor(r * 255), Math.floor(g * 255), Math.floor(b * 255)];
	},

  hexToRGB : function (c) {
    var r = parseInt("0x" + c.substring(1, 3)),
        g = parseInt("0x" + c.substring(3, 5)),
        b = parseInt("0x" + c.substring(5, 7));
    return [r, g, b];
  },

  rgbToHex : function (n) {
    var c = n.toString(16);
    if(c.length > 2) c = "FF";
    if(c.length == 1) c = "0" + c;
    return c;
  },

  lab : function () {
    var args = arguments[0],
        r = args[0],
        g = args[1],
        b = args[2];

    pergola.neutral = (r == g && r == b);
    pergola.themeLuminosity = pergola.color.luminosity(r, g, b);
    pergola.colorComponents = {r : r, g : g, b : b};
    pergola.ingredients = pergola.toArray(pergola.colorComponents).sort(function (a,b) { return b - a });
  },

  luminosity : function (r, g, b) {
    return Math.round((r + g + b) / 3);
  },

  shade : function (cents, f) {
    var shade = "#",
        colorComponents = pergola.colorComponents,
        ingredients = pergola.ingredients,
        average;

    if (pergola.neutral) {
      average = cents[1];
      for (var i = 0; i < cents.length; i++) {
        cents[i] = average;
      }
    }
    if (f) {
      var i = 0,
          c,
          add;

      if (typeof cents == "number") {
        average = pergola.themeLuminosity;
        for (var i = 0; i < 3; i++) {
          c = average * (cents / 100);
          if (c > 255) c = 255;
          if (f < 0) add = (average / 100) * f;
          else add = ((255 - c) / 100) * f;
          if (c + add < 0) add = -c;
          shade += pergola.color.rgbToHex(Math.round(c + add)).toUpperCase();
        }
      }
      else {
        for (var a in colorComponents) {
          for (var b in ingredients) {
            if (colorComponents[a] == ingredients[b]) {
              c = colorComponents[a] * (cents[b] / 100);
              if (c > 255) c = 255;
              if (f < 0) add = (colorComponents[a] / 100) * f;
              else add = ((255 - c) / 100) * f;
              if (c + add < 0) add = -c;
              shade += pergola.color.rgbToHex(Math.round(c + add)).toUpperCase();
              break;
            }
          }
        }
      }
    }
    else {
      for (var a in colorComponents) {
        for (var b in ingredients) {
          if (colorComponents[a] == ingredients[b]) {
            shade += pergola.color.rgbToHex(Math.round(colorComponents[a] * (cents[b] / 100))).toUpperCase();
            break;
          }
        }
      }
    }
    return shade;
  },

  keywordNames : {
    aliceblue : "rgb(240, 248, 255)",
    antiquewhite : "rgb(250, 235, 215)",
    aqua : "rgb(0, 255, 255)",
    aquamarine : "rgb(127, 255, 212)",
    azure : "rgb(240, 255, 255)",
    beige : "rgb(245, 245, 220)",
    bisque : "rgb(255, 228, 196)",
    black : "rgb( 0, 0, 0)",
    blanchedalmond : "rgb(255, 235, 205)",
    blue : "rgb( 0, 0, 255)",
    blueviolet : "rgb(138, 43, 226)",
    brown : "rgb(165, 42, 42)",
    burlywood : "rgb(222, 184, 135)",
    cadetblue : "rgb( 95, 158, 160)",
    chartreuse : "rgb(127, 255, 0)",
    chocolate : "rgb(210, 105, 30)",
    coral : "rgb(255, 127, 80)",
    cornflowerblue : "rgb(100, 149, 237)",
    cornsilk : "rgb(255, 248, 220)",
    crimson : "rgb(220, 20, 60)",
    cyan : "rgb( 0, 255, 255)",
    darkblue : "rgb( 0, 0, 139)",
    darkcyan : "rgb( 0, 139, 139)",
    darkgoldenrod : "rgb(184, 134, 11)",
    darkgray : "rgb(169, 169, 169)",
    darkgreen : "rgb( 0, 100, 0)",
    darkgrey : "rgb(169, 169, 169)",
    darkkhaki : "rgb(189, 183, 107)",
    darkmagenta : "rgb(139, 0, 139)",
    darkolivegreen : "rgb( 85, 107, 47)",
    darkorange : "rgb(255, 140, 0)",
    darkorchid : "rgb(153, 50, 204)",
    darkred : "rgb(139, 0, 0)",
    darksalmon : "rgb(233, 150, 122)",
    darkseagreen : "rgb(143, 188, 143)",
    darkslateblue : "rgb( 72, 61, 139)",
    darkslategray : "rgb( 47, 79, 79)",
    darkslategrey : "rgb( 47, 79, 79)",
    darkturquoise : "rgb( 0, 206, 209)",
    darkviolet : "rgb(148, 0, 211)",
    deeppink : "rgb(255, 20, 147)",
    deepskyblue : "rgb( 0, 191, 255)",
    dimgray : "rgb(105, 105, 105)",
    dimgrey : "rgb(105, 105, 105)",
    dodgerblue : "rgb( 30, 144, 255)",
    firebrick : "rgb(178, 34, 34)",
    floralwhite : "rgb(255, 250, 240)",
    forestgreen : "rgb( 34, 139, 34)",
    fuchsia : "rgb(255, 0, 255)",
    gainsboro : "rgb(220, 220, 220)",
    ghostwhite : "rgb(248, 248, 255)",
    gold : "rgb(255, 215, 0)",
    goldenrod : "rgb(218, 165, 32)",
    gray : "rgb(128, 128, 128)",
    grey : "rgb(128, 128, 128)",
    green : "rgb( 0, 128, 0)",
    greenyellow : "rgb(173, 255, 47)",
    honeydew : "rgb(240, 255, 240)",
    hotpink : "rgb(255, 105, 180)",
    indianred : "rgb(205, 92, 92)",
    indigo : "rgb( 75, 0, 130)",
    ivory : "rgb(255, 255, 240)",
    khaki : "rgb(240, 230, 140)",
    lavender : "rgb(230, 230, 250)",
    lavenderblush : "rgb(255, 240, 245)",
    lawngreen : "rgb(124, 252, 0)",
    lemonchiffon : "rgb(255, 250, 205)",
    lightblue : "rgb(173, 216, 230)",
    lightcoral : "rgb(240, 128, 128)",
    lightcyan : "rgb(224, 255, 255)",
    lightgoldenrodyellow : "rgb(250, 250, 210)",
    lightgray : "rgb(211, 211, 211)",
    lightgreen : "rgb(144, 238, 144)",
    lightgrey : "rgb(211, 211, 211)",
    lightpink : "rgb(255, 182, 193)",
    lightsalmon : "rgb(255, 160, 122)",
    lightseagreen : "rgb( 32, 178, 170)",
    lightskyblue : "rgb(135, 206, 250)",
    lightslategray : "rgb(119, 136, 153)",
    lightslategrey : "rgb(119, 136, 153)",
    lightsteelblue : "rgb(176, 196, 222)",
    lightyellow : "rgb(255, 255, 224)",
    lime : "rgb( 0, 255, 0)",
    limegreen : "rgb( 50, 205, 50)",
    linen : "rgb(250, 240, 230)",
    magenta : "rgb(255, 0, 255)",
    maroon : "rgb(128, 0, 0)",
    mediumaquamarine : "rgb(102, 205, 170)",
    mediumblue : "rgb( 0, 0, 205)",
    mediumorchid : "rgb(186, 85, 211)",
    mediumpurple : "rgb(147, 112, 219)",
    mediumseagreen : "rgb( 60, 179, 113)",
    mediumslateblue : "rgb(123, 104, 238)",
    mediumspringgreen : "rgb( 0, 250, 154)",
    mediumturquoise : "rgb( 72, 209, 204)",
    mediumvioletred : "rgb(199, 21, 133)",
    midnightblue : "rgb( 25, 25, 112)",
    mintcream : "rgb(245, 255, 250)",
    mistyrose : "rgb(255, 228, 225)",
    moccasin : "rgb(255, 228, 181)",
    navajowhite : "rgb(255, 222, 173)",
    navy : "rgb( 0, 0, 128)",
    oldlace : "rgb(253, 245, 230)",
    olive : "rgb(128, 128, 0)",
    olivedrab : "rgb(107, 142, 35)",
    orange : "rgb(255, 165, 0)",
    orangered : "rgb(255, 69, 0)",
    orchid : "rgb(218, 112, 214)",
    palegoldenrod : "rgb(238, 232, 170)",
    palegreen : "rgb(152, 251, 152)",
    paleturquoise : "rgb(175, 238, 238)",
    palevioletred : "rgb(219, 112, 147)",
    papayawhip : "rgb(255, 239, 213)",
    peachpuff : "rgb(255, 218, 185)",
    peru : "rgb(205, 133, 63)",
    pink : "rgb(255, 192, 203)",
    plum : "rgb(221, 160, 221)",
    powderblue : "rgb(176, 224, 230)",
    purple : "rgb(128, 0, 128)",
    red : "rgb(255, 0, 0)",
    rosybrown : "rgb(188, 143, 143)",
    royalblue : "rgb( 65, 105, 225)",
    saddlebrown : "rgb(139, 69, 19)",
    salmon : "rgb(250, 128, 114)",
    sandybrown : "rgb(244, 164, 96)",
    seagreen : "rgb( 46, 139, 87)",
    seashell : "rgb(255, 245, 238)",
    sienna : "rgb(160, 82, 45)",
    silver : "rgb(192, 192, 192)",
    skyblue : "rgb(135, 206, 235)",
    slateblue : "rgb(106, 90, 205)",
    slategray : "rgb(112, 128, 144)",
    slategrey : "rgb(112, 128, 144)",
    snow : "rgb(255, 250, 250)",
    springgreen : "rgb( 0, 255, 127)",
    steelblue : "rgb( 70, 130, 180)",
    tan : "rgb(210, 180, 140)",
    teal : "rgb( 0, 128, 128)",
    thistle : "rgb(216, 191, 216)",
    tomato : "rgb(255, 99, 71)",
    turquoise : "rgb( 64, 224, 208)",
    violet : "rgb(238, 130, 238)",
    wheat : "rgb(245, 222, 179)",
    white : "rgb(255, 255, 255)",
    whitesmoke : "rgb(245, 245, 245)",
    yellow : "rgb(255, 255, 0)",
    yellowgreen : "rgb(154, 205, 50)"
  }
});