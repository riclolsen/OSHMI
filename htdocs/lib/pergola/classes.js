
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *  
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


pergola.extend(pergola, {

  Background : function () {
    pergola.Background.baseConstructor.call(this, arguments[0]);
  },

  Button : function () {
    pergola.Button.baseConstructor.call(this, arguments[0]);
  },

  Caption : function () {
    pergola.Caption.baseConstructor.call(this, arguments[0]);
  },

  CheckBox : function () {
    pergola.CheckBox.baseConstructor.call(this, arguments[0]);
  },

  ColorBoxSelector : function () {
    pergola.ColorBoxSelector.baseConstructor.call(this, arguments[0]);
  },

  ColorPicker : function () {
    pergola.ColorPicker.baseConstructor.call(this, arguments[0]);
  },

  ColorPickerExtended : function () {
    pergola.ColorPickerExtended.baseConstructor.call(this, arguments[0]);
  },

  ComboBox : function () {
    pergola.ComboBox.baseConstructor.call(this, arguments[0]);
  },

  CommandButton : function () {
    pergola.CommandButton.baseConstructor.call(this, arguments[0]);
  },

  Coordinates : function () {
    pergola.Coordinates.baseConstructor.call(this, arguments[0]);
  },

  DialogButton : function () {
    pergola.DialogButton.baseConstructor.call(this, arguments[0]);
  },

  Doc : function () {
    pergola.Doc.baseConstructor.call(this, arguments[0]);
  },

  Dragarea : function () {
    pergola.Dragarea.baseConstructor.call(this, arguments[0]);
  },

  Frame : function () {
    pergola.Frame.baseConstructor.call(this, arguments[0]);
  },

  Input : function () {
    pergola.Input.baseConstructor.call(this, arguments[0]);
  },

  InputCursor : function () {
    pergola.InputCursor.baseConstructor.call(this, arguments[0]);
  },

  Layout : function () {
    pergola.Layout.baseConstructor.call(this, arguments[0]);
  },

  Legend : function () {
    pergola.Legend.baseConstructor.call(this, arguments[0]);
  },

  ListItem : function () {
    pergola.ListItem.baseConstructor.call(this, arguments[0]);
  },

  Menu : function () {
    pergola.Menu.baseConstructor.call(this, arguments[0]);
  },

  Menubar : function () {
    pergola.Menubar.baseConstructor.call(this, arguments[0]);
  },

  MenuItem : function () {
    pergola.MenuItem.baseConstructor.call(this, arguments[0]);
  },

  MenuList : function () {
    pergola.MenuList.baseConstructor.call(this, arguments[0]);
  },

  Panel : function () {
    pergola.Panel.baseConstructor.call(this, arguments[0]);
  },

  PopupList : function () {
    pergola.PopupList.baseConstructor.call(this, arguments[0]);
  },

  PopupListItem : function () {
    pergola.PopupListItem.baseConstructor.call(this, arguments[0]);
  },

  Preview : function () {
    pergola.Preview.baseConstructor.call(this, arguments[0]);
  },

  Progress : function () {
    pergola.Progress.baseConstructor.call(this, arguments[0]);
  },

  QuickTip : function () {
    pergola.QuickTip.baseConstructor.call(this, arguments[0]);
  },

  RadioButton : function () {
    pergola.RadioButton.baseConstructor.call(this, arguments[0]);
  },

  RubberBox : function () {
    pergola.RubberBox.baseConstructor.call(this, arguments[0]);
  },

  Scrollbar : function () {
    pergola.Scrollbar.baseConstructor.call(this, arguments[0]);
  },

  ScrollSlider : function () {
    pergola.ScrollSlider.baseConstructor.call(this, arguments[0]);
  },

  Selector : function () {
    pergola.Selector.baseConstructor.call(this, arguments[0]);
  },

  Separator : function () {
    pergola.Separator.baseConstructor.call(this, arguments[0]);
  },

  Slider : function () {
    pergola.Slider.baseConstructor.call(this, arguments[0]);
  },

  StatusBar : function () {
    pergola.StatusBar.baseConstructor.call(this, arguments[0]);
  },

  Tab : function () {
    pergola.Tab.baseConstructor.call(this, arguments[0]);
  },

  TabsDock : function () {
    pergola.TabsDock.baseConstructor.call(this, arguments[0]);
  },

  Taskbar : function () {
    pergola.Taskbar.baseConstructor.call(this, arguments[0]);
  },

  ToolBar : function () {
    pergola.ToolBar.baseConstructor.call(this, arguments[0]);
  },

  ToolButton : function () {
    pergola.ToolButton.baseConstructor.call(this, arguments[0]);
  },

  TopBar : function () {
    pergola.TopBar.baseConstructor.call(this, arguments[0]);
  },

  Window : function () {
    pergola.Window.baseConstructor.call(this, (arguments[0] || pergola.Window.unnamed()));
  },

  WindowTab : function () {
    pergola.WindowTab.baseConstructor.call(this, arguments[0]);
  },

  WindowTopBar : function () {
    pergola.WindowTopBar.baseConstructor.call(this, arguments[0]);
  }

});



pergola.subclass(
  [
    pergola.Background,
    pergola.Button,
    pergola.CheckBox,
    pergola.Caption,
    pergola.ColorBoxSelector,
    pergola.ColorPicker,
    pergola.ColorPickerExtended,
    pergola.ComboBox,
    pergola.Coordinates,
    pergola.Doc,
    pergola.Dragarea,
    pergola.Frame,
    pergola.Input,
    pergola.InputCursor,
    pergola.Layout,
    pergola.Legend,
    pergola.ListItem,
    pergola.Menu,
    pergola.Menubar,
    pergola.MenuItem,
    pergola.MenuList,
    pergola.Panel,
    pergola.PopupList,
    pergola.Preview,
    pergola.Progress,
    pergola.QuickTip,
    pergola.RadioButton,
    pergola.RubberBox,
    pergola.Scrollbar,
    pergola.ScrollSlider,
    pergola.Selector,
    pergola.Separator,
    pergola.Slider,
    pergola.StatusBar,
    pergola.Tab,
    pergola.TabsDock,
    pergola.Taskbar,
    pergola.ToolBar,
    pergola.TopBar,
    pergola.Window
  ],

  pergola.Class
);


pergola.subclass(
  [
    pergola.ToolButton,
    pergola.WindowTab,
    pergola.DialogButton
  ],

  pergola.Button
);

pergola.subclass(pergola.CommandButton, pergola.ToolButton);
pergola.subclass(pergola.PopupListItem, pergola.ListItem);
pergola.subclass(pergola.WindowTopBar, pergola.TopBar);





(function () {
  var extend = pergola.extend,
      pAttributes = pergola.presentationAttributes;


  extend(pergola.Background.prototype, {
  	parent : null,
    width : "100%",
    height : "100%",
    fill : pAttributes.background.fill,
    stroke : pAttributes.background.stroke,
    opacity : pAttributes.background.opacity,
    'xlink:href' : pAttributes.background['xlink:href'],
    display : "block",
    'pointer-events' : "visiblePainted",
    preserveAspectRatio : "xMidYMid slice",
    cursor : "default",

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.parent) {
        alert (pergola.messages.missingParent[0] + this.name + "Background instance");
        return;
      };
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.rect = $C({element : "rect", width : this.width, height : this.height, fill : this.fill, stroke : this.stroke, 'pointer-events' : this['pointer-events'], appendTo : this.container});
      this.image = $C({element : "image", x : .5, y : .5, width : this.width, height : this.height, "xlink:href" : this['xlink:href'], preserveAspectRatio : this.preserveAspectRatio, display : this.display, opacity : this.opacity, 'pointer-events' : "none", appendTo : this.container});

      return this;
    }
  });


  extend(pergola.Button.prototype, {
    fill : pAttributes.button.fill,
    "fill-opacity" : 0,
    stroke : pAttributes.button.stroke,
    maskFill : pAttributes.button.maskFill,
    large : pAttributes.button.large,
    small : pAttributes.button.small,
    hasVisualEffects : true,
    hasTextEffect : true,
    textFill : pAttributes.text.fill,
    textFillInverse : pAttributes.text.fillInverse,

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var eType = evt.type;
      if (eType == "mouseover") {
        this.rollover(evt);
        if (this.quickTip) {
          this.quickTipInit(evt);
          this.mouse = pergola.mousePoint(evt);
        }
      }
      else if (eType == "mousedown" || eType == "mouseout") {
        if (this.quickTip) {
          if (this.quickTip.timer) this.quickTip.timer.clear();
          this.quickTip.hide();
        }
        if (!this.selected && eType == "mouseout") this.rollover(evt);
      }
      else if (eType == "mousemove") this.mouse = pergola.mousePoint(evt);
      if (this.ev) {
        for (var e in this.ev) {
          if (this.ev[e] == eType) this.checkUserFunction(evt);
          break;
        }
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      var x, 
					w = this.width,
          h = this.height,
          s = this.size,
          l,
          buffer,
          bBox,
          cx,
          cy,
          pos,
          old,
          newString,
					strokeW,
					transform,
					element,
					shape,
					attributes = {stroke : this.stroke};

      if ((w || h) && s) {
        alert (pergola.messages.header + this.name + pergola.messages.conflictingProperties + pergola.messages.wrongButton);
        return;
      }
      else if (s && (s != "small" && s != "large")) {
        alert (pergola.messages.header + this.name + pergola.messages.wrongButtonSize + s);
        return;
      }
      else if (w || h) {
        this.width = w || this.large.width;
        this.height = h || this.large.height;
      }
      else if ((!w || !h) && !s || s == "large") {
        this.width = this.large.width;
        this.height = this.large.height;
      }
      else if (s == "small") {
        this.width = this.small.width;
        this.height = this.small.height;
      }
  
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.buttonGroup = $C({element : "g", appendTo : this.container});

      if (this.extra) {
				transform = this.extra.transform;
				delete this.extra.transform;
				pergola.extend(attributes, this.extra);
			}


			if (this.shape) {
        if (typeof this.shape != 'object') {
          alert (pergola.messages.wrongButtonShape);
          return;
        }
				this.mask = pergola.extend({}, this.shape);
				this.button = pergola.extend({}, this.shape);
				pergola.extend(this.mask, attributes);				
				pergola.extend(this.button, attributes);
      }
      else {
      	this.mask = pergola.extend({element : "rect", width : this.width, height : this.height}, attributes);
				this.button = pergola.extend({element : "rect", width : this.width, height : this.height}, attributes);
      	
	      if (this.button['stroke-width']) {
	        strokeW = this.button['stroke-width'];
	        if (typeof strokeW == "string") {
	          if (strokeW.indexOf("px") != -1) strokeW = parseInt(strokeW);
	          else strokeW = null;
	        }
	      }
	      else strokeW = 1;
	      if (strokeW) x = this.setPositionToStrokeWidth(strokeW);
	      if (x) this.button.x = this.mask.x = this.button.y = this.mask.y = x;
      }

      this.mask.fill = this.maskFill;
      this.button.fill = this.fill;
      this.button["fill-opacity"] = this["fill-opacity"];
      this.mask.appendTo = this.button.appendTo = this.buttonGroup;


			this.mask = $C(this.mask);
      this.button = $C(this.button);

      if (transform) {
        pos = transform.indexOf("rotate");

        if (pos != -1) {
          l = "rotate(".length;
      		buffer = transform.substring((pos + l), transform.indexOf(")", (pos + l)));
      		bBox = this.button.getBBox();
          cx = bBox.width / 2;
          cy = bBox.height / 2;
          old = "rotate(" + buffer + ")";
          newString = "rotate(" + buffer + " " + cx.trim(3) + " " + cy.trim(3) + ")";
          transform = transform.replace(old, newString);
        }

        this.buttonGroup.setAttributeNS(null, "transform", transform);
      }

      if (this.shape) {
        bBox = this.button.getBBox();
        this.width = bBox.width;
        this.height = bBox.height;
      }

      this.registerEvents(this.button, "mousedown");

      if (this.owner) this.registerEvents(this.button, "mousedown", this.owner);
      if (this.ev) {
        if (typeof this.ev == "string") this.ev = [this.ev];
        this.registerEvents(this.button, this.ev);
      }

      if (this.hasVisualEffects) this.registerEvents(this.button, ["mouseover", "mouseout"]);

      if (this.text) {
        this.textFill = this.text.fill || this.textFill;
        this.text = $C(pergola.extend({element : 'text', appendTo : this.container}, this.text));
      }

      if (this.symbol) this.symbol = pergola.symbol.call(this, this.symbol);

      if (this.image) {
        this.image = $C(pergola.extend({element : "image", opacity : 0, "pointer-events" : "none", appendTo : this.container}, this.image));
      }

      if (this.quickTip) this.quickTipFormat();

      this.sweep(["size", "shape", "extra"]);

      return this;
    }
  });


  extend(pergola.Caption.prototype, {
  	position : "left",
    "text-anchor" : "end",
    textFill : pAttributes.text.fill,

    build : function () {
      var bbox,
          x, y;

      pergola.extend(this, arguments[0]);

      if (this.position == "right") {
	      this["text-anchor"] = "start";
	      this.x = this.x || this.owner.container.getBBox().width + 12;
	    }
	    else {
	      this.x = this.x || -12;
	    }

      this.element = $C({element : "text", x : this.x, y : this.y, "font-family" : this["font-family"], "font-size" : this["font-size"], fill : this.textFill, "text-anchor" : this["text-anchor"], "pointer-events" : "none", textNode : this.textNode, appendTo : this.parent});
      bbox = this.element.getBBox();
      x = (this["text-anchor"] == "start") ? this.x - 2 : this.x - bbox.width - 2;
      y = this.y - this["font-size"];
      $C({element : "rect", x : x, y : y, width : bbox.width + 4, height : bbox.height, fill : "none", "pointer-events" : "all", appendTo : this.parent});

      return this;
    }
  });



  extend(pergola.CheckBox.prototype, {
    width : pAttributes.checkBox.width,
    height : pAttributes.checkBox.height,
    fill : pAttributes.checkBox.fill,
    checked : false,

    handleEvent : function (evt) {
      var display;

      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null, true)) return;
      if (this.checked) {
        this.checked = false;
        if (this.owner) {
          for (var a in this.owner.checked) if (this.owner.checked[a] == this) this.owner.checked.splice(a, 1);
        }
        display = "none";
      }
      else {
        this.checked = true;
        if (this.owner) this.owner.checked.push(this);
        display = "block";
      }
      
      this.checkmark.setAttributeNS(null, "display", display);
      if (this.fn) this.checkUserFunction(evt);
      if (this.owner && this.owner.fn) this.owner.fn();
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (this.owner && !this.owner.checked) this.owner.checked = [];
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.registerEvents(this.container, "mouseup");
      if (this.ev) {
        if (typeof this.ev == "string") this.ev = [this.ev];
        this.registerEvents(this.container, this.ev);
      }
      $C({element : "rect", x : .5, y : .5, width : this.width, height : this.height, fill : this.fill, stroke : this.stroke, appendTo : this.container});
      this.checkmark = pergola.symbol.call(this, {
        symbol : pergola.symbols.checkmark,
        x : 3,
        y : 3
      });
      this.checkmark = this.checkmark.firstChild;
      var display = this.checked ? "block" : "none";
      this.checkmark.setAttributeNS(null, "display", display);

      if (this.caption) {
      	this.caption = new pergola.Caption().build(pergola.extend(this.caption, {owner : this, parent : this.container}));
			}

      return this;
    }
  });


  extend(pergola.ColorBoxSelector.prototype, {
    fill : pAttributes.colorBoxSelector.fill,
    stroke : pAttributes.colorBoxSelector.stroke,
    width : pAttributes.colorBoxSelector.width,
    height : pAttributes.colorBoxSelector.height,

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Selector.handle) pergola.Selector.handle.close(evt);
      pergola.colorpicker.init({
        user : this,
        color : this.fill
      });
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      var w = this.width,
          h = this.height;

      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.rect = $C({element : "rect", width : w, height : h, fill : this.fill, appendTo : this.container});
      this.registerEvents(this.container, "mouseup");
      $C({element : "path", d : "M1 " + h + "v-" + (h - 1) + "h" + (w - 1), fill : "none", stroke : "#C0C0C0", 'stroke-width' : 2, 'pointer-events' : "none", appendTo : this.container});
      $C({element : "path", d : "M0 " + h + "h" + w + "v-" + h + "l-2 2v" + (h - 4) + "h-" + (w - 4) + "z", fill : "#E8E8E8", 'pointer-events' : "none", appendTo : this.container});
      
      if (this.caption) {
        this.caption = new pergola.Caption().build(pergola.extend(this.caption, {owner : this, parent : this.container}));
			}

      return this;
    }
  });


  extend(pergola.ColorPicker.prototype, {
    width : 256,
    height : 256,
    max : 255,

    release : function () {
      var r = this.RGB.r,
					g = this.RGB.g,
					b = this.RGB.b,
					hexR = this.toHex(r),
					hexG = this.toHex(g),
					hexB = this.toHex(b);

    	this.input.hex.value = (hexR + hexG + hexB).toUpperCase();
      this.input.r.value = r;
      this.input.g.value = g;
      this.input.b.value = b;
			this.color = "#" + this.input.hex.value;
    },

    spectrumIinitDrag : function (evt) {
      var point = pergola.mousePoint(evt);
      return {
        callback : this.spectrumScan,
        handle : this,
        offset : {
					x : point.x - this.drop.x,
					y : point.y - this.drop.y
				},
        updateCoordinates : false,
        cursor : "crosshair"
      };
    },

    spectrumScan : function (evt) {
      var handle = this.handle,
          drop = handle.drop,
          point = pergola.mousePoint(evt);

      drop.x = Math.round(point.x - this.offset.x);
      drop.y = Math.round(point.y - this.offset.y);
      if (drop.x < 0) drop.x = 0;
      else if (drop.x > handle.max) drop.x = handle.max;
      if (drop.y < 0) drop.y = 0;
      else if (drop.y > handle.max) drop.y = handle.max;
      handle.setDrop();
    },

    setDrop : function () {
      var x = this.drop.x,
          y = this.drop.y,
					rgb;

      this.colorIndex = y * 256 + x;
      this.hue = pergola.color.rgbTable[this.colorIndex];
      this.RGB.r = this.valueFromBrightness(this.hue[0]);
      this.RGB.g = this.valueFromBrightness(this.hue[1]);
      this.RGB.b = this.valueFromBrightness(this.hue[2]);
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [this.RGB.r, this.RGB.g, this.RGB.b] + ")");
      this.brightnessMask.setAttributeNS(null, "stop-color", "rgb(" + this.hue + ")");
      this.drop.container.setAttributeNS(null,"transform", "translate(" + x + " " + y + ")");
      this.valuesDisplay(this.RGB.r, this.RGB.g, this.RGB.b);
    },

    brightnessIinitDrag : function (evt) {
      return {
        callback : this.brightnessScan,
        handle : this,
        offset : pergola.mousePoint(evt).y - this.needle.y,
        updateCoordinates : false
      };
    },

    brightnessScan : function (evt) {
      var handle = this.handle,
          needle = handle.needle;

      needle.y = pergola.mousePoint(evt).y - this.offset;
      if (needle.y < 0) needle.y = 0;
      else if (needle.y > handle.max) needle.y = handle.max;
      handle.setBrightness();
    },

    setBrightness : function () {
      this.opacity = (this.needle.y / this.max);
      this.brightness = this.max - this.needle.y;
      this.needle.container.setAttributeNS(null, "transform", "translate(0 " + this.needle.y + ")");
      this.spectrumMask.setAttributeNS(null, "opacity", this.opacity);
      this.RGB.r = this.valueFromBrightness(this.hue[0]);
      this.RGB.g = this.valueFromBrightness(this.hue[1]);
      this.RGB.b = this.valueFromBrightness(this.hue[2]);
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [this.RGB.r, this.RGB.g, this.RGB.b] + ")");
      this.valuesDisplay(this.RGB.r, this.RGB.g, this.RGB.b);
    },

    setByHex : function (v) {
    	var c = this.toRGB(v);
    	this.RGB.r = c[0];
      this.RGB.g = c[1];
      this.RGB.b = c[2];
      this.set();
    },

    set : function () {
      var r = Number(this.RGB.r),
      		g = Number(this.RGB.g),
      		b = Number(this.RGB.b),
					values = [r, g, b],
		      isNeutral = (r == g && g == b),
		      max = Math.max.apply(null, values),
		      min = Math.min.apply(null, values),
					ratio = (max - min) / this.max * 6,
					offset = 0,
					region,
					sign;

      this.drop.y = isNeutral ? this.max : Math.round(min * (this.max / max));

      if (max == r) {
        region = 0;
        sign = g > b;
      }
      else if (max == g) {
        region = 85;
        sign = r < b;
      }
      else {
        region = 170;
        sign = r > g;
      }

      for (var i in values) {
        if (values[i] == max) {
          values.splice(i, 1);
          break;
        }
      }

      if (values[1] != values[0]) {
        offset = Math.round(((Math.max.apply(null, values) - min) / ratio) * (sign || ~0X0));
      }

      this.drop.x = region + offset;
      if (this.drop.x < 0) this.drop.x += this.max;

      this.needle.y = (this.max - max);
      this.brightness = max;
      this.opacity = (this.needle.y / this.max);

      this.spectrumMask.setAttributeNS(null, "opacity", this.opacity);
      this.drop.container.setAttributeNS(null, "transform", "translate(" + this.drop.x + " " + this.drop.y + ")");
      this.needle.container.setAttributeNS(null, "transform", "translate(0 " + this.needle.y + ")");
      this.hue = this.hueFromValues([r, g, b]);
      this.brightnessMask.setAttributeNS(null, "stop-color", "rgb(" + this.hue + ")");
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [r, g, b] + ")");
      this.valuesDisplay(r, g, b);
      this.release();
    },

    valuesDisplay : function (r, g, b) {
    	var vBox = this.input,
					hex = this.toHex;

      vBox.hex.element.firstChild.data = (hex(r) + hex(g) + hex(b)).toUpperCase();
      vBox.r.element.firstChild.data = r;
      vBox.g.element.firstChild.data = g;
      vBox.b.element.firstChild.data = b;
    },

    valueFromBrightness : function (n) {
      return parseInt(n / this.max * this.brightness);
    },

    hueFromValues : function (rgb) {
    	if (!this.brightness) return [this.max, this.max, this.max];

      return [
				Math.ceil(rgb[0] / this.brightness * this.max),
				Math.ceil(rgb[1] / this.brightness * this.max),
				Math.ceil(rgb[2] / this.brightness * this.max)
			]												
    },

    toHex : function (n) {
      var c = Number(n).toString(16);

      if(c.length > 2) c = "FF";
      if(c.length == 1) c = "0" + c;
      return c;
    },

    toRGB : function (c) {
      var r = parseInt("0x" + c.substring(0, 2)),
          g = parseInt("0x" + c.substring(2, 4)),
          b = parseInt("0x" + c.substring(4, 6));

      return [r, g, b];
    },

    absoluteX : function (node) {
      return node.getCTM().e;
    },

    absoluteY : function (node) {
      return node.getCTM().f;
    },

    init : function () {
      if (pergola.Panel.checkPreemptive(this, true)) return;
      pergola.extend(this, arguments[0]);
      this.color = pergola.color.convert(this.color, "hex") || "#FFFFFF";
      this.sample.setAttributeNS(null, "fill", this.color);
      this.setByHex(this.color.substring(1));
      this.owner.container.setAttributeNS(null, "display", "block");
      this.owner.fn = this.user.fn;
    },

    handleEvent : function (evt) {
      var tgt = evt.target,
          point = pergola.mousePoint(evt);

      this.eventBehavior(evt);
      if (tgt == this.spectrumMask) {
        this.drop.x = Math.round(point.x - this.absoluteX(this.drop.container.parentNode));
        this.drop.y = Math.round(point.y - this.absoluteY(this.drop.container.parentNode));
        this.setDrop();
        pergola.dragarea.activate(this.spectrumIinitDrag(evt));
      }
      else if (tgt == this.brightnessBar) {
        this.needle.y = point.y - this.absoluteY(this.needle.container.parentNode);
        this.setBrightness();
        pergola.dragarea.activate(this.brightnessIinitDrag(evt));
      }
    },

    build : function () {
    	var g, clip, gradient;

      pergola.extend(this, arguments[0]);
      this.RGB = {};
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});

// spectrum box
      $C({element : "rect", x : -2, y : -2, width : this.width + 4, height : this.height + 4, fill : "url(#insetStroke)", appendTo : this.container});
      clip = $C({element : "clipPath", id : "paletteClip", appendTo : pergola.defs});
      $C({element : "rect", width : this.width, height : this.height, appendTo : clip});
      g = $C({element : "g", id : this.id + "_spectrumGroup", 'clip-path' : "url(#paletteClip)", 'cursor' : "crosshair", appendTo : this.container});
      $C({element : "image", width : this.width, height : this.height, 'xlink:href' : pergola.path + "lib/palettes/spectrum.png", 'pointer-events' : "none", appendTo : g});
      this.spectrumMask = $C({element : "rect", width : this.width, height : this.height, fill : "black", appendTo : g});
      this.registerEvents(this.spectrumMask, "mousedown");
      this.drop = {container : $C({element : "g", 'pointer-events' : "none", appendTo : g})};
      $C({element : "circle", cx : .5, cy : .5, r : 6, fill : "none", stroke : "black", appendTo : this.drop.container});
      $C({element : "circle", cx : .5, cy : .5, r : 7, fill : "none", stroke : "white", appendTo : this.drop.container});
  
// brightness bar
      g = $C({element : "g", transform : "translate(276 0)", appendTo : this.container});
      gradient = $C({element : "linearGradient", id : "brightnessbar", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : this.container});
      this.brightnessMask = $C({element : "stop", id : "brightnessMaskColor", offset : "0%", "stop-color" : "#ffffff", appendTo : gradient});
      $C({element : "stop", offset : "100%", "stop-color" : "#000000", appendTo : gradient});
  
      $C({element : "path", d : "M21-1v258h-22", fill : "none", stroke : "#F8F8F8", 'stroke-width' : 2, 'pointer-events' : "none", appendTo : g});
      $C({element : "path", d : "M-2,258v-260h24l-2 2h-20v256z", fill : "#C0C0C0", 'pointer-events' : "none", appendTo : g});
      $C({element : "rect", width : 20, height : 256, fill : "url(#brightnessbar)", 'pointer-events' : "none", appendTo : g});
      this.needle = {
        x : 0,
        y : 0,
        container : $C({element : "g", id : this.id + "_needleGroup", transform : "translate(" + this.x + " " + this.y + ")", appendTo : g})
      };
      $C({element : "path", d : "M-4,0.5l-4-5v10z", fill : "#303030", appendTo : this.needle.container});
      $C({element : "path", d : "M24,0.5l4-5v10z", fill : "#303030", appendTo : this.needle.container});
      this.brightnessBar = $C({element : "rect", id : this.id + "_brightnessBar", x : -10, width : 40, height : 256, fill : "white", opacity : 0, appendTo : g});
      this.registerEvents(this.brightnessBar, "mousedown");
  
// sample box
      g = $C({element : "g", transform : "translate(332 0)", appendTo : this.container});
      $C({element : "rect", x : -2, y : -2, width : 58, height : 58, fill : "url(#insetStroke)", appendTo : g});
      this.sample = $C({element : "rect", width : 54, height : 27, fill : "white", appendTo : g});
      this.newSample = $C({element : "rect", y : 27, width : 54, height : 27, fill : "white", appendTo : g});

// input boxes
      g = $C({element : "g", transform : "translate(336 110)", appendTo : this.container});

      this.input = {

        r : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        caption : {
	        	x : -10,
	          textNode : "R",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.r,
	        realTime : true,
	        target : this.RGB,
	        propName : "r",
	        fn : "set"
	      }),

        g : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        y : 26,
	        caption : {
	        	x : -10,
	          textNode : "G",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.g,
	        realTime : true,
	        target : this.RGB,
	        propName : "g",
	        fn : "set"
	      }),

        b : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        y : 52,
	        caption : {
	        	x : -10,
	          textNode : "B",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.b,
	        realTime : true,
	        target : this.RGB,
	        propName : "b",
	        fn : "set"
	      }),

        hex : new pergola.Input().build({
	        owner : this,   
	        parent : g,
	        type : pergola.keypressHex,
	        y : 78,
	        width : 56,
	        hasButtons : false,
	        caption : {
	        	x : -10,
	          textNode : "#",
	        },
	        max : 0xFFFFFF,
	        min : 0,
	        maxLength : 6,
	        value : "FFFFFF",
	        message : "This value must be between 0 and FFFFFF.",
	        fn : "setByHex"
	      })
      };

      return this;
    }
  });

  extend(pergola.ColorPickerExtended.prototype, {
    width : 1536,
    height : 256,
    max : 255,

    release : function () {
      var r = this.RGB.r,
					g = this.RGB.g,
					b = this.RGB.b,
					hexR = this.toHex(r),
					hexG = this.toHex(g),
					hexB = this.toHex(b);

    	this.input.hex.value = (hexR + hexG + hexB).toUpperCase();
      this.input.r.value = r;
      this.input.g.value = g;
      this.input.b.value = b;
			this.color = "#" + this.input.hex.value;
    },

    spectrumIinitDrag : function (evt) {
      var point = pergola.mousePoint(evt);
      return {
        callback : this.spectrumScan,
        handle : this,
        offset : {
					x : point.x - this.drop.x,
					y : point.y - this.drop.y
				},
        updateCoordinates : false,
        cursor : "crosshair"
      };
    },

    spectrumScan : function (evt) {
      var handle = this.handle,
          drop = handle.drop,
          point = pergola.mousePoint(evt);

      drop.x = Math.round(point.x - this.offset.x);
      drop.y = Math.round(point.y - this.offset.y);
      if (drop.x < 0) drop.x = 0;
      else if (drop.x > handle.width - 1) drop.x = handle.width - 1;
      if (drop.y < 0) drop.y = 0;
      else if (drop.y > handle.max) drop.y = handle.max;
      handle.setDrop();
    },

    setDrop : function () {
      var x = this.drop.x,
          y = this.drop.y,
					rgb;

      this.colorIndex = y * this.width + x;
      this.hue = pergola.color.rgbTable[this.colorIndex];
      this.RGB.r = this.valueFromBrightness(this.hue[0]);
      this.RGB.g = this.valueFromBrightness(this.hue[1]);
      this.RGB.b = this.valueFromBrightness(this.hue[2]);
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [this.RGB.r, this.RGB.g, this.RGB.b] + ")");
      this.brightnessMask.setAttributeNS(null, "stop-color", "rgb(" + this.hue + ")");
      this.drop.container.setAttributeNS(null,"transform", "translate(" + x + " " + y + ")");
      this.valuesDisplay(this.RGB.r, this.RGB.g, this.RGB.b);
    },

    brightnessIinitDrag : function (evt) {
      return {
        callback : this.brightnessScan,
        handle : this,
        offset : pergola.mousePoint(evt).y - this.needle.y,
        updateCoordinates : false
      };
    },

    brightnessScan : function (evt) {
      var handle = this.handle,
          needle = handle.needle;

      needle.y = pergola.mousePoint(evt).y - this.offset;
      if (needle.y < 0) needle.y = 0;
      else if (needle.y > handle.max) needle.y = handle.max;
      handle.setBrightness();
    },

    setBrightness : function () {
      this.opacity = (this.needle.y / this.max);
      this.brightness = this.max - this.needle.y;
      this.needle.container.setAttributeNS(null, "transform", "translate(0 " + this.needle.y + ")");
      this.spectrumMask.setAttributeNS(null, "opacity", this.opacity);
      this.RGB.r = this.valueFromBrightness(this.hue[0]);
      this.RGB.g = this.valueFromBrightness(this.hue[1]);
      this.RGB.b = this.valueFromBrightness(this.hue[2]);
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [this.RGB.r, this.RGB.g, this.RGB.b] + ")");
      this.valuesDisplay(this.RGB.r, this.RGB.g, this.RGB.b);
    },

    setByHex : function (v) {
    	var c = this.toRGB(v);
    	this.RGB.r = c[0];
      this.RGB.g = c[1];
      this.RGB.b = c[2];
      this.set();
    },

    set : function () {
      var r = Number(this.RGB.r),
      		g = Number(this.RGB.g),
      		b = Number(this.RGB.b),
					values = [r, g, b],
		      isNeutral = (r == g && g == b),
		      max = Math.max.apply(null, values),
		      min = Math.min.apply(null, values),
					ratio = (max - min) / this.max,
					offset = 0,
					region,
					sign;

      this.drop.y = isNeutral ? this.max : Math.round(min * (this.max / max));

      if (max == r) {
        region = 0;
        sign = g > b;
      }
      else if (max == g) {
        region = 512;
        sign = r < b;
      }
      else {
        region = 1024;
        sign = r > g;
      }

      for (var i in values) {
        if (values[i] == max) {
          values.splice(i, 1);
          break;
        }
      }

      if (values[1] != values[0]) {
        offset = Math.round(((Math.max.apply(null, values) - min) / ratio) * (sign || ~0X0));
      }

      this.drop.x = region + offset;
      if (this.drop.x < 0) this.drop.x += this.width - 1;

      this.needle.y = (this.max - max);
      this.brightness = max;
      this.opacity = (this.needle.y / this.max);

      this.spectrumMask.setAttributeNS(null, "opacity", this.opacity);
      this.drop.container.setAttributeNS(null, "transform", "translate(" + this.drop.x + " " + this.drop.y + ")");
      this.needle.container.setAttributeNS(null, "transform", "translate(0 " + this.needle.y + ")");
      this.hue = this.hueFromValues([r, g, b]);
      this.brightnessMask.setAttributeNS(null, "stop-color", "rgb(" + this.hue + ")");
      this.newSample.setAttributeNS(null, "fill", "rgb(" + [r, g, b] + ")");
      this.valuesDisplay(r, g, b);
      this.release();
    },

    valuesDisplay : function (r, g, b) {
    	var vBox = this.input,
					hex = this.toHex;

      vBox.hex.element.firstChild.data = (hex(r) + hex(g) + hex(b)).toUpperCase();
      vBox.r.element.firstChild.data = r;
      vBox.g.element.firstChild.data = g;
      vBox.b.element.firstChild.data = b;
    },

    valueFromBrightness : function (n) {
      return parseInt(n / this.max * this.brightness);
    },

    hueFromValues : function (rgb) {
    	if (!this.brightness) return [this.max, this.max, this.max];

      return [
				Math.ceil(rgb[0] / this.brightness * this.max),
				Math.ceil(rgb[1] / this.brightness * this.max),
				Math.ceil(rgb[2] / this.brightness * this.max)
			]												
    },

    toHex : function (n) {
      var c = Number(n).toString(16);

      if(c.length > 2) c = "FF";
      if(c.length == 1) c = "0" + c;
      return c;
    },

    toRGB : function (c) {
      var r = parseInt("0x" + c.substring(0, 2)),
          g = parseInt("0x" + c.substring(2, 4)),
          b = parseInt("0x" + c.substring(4, 6));

      return [r, g, b];
    },

    absoluteX : function (node) {
      return node.getCTM().e;
    },

    absoluteY : function (node) {
      return node.getCTM().f;
    },

    init : function () {
      if (pergola.Panel.checkPreemptive(this, true)) return;
      pergola.extend(this, arguments[0]);
      this.color = pergola.color.convert(this.color, "hex") || "#FFFFFF";
      this.sample.setAttributeNS(null, "fill", this.color);
      this.setByHex(this.color.substring(1));
      this.owner.container.setAttributeNS(null, "display", "block");
      this.owner.fn = this.user.fn;
    },

    handleEvent : function (evt) {
      var tgt = evt.target,
          point = pergola.mousePoint(evt);

      this.eventBehavior(evt);
      if (tgt == this.spectrumMask) {
        this.drop.x = Math.round(point.x - this.absoluteX(this.drop.container.parentNode));
        this.drop.y = Math.round(point.y - this.absoluteY(this.drop.container.parentNode));
        this.setDrop();
        pergola.dragarea.activate(this.spectrumIinitDrag(evt));
      }
      else if (tgt == this.brightnessBar) {
        this.needle.y = point.y - this.absoluteY(this.needle.container.parentNode);
        this.setBrightness();
        pergola.dragarea.activate(this.brightnessIinitDrag(evt));
      }
    },

    build : function () {
    	var g, clip, gradient;

      pergola.extend(this, arguments[0]);
      this.RGB = {};
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});

// spectrum box
      $C({element : "path", d : "M1537-1v258h-1538", fill : "none", stroke : "#F8F8F8", 'stroke-width' : 2, appendTo : this.container});
      $C({element : "path", d : "M-2,258v-260h1540l-2 2h-1536v256z", fill : "#C0C0C0", appendTo : this.container});
      clip = $C({element : "clipPath", id : "paletteClip", appendTo : pergola.defs});
      $C({element : "rect", width : this.width, height : this.height, appendTo : clip});
      g = $C({element : "g", id : this.id + "_spectrumGroup", 'clip-path' : "url(#paletteClip)", 'cursor' : "crosshair", appendTo : this.container});
      $C({element : "image", width : this.width, height : this.height, 'xlink:href' : pergola.path + "lib/palettes/spectrumExtended.png", 'pointer-events' : "none", appendTo : g});
      this.spectrumMask = $C({element : "rect", width : this.width, height : this.height, fill : "black", appendTo : g});
      this.registerEvents(this.spectrumMask, "mousedown");

// drop
      this.drop = {container : $C({element : "g", 'pointer-events' : "none", appendTo : g})};
      $C({element : "circle", cx : .5, cy : .5, r : 6, fill : "none", stroke : "black", appendTo : this.drop.container});
      $C({element : "circle", cx : .5, cy : .5, r : 7, fill : "none", stroke : "white", appendTo : this.drop.container});
  
// brightness bar
      g = $C({element : "g", transform : "translate(454 270)", appendTo : this.container});
      gradient = $C({element : "linearGradient", id : "brightnessbar", x1 : "0%", y1 : "0%", x2 : "0%", y2 : "100%", gradientUnits : "objectBoundingBox", appendTo : this.container});
      this.brightnessMask = $C({element : "stop", id : "brightnessMaskColor", offset : "0%", "stop-color" : "#ffffff", appendTo : gradient});
      $C({element : "stop", offset : "100%", "stop-color" : "#000000", appendTo : gradient});
  
      $C({element : "path", d : "M25-1v258h-26", fill : "none", stroke : "#F8F8F8", 'stroke-width' : 2, 'pointer-events' : "none", appendTo : g});
      $C({element : "path", d : "M-2,258v-260h28l-2 2h-24v256z", fill : "#C0C0C0", 'pointer-events' : "none", appendTo : g});
      $C({element : "rect", width : 24, height : 256, fill : "url(#brightnessbar)", 'pointer-events' : "none", appendTo : g});

// needle
      this.needle = {
        x : 0,
        y : 0,
        container : $C({element : "g", id : this.id + "_needleGroup", transform : "translate(" + this.x + " " + this.y + ")", appendTo : g})        
      };
      $C({element : "path", d : "M-4,0.5l-4-5v10z", fill : "#303030", appendTo : this.needle.container});
      $C({element : "path", d : "M28,0.5l4-5v10z", fill : "#303030", appendTo : this.needle.container});

      this.brightnessBar = $C({element : "rect", id : this.id + "_brightnessBar", x : -10, width : 44, height : 256, fill : "white", opacity : 0, appendTo : g});
      this.registerEvents(this.brightnessBar, "mousedown");
  
// sample box
      g = $C({element : "g", transform : "translate(142 270)", appendTo : this.container});
      $C({element : "path", d : "M257-1v258h-258", fill : "none", stroke : "#F8F8F8", 'stroke-width' : 2, appendTo : g});
      $C({element : "path", d : "M-2,258v-260h260l-2 2h-256v256z", fill : "#C0C0C0", appendTo : g});
      this.sample = $C({element : "rect", width : 256, height : 128, fill : "white", appendTo : g});
      this.newSample = $C({element : "rect", y : 128, width : 256, height : 128, fill : "white", appendTo : g});


// input boxes
      g = $C({element : "g", transform : "translate(546 380)", appendTo : this.container});
      this.input = {

        r : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        caption : {
	        	x : -10,
	          textNode : "R",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.r,
	        realTime : true,
	        target : this.RGB,
	        propName : "r",
	        fn : "set"
	      }),

        g : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        y : 26,
	        caption : {
	        	x : -10,
	          textNode : "G",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.g,
	        realTime : true,
	        target : this.RGB,
	        propName : "g",
	        fn : "set"
	      }),

        b : new pergola.Input().build({
	        owner : this,
	        parent : g,
	        type : pergola.keypressPosInt,
	        y : 52,
	        caption : {
	        	x : -10,
	          textNode : "B",
	        },
	        max : this.max,
	        min : 0,
	        maxLength : 3,
	        value : this.RGB.b,
	        realTime : true,
	        target : this.RGB,
	        propName : "b",
	        fn : "set"
	      }),

        hex : new pergola.Input().build({
	        owner : this,   
	        parent : g,
	        type : pergola.keypressHex,
	        y : 78,
	        width : 56,
	        hasButtons : false,
	        caption : {
	        	x : -10,
	          textNode : "#",
	        },
	        max : 0xFFFFFF,
	        min : 0,
	        maxLength : 6,
	        value : "FFFFFF",
	        message : "This value must be between 0 and FFFFFF.",
	        fn : "setByHex"
	      })

      };

      return this;
    }
  });


  extend(pergola.ComboBox.prototype, {
    width : pAttributes.comboBox.width,
    height : pAttributes.comboBox.height,
    listItemHeight : pAttributes.comboBox.listItemHeight,
    listMargin : pAttributes.comboBox.listMargin,
    scrollSize : pAttributes.scrollBar.scrollSize,
    margin : 0,
    paneY : 0,
    selectionHandle : null,
    selection : null,

    cancelSelection : function (evt, reset) {
      this.selection = "";
      if (this.selectionHandle) {
        var handle = this.selectionHandle;
        handle.effects(evt);
        this.registerEvents(handle.button, "mouseout", handle);
        if (handle.preview) handle.preview.setAttributeNS(null, "display", "none");
        this.selectionHandle = null;
        if (reset) {
          this.doc.ctm.e = this.doc.ctm.f = 0;
          this.doc.matrixTransformApply();
          this.doc.horScrollSliderSync();
          this.doc.vertScrollSliderSync();
        }
      }
      if (this.owner) this.owner.validated = false;
    },

    empty : function () {
      if (this.listGroup) pergola.removeChildren(this.listGroup);
      if (this.list) {
				for (var i in this.list) {
          delete this[this.list[i]];
        }
			}
    },

    getListWidth : function () {
    	var lineLength = [],
					width,
					subtract;

      for (var a in this.list) lineLength.push(this.list[a].width(this['font-size'], this['font-family']));
      width = Math.max.apply(null, lineLength) + this.listMargin * 2;
      subtract = 6 + this.vScrollBar.size * (this.list.length * this.listItemHeight > this.height);
      if (width < this.width - subtract) width = this.width - subtract;
      
      return width;
    },

    update : function (list) {
    	if (!list) {
        alert(pergola.messages.missingList + "combobox " + this.id);
        return;
      }

      var width;

      this.empty();
      if (!this.listGroup) this.listGroup = $C({element : "g", transform : "translate(2 2)", 'font-size' : this['font-size'], 'font-family' : this['font-family'], appendTo : this.doc.transformable});
      this.list = list;
      this.advanceY = 0;

      width = this.getListWidth();
      for (var a in this.list) {
        var prop = this.list[a];
        this[prop] = new pergola.ListItem(this.id + "_" + prop)
        .build({
          owner : this,
          parent : this.listGroup,
          width : width,
          string : prop
        });
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);

      var width;

      if (this.owner && !this.x) this.x = ((this.owner.width - this.width) / 2);
      this.height -= this.height % (this.listItemHeight + 2) - 4;

      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      $C({element : "rect", id : this.id + "_background", x : .5, y : .5, width : this.width - 1, height : this.height - 1, fill : "white", stroke : this.stroke, appendTo : this.container});
      this.doc = new pergola.Doc(this.id + "_doc")
			.build({
        owner : this,
        parent : this.container,
        x : 1,
        y : 1
      });
      this.hScrollBar = new pergola.Scrollbar(this.id + "_hScrollBar")
      .buildH({
        owner : this,
        parent : this.container,
        scrollSize : this.scrollSize,
        scrollTarget : this.doc,
        symbol : pergola.symbols.arrow.small,
        step : 8
      });
      this.vScrollBar = new pergola.Scrollbar(this.id + "_vScrollBar")
      .buildV({
        owner : this,
        parent : this.container,
        scrollSize : this.scrollSize,
        scrollTarget : this.doc,
        symbol : pergola.symbols.arrow.small,
        step : this.listItemHeight + 2
      });
      var scrlsize = this.hScrollBar.size;
      this.resizeDummy = $C({element : "rect", x : this.width - scrlsize - 1, y : this.height - scrlsize - 1, width : scrlsize, height : scrlsize, fill : pAttributes.scrollBar.fill});
      this.container.insertBefore(this.resizeDummy, this.hScrollBar.container);

      if (this.list) {
        this.listGroup = $C({element : "g", transform : "translate(2 2)", 'font-size' : this['font-size'], 'font-family' : this['font-family']});
        this.advanceY = 0;

        width = this.getListWidth();
        for (var a in this.list) {
          var prop = this.list[a];
          this[prop] = new pergola.ListItem(this.id + "_" + prop)
          .build({
            owner : this,
            parent : this.listGroup,
            width : width,
            string : prop
          });
        }
        this.doc.transformable.appendChild(this.listGroup);
      }
      else if (this.layout) {
        var layout = this.layout;
        this.layout = new pergola.Layout();
        for (var p in layout) this.layout[p] = layout[p];
        this.layout.build({
          owner : this,
          parent : this.doc.transformable,
          x : layout.x,
          y : layout.y,
          width : layout.width || this.doc.width(),
          height : layout.height || this.doc.height()
        });
      }
      if (!pergola.mutationEvnt) this.doc.updateOnMutationEvent();

      return this;
    }
  });


  extend(pergola.CommandButton.prototype, {
    width : pAttributes.commandButton.width,
    height : pAttributes.commandButton.height,
    extra : {
      rx : pAttributes.commandButton.rx
    }
  });


  extend(pergola.Coordinates.prototype, {
    "font-size" : pAttributes.coordinates["font-size"],

    enable : function () {
      this.text.setAttributeNS(null, "display", "block");
    },

    disable : function () {
      this.text.setAttributeNS(null, "display", "none");
      this.text.firstChild.data = " ";
    },

    update : function (evt, m) {
      this.text.setAttributeNS(null, "transform", "translate(" + (evt.clientX + 6) + " " + (evt.clientY - 6) + ")");
      this.text.firstChild.data = m;
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.text = $C({element : "text", id : "pergola_coordinates", "font-size" : this["font-size"], "pointer-events" : "none", textNode : " ", appendTo : this.parent});

      return this;
    }
  });


  extend(pergola.DialogButton.prototype, {
    rx : pAttributes.dialogButton.rx,
    width : pAttributes.dialogButton.width,
    height : pAttributes.dialogButton.height,

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner && !this.fn) {
        alert (pergola.messages.missingHandler + "DialogButton");
        return;
      }
      var attributes = {
        width : this.width,
        height : this.height,
        rx : this.rx,
        fill : this.fill,
        'fill-opacity' : 0,
        stroke : this.stroke
      };
      var maskAttributes = {
        width : this.width,
        height : this.height,
        rx : this.rx,
        fill : this.maskFill
      };
      if (this.extra) pergola.extend(attributes, this.extra);
      var x = this.setPositionToStrokeWidth(attributes['stroke-width'] || 1);
      if (x) maskAttributes.y = maskAttributes.x = attributes.y = attributes.x = x;
      
      this.container = $C({element : "g", id :  this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.mask = $C(pergola.extend({element : 'rect', appendTo : this.container}, maskAttributes));
      this.button = $C(pergola.extend({element : 'rect', appendTo : this.container}, attributes));
      if (this.hasVisualEffects) this.registerEvents(this.button, ["mouseover", "mouseout"]);
      if (this.owner) this.registerEvents(this.button, "mouseup", this.owner);
      if (this.ev) {
        if (typeof this.ev == "string") this.ev = [this.ev];
        this.registerEvents(this.button, this.ev);
      }
      this.text = $C({element : "text", x : this.width / 2, y : this.height / 2 + 4, 'font-family' : this['font-family'], 'font-size' : this['font-size'], fill : this.textFill, 'text-anchor' : "middle", 'pointer-events' : "none", textNode : this.text, appendTo : this.container});
      if (this.extra) delete this.extra;

      return this;
    }
  });


  extend(pergola.Doc.prototype, {
    parent : null,
    scaleFactor : 1,
    maxScale : 64,
    minScale : .0313,
    autoscrollValue : 6,

    width : function () {
			return this.owner.width - this.owner.margin * 2 - 2
		},

    height : function () {
      var owner = this.owner;
      return owner.height - this.owner.margin - 2 - owner.paneY - (owner.hasStatus ? owner.status.height : 0);
    },

    wRatio : function () {return this.scaledW / this.owner.hScrollBar.range},

    hRatio : function () {return this.scaledH / this.owner.vScrollBar.range},

    getVisibleCenter : function () {
      return {
        x : (this.owner.hScrollBar.slider.pos * this.wRatio() + this.visibleW / 2) / this.scaleFactor,
        y : (this.owner.vScrollBar.slider.pos * this.hRatio() + this.visibleH / 2) / this.scaleFactor
      }
    },

	  zoomPointIsCenter : function () {
	    return {
	      x : (this.scaledW > this.visibleW) ? this.visibleW / 2 : this.scaledW / 2,
	    	y : (this.scaledH > this.visibleH) ? this.visibleH / 2 : this.scaledH / 2
			};
	  },
	
	  zoomPointIsMouse : function (point) {
	    return {
//	      x : point.x - this.absoluteX(this.container),
//	    	y : point.y - this.absoluteY(this.container)
/*
 * workaround for Firefox getCTM() bug (https://bugzilla.mozilla.org/show_bug.cgi?id=873106)
*/
        x : point.x - (this.owner.x + this.x),
        y : point.y - (this.owner.y + this.y)
			};
	  },

    centerCanvas : function () {
      this.ctm.e = -parseInt(((this.canvas.width - this.width()) / 2));
      this.ctm.f = -parseInt(((this.canvas.height - this.height()) / 2));
			this.matrixTransformApply();
    },

	  matrixTransformApply : function () {
	    var m = this.ctm,
	        matrix = "matrix(" + m.a + " " + m.b + " " + m.c + " " + m.d + " " + m.e + " " + m.f + ")";

	    this.transformable.setAttributeNS(null, "transform", matrix);
	  },

	  transformUpdate : function () {
	    var owner = this.owner,
	        hBar = owner.hScrollBar,
	        vBar = owner.vScrollBar,
	        ownerW = owner.width,
	        ownerH = owner.height,
	        s = owner.hScrollBar.size - 1,
	        w = this.width(),
	        h = this.height();

	    if (owner.map) {
	      var map = owner.map,
	          mapSize = map.size();
	      this.ctm.e = -parseInt(((mapSize.x - this.width()) / 2));
	      this.ctm.f = -parseInt(((mapSize.y - this.height()) / 2));
	      if (map.grid) owner.mapGutterPositionOnScroll();
	    }

	    this.setBackground();
			this.scrollBarsDisplay();
	    hBar.extent = w - s * (owner.resizable | vBar.visible) + (!owner.resizable * !vBar.visible) - hBar.margins;
	    vBar.extent = h - s * (owner.resizable | hBar.visible) + (!owner.resizable * !hBar.visible) - vBar.margins;
	    this.horScrollSliderSync();
	    this.vertScrollSliderSync();
	    hBar.track.setAttributeNS(null, "width", hBar.extent);
	    vBar.track.setAttributeNS(null, "height", vBar.extent);
	    hBar.container.setAttributeNS(null, "transform", "translate(" + this.x + " " + (this.y + h - s - 1) + ")");
	    vBar.container.setAttributeNS(null, "transform", "translate(" + (this.x + w - s - 1) + " " + this.y + ")");
	    hBar.fwdScroll.container.setAttributeNS(null, "transform", "translate(" + (hBar.scrollButtonSize + hBar.extent - 1) + " 0)");
	    vBar.fwdScroll.container.setAttributeNS(null, "transform", "translate(0 " + (vBar.scrollButtonSize + vBar.extent - 1) + ")");

	    if (owner.resizable) {
	      var frame = owner.frame.rect,
	          bg = owner.background,
	          topBar = owner.topBar,
	          statusBar = owner.status,
	          hDiff = this.scaledW + this.ctm.e,
	          vDiff = this.scaledH + this.ctm.f;
	      if (this.visibleW >= hDiff && this.ctm.e < 0) this.ctm.e += this.visibleW - hDiff;
	      if (this.visibleH >= vDiff && this.ctm.f < 0) this.ctm.f += this.visibleH - vDiff;
	      this.container.setAttributeNS(null, "width", w);
	      this.container.setAttributeNS(null, "height", h);
	      bg.rect.setAttributeNS(null, "width", w + 1);
	      bg.rect.setAttributeNS(null, "height", h + 1);
	      if (bg['xlink:href']) {
	        bg.image.setAttributeNS(null, "width", w);
	        bg.image.setAttributeNS(null, "height", h);
	      }
	      frame.setAttributeNS(null, "width", ownerW - 1);
	      frame.setAttributeNS(null, "height", ownerH - 1);
	      topBar.title.setAttributeNS(null, "x", ownerW / 2);
	      topBar.bar.setAttributeNS(null, "width", w + 2);
	      topBar.mask.setAttributeNS(null, "width", w + 2);
	      if (owner.toolBar) {
	        owner.toolBar.bar.setAttributeNS(null, "width", w + 2);
	        if (owner.toolBar.decoration) {
	          owner.toolBar.decoration.setAttributeNS(null, "width", (w + 2) - 8);
	        }
	      }
	      if (statusBar) {
	        statusBar.container.setAttributeNS(null, "transform", "translate(" + owner.margin + " " + (ownerH - statusBar.height - owner.margin) + ")");
	        statusBar.bar.setAttributeNS(null, "width", ownerW - owner.margin * 2);
	      }
	      if (this.copyright) this.copyright.setAttributeNS(null, "transform", "translate(13 " + (this.visibleH - 54) + ")");
	      if (owner.closeGroup) owner.closeGroup.setAttributeNS(null, "transform", "translate(" + owner.closeGroupPos() + ")");
	    }

	    this.resizeButtonDisplay();
	    this.matrixTransformApply();
	  },

		append : function () {
			var args = arguments[0],
					container = args.container || this.transformable;

			if (args.resetScale) {
				this.scaleFactor = 1;
		  	this.scale();
			}
			if (!args.merge) this.flush(container);
			container.appendChild(args.newdoc);
		},

	  flush : function (layer) {
      pergola.removeChildren(layer);
			if (layer == this.transformable) this.background = $C({element : "rect", id : this.id + "_background", width : this.width(), height : this.height(), fill : this.fill, appendTo : layer});
		},

	  setBackground : function () {
	  	if (! this.docBgResizable) return;
			var bbox = this.transformable.getBBox(),
					w = (this.scaledW < this.visibleW) ? this.scaledW + this.visibleW / this.scaleFactor : bbox.width,
					h = (this.scaledH < this.visibleH) ? this.scaledH + this.visibleH / this.scaleFactor : bbox.height;

			this.background.setAttributeNS(null, "width", w);
      this.background.setAttributeNS(null, "height", h);
		},

	  scale : function (point) {
	    point = point ? this.zoomPointIsMouse(point) : this.zoomPointIsCenter();

			var offset = {x : point.x - this.ctm.e, y : point.y - this.ctm.f},
					xRatio = this.scaledW / offset.x,
					yRatio = this.scaledH / offset.y,
					posX,
					posY,
					minX,
					minY;

	    this.scaledW = this.canvas.width * this.scaleFactor;
	    this.scaledH = this.canvas.height * this.scaleFactor;
	    posX = this.scaledW / xRatio * -1 + point.x * (pergola.rubberBox.width < 2);
	    posY = this.scaledH / yRatio * -1 + point.y * (pergola.rubberBox.width < 2);
	    minX =- (this.scaledW - this.visibleW);
	    minY =- (this.scaledH - this.visibleH);
	    this.ctm.e = parseInt((posX > minX)? posX : minX * (minX < 0 | 0));
	    this.ctm.f = parseInt((posY > minY)? posY : minY * (minY < 0 | 0));
	    if (this.ctm.e > 0) this.ctm.e = 0;
	    if (this.ctm.f > 0) this.ctm.f = 0;
	    this.ctm.a = this.ctm.d = this.scaleFactor;
	    this.scrollBarsDisplay();
	    this.horScrollSliderSync();
	    this.vertScrollSliderSync();
	    this.resizeButtonDisplay();
	    this.setBackground();
	    this.matrixTransformApply();
	    if (this.owner.status) this.owner.status.zoom(this.scaleFactor);
	  },

    updateOnMutationEvent : function () {
      if (this.owner.type != "map") {
      	this.setBackground();
        this.canvas.width = this.transformable.getBBox().width;
        this.canvas.height = this.transformable.getBBox().height;
      }

      this.scaledW = this.canvas.width * this.scaleFactor;
      this.scaledH = this.canvas.height * this.scaleFactor;
      this.visibleW = this.width();
      this.visibleH = this.height();
      this.transformUpdate();
      if ("cloneGroup" in this) this.updatePreview();
    },

  horScrollSliderSync : function () {
    var hBar = this.owner.hScrollBar,
        sldr = hBar.slider,
				ratio;

    hBar.page = this.visibleW - hBar.step;
		hBar.range = hBar.extent - hBar.sliderClearance;
		ratio = this.wRatio();
    if (hBar.scrollType == "navigation") {
      sldr.size = sldr.width;
      sldr.pos = this.canvas.width / ratio / 2;
    }
    else {
      sldr.size = this.visibleW / ratio;
      if (sldr.size < sldr.minSize) sldr.size = sldr.minSize;
      sldr.pos = this.ctm.e / ratio * ~0X0;
    }
    sldr.max = hBar.range - sldr.size;
    if (sldr.pos < 0) sldr.pos = 0;
    if (sldr.pos > sldr.max) sldr.pos = sldr.max;
    sldr.container.setAttributeNS(null, "transform", "translate(" + Math.round(sldr.pos) + " 0)");
    sldr.bar.setAttributeNS(null, "width", Math.round(sldr.size));
  },

  vertScrollSliderSync : function () {
    var vBar = this.owner.vScrollBar,
        sldr = vBar.slider,
				ratio;

    vBar.page = this.visibleH - vBar.step;
		vBar.range = vBar.extent - vBar.sliderClearance;
		ratio = this.hRatio()
    if (vBar.scrollType == "navigation") {
      sldr.size = sldr.height;
      sldr.pos = this.canvas.height / ratio / 2;
    }
    else {
      sldr.size = this.visibleH / ratio;
      if (sldr.size < sldr.minSize) sldr.size = sldr.minSize;
      sldr.pos = this.ctm.f / ratio * ~0X0;
    }
    sldr.max = vBar.range - sldr.size;
    if (sldr.pos < 0) sldr.pos = 0;
    if (sldr.pos > sldr.max) sldr.pos = sldr.max;
    sldr.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(sldr.pos) + ")");
    sldr.bar.setAttributeNS(null, "height", Math.round(sldr.size));
  },

	  resizeButtonDisplay : function () {
	    var owner = this.owner,
	        rsz = owner.resizeBtn;

	    if (owner.resizeDummy) {
	      owner.resizeDummy.setAttributeNS(null, "opacity", (owner.hScrollBar.visible | owner.vScrollBar.visible));
	      return;
	    }
	    rsz.container.setAttributeNS(null, "transform", "translate(" + (owner.width - rsz.x) + " " + (owner.height - rsz.y - (owner.status ? owner.status.height : 0)) + ")");
	    rsz.button.setAttributeNS(null, "opacity", (owner.hScrollBar.visible | owner.vScrollBar.visible));
	  },

	  scrollBarsDisplay : function () {
	    var owner = this.owner,
	        hBar = owner.hScrollBar,
	        vBar = owner.vScrollBar;

	    hBar.visible = (this.visibleW < this.scaledW) ? true : false;
	    vBar.visible = (this.visibleH < this.scaledH) ? true : false;
	    this.visibleW = this.width() - owner.vScrollBar.size * vBar.visible;
	    this.visibleH = this.height() - owner.hScrollBar.size * hBar.visible;
	    hBar.container.setAttributeNS(null, "display", hBar.visible ? "block" : "none");
	    vBar.container.setAttributeNS(null, "display", vBar.visible ? "block" : "none");
	  },

    updatePreview : function () {
      var clone = this.transformable.cloneNode(true);
      clone.id = this.id + "_preview";
      var nodes = clone.getElementsByTagNameNS(pergola.ns.svg, "*");
      for (var i = 0; i < nodes.length; i ++) nodes.item(i).id = "";
      var oldClone = this.cloneGroup.firstChild;
      this.cloneGroup.replaceChild(clone, oldClone);
    },

    dblclick : function (evt) {
      evt.preventDefault();
      if (evt.detail == 2) {
        var owner = this.owner,
            map = owner.map,
            ctm = this.ctm,
            porg = polymaps.origin,
            z = map.zoom(),
            point = pergola.mousePoint(evt);

        if (evt.shiftKey) z = Math.ceil(z) - z - 1;
        else z = 1 - z + Math.floor(z);
        var m = {x : point.x - (ctm.e + porg.x), y : point.y - (ctm.f + porg.y)};
        owner.centerMap(map.pointLocation(m));
        map.zoomBy(z);
        z = map.zoom();
        owner.mapZoomLevelMark(z);
        if (owner.status) owner.status.zoom(z);
      }
    },

    mousewheel : function (evt) {
      if (!this.owner.mousewheel) return;  

      var editor = (this.owner.type == "editor"),
					delta = 1 + (evt.wheelDelta / 60 || -evt.detail) * .05;

      if (editor) this.hideSelectionOnScale();

      this.scaleFactor = (this.scaleFactor * delta).trim(4);
      if (this.scaleFactor > this.maxScale) this.scaleFactor = this.maxScale;
      if (this.scaleFactor < this.minScale) this.scaleFactor = this.minScale;

      this.scale(pergola.mousePoint(evt));

      if (editor) this.redrawOnScale();
    },

    zoomFromValue : function (v) {
      this.scaleFactor = parseFloat(v / 100);
      this.scale();
      if (this.owner.type == "editor") this.redrawOnScale();
    },

    overflow : function (v) {
      v = parseFloat(v);
      return {
        isOut : v / 100 > this.maxScale || v / 100 < this.minScale,
        message : "This value must be between " + (this.minScale * 100).toFixed(2) + "% and " + (this.maxScale * 100) + "%"
      };
    },

    scrollRange : function () {
      return {
        minX : -(this.scaledW - this.visibleW) * (this.scaledW > this.visibleW),
        minY :-(this.scaledH - this.visibleH) * (this.scaledH > this.visibleH)
      }
    },

    getOffset : function () {
    	if (pergola.browser.firefox) {
				return {
					x : this.ff_absoluteX() + this.x + this.ctm.e,
	        y : this.ff_absoluteY() + this.y + this.ctm.f
				};
			}
      return {
        x : this.absoluteX(this.container) + this.ctm.e,
        y : this.absoluteY(this.container) + this.ctm.f
      }
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var tgt = evt.target,
      		eType = evt.type,
					tool = this.currentTool;

      if (evt.type == "mousedown") {
      	if (pergola.Menu.active) pergola.Menu.handle.close(evt);
        if (tool) {
        	if (tool.name == "typeTool") this.registerEvents(this.transformable, ["mouseup"]);
					else this[tool.fn](evt);
				}
        if (this.owner.map && evt.detail == 2) this.dblclick(evt);
      }
      else if (evt.type == "mouseup") {
      	this.unregisterEvents(this.transformable, ["mouseup"]);
				if (tool && tool.name == "typeTool") this[tool.fn](evt);
			}
      else if (eType == "mousewheel" || eType == "DOMMouseScroll") this.mousewheel(evt);
      else if (eType == "DOMNodeInserted" || eType == "DOMNodeRemoved") this.updateOnMutationEvent();
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert(pergola.messages.missingOwner + "Doc");
        return;
      };
      if (!this.parent) {
        alert (pergola.messages.missingParent[0] + this.name);
        return;
      };
      this.ctm = {a : 1, b : 0, c : 0, d : 1, e : 0, f : 0};
      var w = this.width(),
          h = this.height();

      this.fill = (this.owner.type == "editor") ? "white" : "none";
			this.canvas = this.owner.canvas || {width : 0, height : 0};
      this.scaledW = w;
      this.scaledH = h;
      this.selection = [];
      this.container = $C({element : "svg", id : this.id, x : this.x, y : this.y, width : w, height : h, appendTo : this.parent});
      this.transformable = $C({element : "g", id : this.id + "_transformable", appendTo : this.container});
      this.background = $C({element : "rect", id : this.id + "_background", width : w, height : h, fill : this.fill, appendTo : this.transformable});
      if (this.owner.type == "editor") {
        this.ctm.e = -parseInt(((w - this.width()) / 2));
        this.ctm.f = -parseInt(((h - this.height()) / 2));
      }
      if (this.owner.type != "map") {
			  this.registerEvents(this.container, ["mousewheel", "DOMMouseScroll"]);
      }

      this.registerEvents(this.transformable, ["mousedown"]);
      if (this.owner.type != "editor") this.registerEvents(this.transformable, ["DOMNodeInserted", "DOMNodeRemoved"]);

      return this;
    }
  });


  extend(pergola.Dragarea.prototype, {
    width : "100%",
    height : "100%",
    fill : "none",
    updateCoordinates : true,
    cursor : "default",

    activate : function () {
      if (!arguments[0]) return;
      pergola.extend(this, arguments[0]);
      this.setPointerEvents([this.rect]);
      this.rect.setAttributeNS(null, "cursor", this.cursor);
      this.registerEvents(this.rect, ["mouseup", "mouseout", "mousemove"]);

      return this;
    },

    release : function (evt) {
      this.unregisterEvents(this.rect, ["mouseup", "mouseout", "mousemove"]);
      this.removePointerEvents([this.rect]);
      if (!this.handle) return;
      var handle = this.handle;
      if (this.updateCoordinates) {
        handle.x = evt.clientX - this.offset.x;
        handle.y = evt.clientY - this.offset.y;
      }
      if (handle.release) handle.release(evt);
      this.reset();
      this.cursor = "default";
      this.rect.setAttributeNS(null, "cursor", this.cursor);
      this.updateCoordinates = true;
      this.handle = this.target = this.fn = this.offset.x = this.offset.y = null;
    },

    resize : function (x, y, w, h) {
      this.x = x;
      this.y = y;
      this.width = w;
      this.height = h;
      this.setXYWH(this.rect);

      return this;
    },

    reset : function () {
      this.x = 0;
      this.y = 0;
      this.width = "100%";
      this.height = "100%";
      this.setXYWH(this.rect)
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var eType = evt.type;
      if (eType == "mousemove") this.callback(evt);
      else if (eType == "mouseup" || eType == "mouseout") this.release(evt);
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.parent = pergola.systemComponents;
      this.rect = $C({element : "rect", id : this.id, width : this.width, height : this.height, fill : this.fill, 'pointer-events' : "none", appendTo : this.parent});

      return this;
    }
  });


  extend(pergola.Frame.prototype, {
    width : pAttributes.frame.width,
    height : pAttributes.frame.height,
    rx : pAttributes.frame.rx,
    fill : pAttributes.frame.fill,
    'fill-opacity' : pAttributes.frame['fill-opacity'](),
    stroke : pAttributes.frame.stroke,
    'stroke-opacity' : pAttributes.frame['stroke-opacity'](),
    filter : "none",

    release : function (evt) {
      this.registerEvents(this.rect, ["mouseover", "mouseout", "mousemove"]);
    },

	  initDrag : function () {
	  	var owner = this.owner;
	  	if (owner.isFull) return;

	  	if (pergola.Key.active) pergola.Key.active.stop = false;
	
	    if (owner.type == "editor") {
	      owner.yardstick.container.setAttributeNS(null, "display", "none");
	      owner.compass.container.setAttributeNS(null, "display", "none");
	    }
	
	    return {
	      callback : this.callback,
	      handle : this,
	      target : owner,
	      offset : this.offset,
	      updateCoordinates : false,
	      cursor : this.cursor
	    }
	  },

    handleEvent : function (evt) {
    	this.eventBehavior(evt);
			var tgt = evt.target,
					owner = this.owner,
					point = pergola.mousePoint(evt),
					x = point.x,
					y = point.y;

			if (evt.type == "mouseover" || evt.type == "mousemove") {
	      this.north = this.east = this.south = this.west = false;
	      this.offset = {x : 0, y : 0};

	      if (x <= owner.x + 8) {
	        this.offset.x = x - owner.x;
	        this.west = true;
	      }
	      else if (x >= owner.x + owner.width - 8) {
	        this.offset.x = owner.x + owner.width - x;
	        this.east = true;
	      }
	      if (y <= owner.y + 8) {
	        this.offset.y = y - owner.y;
	        this.north = true;
	      }
	      else if (y >= owner.y + owner.height - 8) {
	        this.offset.y = owner.y + owner.height - y;
	        this.south = true;
	      }

	      if (this.north && this.west) {
	        this.cursor = "nw-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeNW;
	      }
	      else if (this.north && this.east) {
	        this.cursor = "ne-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeNE;
	      }
	      else if (this.south && this.west) {
	        this.cursor = "sw-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeSW;
	      }
	      else if (this.south && this.east) {
	        this.cursor = "se-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeSE;
	      }
	      else if (this.north) {
	        this.cursor = "n-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeN;
	      }
	      else if (this.east) {
	        this.cursor = "e-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeE;
	      }
	      else if (this.south) {
	        this.cursor = "s-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeS;
	      }
	      else if (this.west) {
	        this.cursor = "w-resize";
	        tgt.setAttributeNS(null, "cursor", this.cursor);
	        this.callback = this.resizeW;
	      }
	    }
	    else if (evt.type == "mouseout") {
	      tgt.setAttributeNS(null, "cursor", "default");
	      this.north = this.east = this.south = this.west = false;
	      this.offset.x = this.offset.y = 0;
	    }
	    else {
	    	this.unregisterEvents(this.rect, ["mouseover", "mouseout", "mousemove"]);
	    	if (pergola.Panel.checkPreemptive(null, true)) return;
				pergola.dragarea.activate(this.initDrag());
				this.owner.bringToFront();
			}
		},

    build : function () {
      pergola.extend(this, arguments[0]);

      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Frame");
        return;
      };

      var x = this.setPositionToStrokeWidth(this['stroke-width'] || 1);
      this.rect = $C({element : "rect", id : this.id, x : x, rx : this.rx, width : this.width, height : this.height, fill : this.fill, 'fill-opacity' : this['fill-opacity'], stroke : this.stroke, 'stroke-opacity' : this['stroke-opacity'], filter : this.filter, appendTo : this.parent});
      if (this.owner && this.owner.resizable) this.registerEvents(this.rect, ["mousedown", "mouseover", "mouseout", "mousemove"]);

      return this;
    }
  });


  extend(pergola.Input.prototype, {
    width : pAttributes.input.width,
    height : pAttributes.input.height,
    fill : pAttributes.input.fill,
    stroke : pAttributes.input.stroke,
    "font-family" : 0,
    "font-size" : 0,
    hasButtons : pAttributes.input.hasButtons,
    realTime : false,
    type : pergola.keypress,
    tX : 5,
		tY : 0,
    cursor : true,

    timerInit : function (evt, button, callback) {
      this.callback = callback;
      if (evt.type == "mousedown") {
        this.registerEvents(evt.target, ["mouseout", "mouseup"]);
        this.callback(evt);
        if (button.timer) button.timer.initialize();
        else {
          button.timer = pergola.Timer()
          .initialize({
            handle : this,
            callback : callback,
            delay : 400,
            frequence : 50
          });
        }
      }
      else if (evt.type == "mouseup" || evt.type == "mouseout") {
        button.timer.clear();
        this.unregisterEvents(evt.target, ["mouseout", "mouseup"]);
        if (this.fn) this.owner[this.fn]();
      }
    },

    setValue : function () {
      if (this.value.length > this.maxLength) {
        this.value = this.value.substring(0, this.value.length - 1);
        return;
      }

      this.element.firstChild.data = this.value;
      if (this.cursor) this.inputCursor.setPosition();

      if (this.realTime) {
				if (this.target) this.target[this.propName] = this.value;
		    this.baseValue = this.value;
	      if (this.fn) this.owner[this.fn](this.value);
			}
    },

	  increment : function () {
	    if (this.value >= this.max) return;
	    this.value++;
	    if (this.target) this.target[this.propName] = this.value;
	    this.element.firstChild.data = this.value;
	    if (this.realTime) this.owner[this.fn]();
	  },

	  decrement : function () {
	    if (this.value <= this.min) return;
	    this.value--;
	    if (this.target) this.target[this.propName] = this.value;
	    this.element.firstChild.data = this.value;
	    if (this.realTime) this.owner[this.fn]();
	  },

    reset : function () {
      this.value = this.element.firstChild.nodeValue = this.baseValue;
    },

    overflow : function () {
    	if (this.owner.overflow) return this.owner.overflow(this.value);

      var v = (this.type == pergola.keypressHex) ? pergola.hexToDecimal(this.value) : parseFloat(this.value),
					msg = pergola.messages.outOfRange;

      return {
        isOut : v > this.max || v < this.min,
        message : this.message || msg[0] + this.min + msg[1] + this.max
      };
    },

    endInput : function () {
      var win = pergola.Window.active(),
					outOfRange = (
						this.type == pergola.keypressNum ||
						this.type == pergola.keypressInt ||
						this.type == pergola.keypressPos ||
						this.type == pergola.keypressPosInt ||
						this.type == pergola.keypressHex
					) ? this.overflow() : false,
					panel = (this.owner.constructor == pergola.Panel),
					cp = (this.owner == pergola.colorpicker);

	    if (win && win.undoManager) win.undoManager.register();

	    if (!this.value) this.key.warn = false;

			if (!outOfRange && !this.value) {
				this.reset();				
			} 
			else if (outOfRange.isOut || !this.value) {
				if (this.realTime) {
					if (this.value > this.max) this.baseValue = this.max;
					else if (this.value < this.min) this.baseValue = this.min;
					if (this.value) outOfRange.message += "\nClosest value inserted.";
					this.reset();
					if (this.target) this.target[this.propName] = this.value;
				}			
				else {
					this.reset();					
				}
        if (this.key.warn) alert(outOfRange.message);
				this.key.warn = true;
				if (!arguments[0]) {
					if (cp) this.owner.owner.validated = true;
					else if (panel) this.owner.validated = false;
				}
      }
      else if (this.target) this.target[this.propName] = this.value;

      if (this.fn) this.owner[this.fn](this.value);
      if (this.owner.map) this.owner.map.add(this.owner.interact);
    },

    handleEvent : function (evt) {
      evt.stopPropagation();
      var tgt = evt.target,
          eType = evt.type,
					win = pergola.Window.active(),
					key = pergola.Key.active;

      if (tgt == this.rect || tgt == this.element) {
        if (key) {
					if (key == this.key) return;
					key.release();
				}

				if (win && win.undoManager) win.undoManager.disable();
        this.baseValue = this.value;
        this.value = this.element.firstChild.nodeValue = "";
        if (this.owner.map) this.owner.map.remove(this.owner.interact);

        this.key = pergola.Key()
        .register({
          handle : this,
          callback: {
            keypress : this.type,
          },
          warn : true,
          stop : true,
          destroy : "key"          
        });
      }
      else if (this.hasButtons) {
        if (tgt == this.inc.button) this.timerInit(evt, this.inc.button, this.increment);
        else if (tgt == this.dec.button) this.timerInit(evt, this.dec.button, this.decrement);
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.owner = this.owner || this;

      var fS = parseInt(this['font-size'] || this.constructor.superClass["font-size"]),
					h = (this.height == "auto") ? Math.ceil(fS * 1.5).oddToEven() : this.height,
					o;

			this.tY = fS + (h - fS) / 2 - 2;
			if (! ("baseValue" in this)) this.baseValue = this.value;

      this.container = $C({element : "g", transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      
      o = {element : "rect", x : .5, y : .5, width : this.width, height : h, fill : this.fill, appendTo : this.container};
      if (this.stroke != "none") o.stroke = this.stroke;
      this.rect = $C(o);

      o = {element : "text", x : this.tX, y : this.tY, textNode : this.value, appendTo : this.container};
      if (this["font-family"]) o["font-family"] = this["font-family"];
      if (this["font-size"]) o["font-size"] = this["font-size"];
      this.element = $C(o);

      this.registerEvents(this.container, "mouseup");

      if (this.hasButtons) {
        h /= 2,

        this.inc = new pergola.Button()
        .build({
          owner : this,
          parent : this.container,
          x : this.width + 3,
          width : 12,
          height : h,
          fill : "url(#neutralButtonGrad)",
          stroke : "#A0A0A0",
          maskFill : "url(#neutralButtonMaskGrad)",
          symbol : {
            symbol : pergola.symbols.arrow.tiny.up,
            x : 4,
            y : 4
          }
        });
        this.dec = new pergola.Button()
        .build({
          owner : this,
          parent : this.container,
          x : this.width + 3,
          y : h,
          width : 12,
          height : h,
          fill : "url(#neutralButtonGrad)",
          stroke : "#A0A0A0",
          maskFill : "url(#neutralButtonMaskGrad)",
          symbol : {
            symbol : pergola.symbols.arrow.tiny.down,
            x : 4,
            y : 4
          }
        });
      }

      if (this.caption) {
      	this.caption = new pergola.Caption().build(pergola.extend(this.caption, {owner : this, y : this.tY, parent : this.container}));
			}

      if (this.appendix) {
      	var bbox = this.container.getBBox();
      	this.appendix = new pergola.Caption().build(pergola.extend(this.appendix, {owner : this, parent : this.container, position : "right", x : bbox.x + bbox.width + 6 + (this.appendix.x | 0), y : this.tY}));
			}

      return this;
    }
  });


  extend(pergola.InputCursor.prototype, {
    visibility : "hidden",
		blinkRate : pAttributes.inputCursor.blinkRate,

    blink : function () {
      this.visibility = (this.visibility == "visible") ? "hidden" : "visible";
		  this.bar.setAttributeNS(null, "visibility", this.visibility);
    },

    setPosition : function () {
      var tX = this.user.tX + this.user.element.getComputedTextLength() + .5;
		
	    this.bar.setAttributeNS(null, "x1", tX);
	    this.bar.setAttributeNS(null, "x2", tX);
    },

    remove : function () {
      this.timer.clear();
		  this.user.container.removeChild(this.bar);
		  delete this.user.inputCursor;
    },

    init : function () {
      pergola.extend(this, arguments[0]);

      this.bar = $C({element : "line", x1 : this.user.tX, y1 : this.user.tY + parseInt(this.user["font-size"]) *.3, x2 : this.user.tX, y2 : this.user.tY - parseInt(this.user["font-size"]), stroke : "black", visibility : "hidden", appendTo : this.user.container});

      this.setPosition();

	  	this.timer = pergola.Timer()
	    .initialize({
	      handle : this,
	      callback : this.blink,
	      frequence : this.blinkRate,
	      destroy : "timer"
	    });

      return this;
    }
  });


  extend(pergola.Layout.prototype, {

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Layout");
        return;
      };
      this[this.type]();

      return this;
    },

    table : function () {
      var owner = this.owner,
          attributes = this.attributes || {},
          strokeWidth = attributes['stroke-width'] || 0,
          sw,
          x,
          table;

      this.x = this.x || 0;
      this.y = this.y || 0;
      owner.cells = [];
      strokeWidth = attributes['stroke-width'] || 0;
      this.cellWidth = parseInt((this.width + strokeWidth * 2 - this.x * 2) / this.cols) - this.spacing;
      this.cellHeight = parseInt((this.height + strokeWidth * 2 - this.y * 2) / this.rows) - this.spacing;

      attributes.width = this.cellWidth;
      attributes.height = this.cellHeight;

      if (strokeWidth) {
        sw = strokeWidth;
        if (typeof sw == "string") {
          if (sw.indexOf("px") != -1) sw = parseInt(sw);
          else sw = null;
        }
      }
      else sw = 1;

      if (sw) x = this.setPositionToStrokeWidth(sw);
      if (x) attributes.y = attributes.x = x;

      table = $C({element : "g", id : this.id + "_table", transform : "translate(" + this.x + " " + this.y + ")"});
      for (r = 0; r < this.rows; r++) {
        for (c = 0; c < this.cols; c++) {
          var g = $C({element : "g", transform : "translate(" + ((this.cellWidth + this.spacing)  * c) + " " + ((this.cellHeight + this.spacing)  * r) + ")", appendTo : table});
          $C(pergola.extend({element : 'rect', appendTo : g}, attributes));
          owner.cells.push(g);
        }
      }

      if (this.fn) this.checkUserFunction();
      this.parent.appendChild(table);
    },

    combobox : function () {
      this.owner.comboBox = new pergola.ComboBox(this.owner.id + "_comboBox")
      .build({
        owner : this.owner,
        x : this.x,
        y : this.y,
        width : this.width,
        height : this.height,
        parent : this.parent,
        list : this.list,
        layout : this.layout
      });
      if (this.fn) this.checkUserFunction();
    },

    colorpicker : function () {
      var owner = this.owner;
      pergola.colorpicker = new pergola.ColorPicker(this.owner.id)
      .build({
        owner : owner,
        parent : owner.container,
        x : 32,
        y : owner.topBar.height + 28
      });
    },

    colorpickerExtended : function () {
      var owner = this.owner;
      pergola.colorpicker = new pergola.ColorPickerExtended(this.owner.id)
      .build({
        owner : owner,
        parent : owner.container,
        x : 8,
        y : owner.topBar.height + 8
      });
    },

    tabbed : function () {
      var owner = this.owner,
					tab,
					t,
					active;

      owner.tabs = this.tabs;
      owner.tabAdvanceX = owner.margin;

      for (t in owner.tabs) {
        tab = owner.tabs[t];
        if (tab.active) active = t;
        owner[t] = new pergola.Tab(owner.id + "_" + t);
        pergola.extend( owner[t], tab);
        owner[t].build({
          owner : owner,
          parent : owner.topBar.container,
          x : owner.tabAdvanceX,
          y : owner.topBar.height + 1
        });
        owner.tabAdvanceX += owner[t].width;
      }

      owner.topBar.container.appendChild(owner[active].container);
    },

    message : function () {
      var owner = this.owner;
      owner.messageGroup = $C({element : "g", id : this.id + "_messsageGroup", 'font-size' : "10pt", transform : "translate(20 60)", 'pointer-events' : "none", appendTo : owner.container});
    }
  });


  extend(pergola.Legend.prototype, {
  	display : "block",
  	"font-size" : pAttributes.legend["font-size"],

  	toggleOn : function () {
      this.container.setAttributeNS(null, "display", (this.display = "block"));
    },
    toggleOff : function () {
      this.container.setAttributeNS(null, "display", (this.display = "none"));
    },
    move : function (x, y) {
			this.container.setAttributeNS(null, "transform", "translate(" + x + " " + y + ")");
		},

		build : function () {
      pergola.extend(this, arguments[0]);
     	var fSize,
	  			ty = 0,
	  			dy,
	  			LF,
					textOffset = this.textOffset || 8,
	    		g,
					element,
					image,
					transform = (this.x || this.y) ? {transform: "translate(" + this.x + " " + this.y + ")"} : {},
					attributes = {},
					bbox,
					legend;

			if (this.textAttributes) pergola.extend(attributes, this.textAttributes);
			attributes["font-size"] = attributes["font-size"] || this["font-size"];
			fSize = parseInt(attributes["font-size"]);
			dy = fSize;
	  	LF = this.vSpacing || (dy / 2);
			this.container = $C(pergola.extend({element : "g", id : this.id, appendTo : this.parent}, transform));
			artGroup = $C({element : "g", appendTo : this.container});
			captionGroup = $C(pergola.extend({element : "g", appendTo : this.container}, attributes));

			for (var p in this.legend) {
				var a = this.legend[p];

				if (typeof a.caption == "string") a.caption = [a.caption];
				this[p] = $C({element : "text", y : ty, appendTo : captionGroup});
				for (var i in a.caption) {
					$C({element : 'tspan', x : 0, dy : dy, textNode : a.caption[i], appendTo : this[p]});
				}
	
				if (a.art) {
					if (a.art.image) {
						image = a.art.image;
						$C({element : "image", x : image.x || 0, y : image.y || (ty + dy - image.height), width : image.width, height : image.height, "xlink:href" : image.url, appendTo : artGroup});
					}
					else {
						for (var j in a.art.elements) {
							$C(pergola.extend(a.art.elements[j], {transform : "translate(0 " + (ty + dy) + ")", appendTo : artGroup}));
						}
					}
				}
	
				ty += dy * ++i + LF;
			}
	
			bbox = artGroup.getBBox();
			captionGroup.setAttributeNS(null, "transform", "translate(" + (bbox.width + textOffset) + " 0)");
			this.container.setAttributeNS(null, "display", this.display);

      return this;
    }
  });



  extend(pergola.ListItem.prototype, {
    fill : pAttributes.listItem.fill,
    maskFill : pAttributes.listItem.maskFill,
    textFill : "black",
    textFillInverse : "white",

    effects : function (evt) {
      this.button.setAttributeNS(null, "opacity", evt.type == "mouseover" | 0);
      this.videoInverse(evt);
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null)) return;
      var eType = evt.type;
      if (eType == "mouseover" || eType == "mouseout") {
        this.effects(evt);
      }
      if (eType == "mouseup") {
        var owner = this.owner;
          if (owner.selectionHandle != this) owner.cancelSelection(evt);
          owner.selection = this.string;
          this.unregisterEvents(this.button, "mouseout");
          owner.selectionHandle = this;
          if (evt.detail == 2) {
            var oo = owner.owner;
            if (owner.fn) owner.checkUserFunction(evt);
            else if (oo && oo.fn) oo.checkUserFunction(evt);
            owner.cancelSelection(evt, true);
            if (oo && oo.close) oo.close(evt);
          }
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "ListItem");
        return;
      };

      var owner = this.owner,
          height = owner.listItemHeight,
          x = this.x || this.owner.listMargin;
          y = height - (height - parseInt(owner['font-size'])) / 2 - 1,
          advY = owner.advanceY || 0;

      this.container = $C({element : "g", id : this.id, transform : "translate(0 " + advY + ")", appendTo : this.parent});
      $C({element : "rect", width : this.width, height : height, fill : this.maskFill, appendTo : this.container});
      this.button = $C({element :"rect", width :this.width, height :height, fill :this.fill, opacity :0, appendTo : this.container});
      this.registerEvents(this.button, ["mouseover", "mouseout"]);
      if (owner.constructor == pergola.PopupList) this.registerEvents(this.button, "mousedown");
      else this.registerEvents(this.button, "mouseup");
      this.text = $C({element : "text", x : x, y : y, 'font-size' : owner['font-size'], 'font-family' : owner['font-family'], "pointer-events" : "none", textNode : this.string, appendTo : this.container});
      if ("advanceY" in owner) owner.advanceY += height + 2;

      return this;
    }
  });


  extend(pergola.Menu, {
    opacity : pAttributes.menu.opacity,
    active : false,
    handle : null
  });


  extend(pergola.Menu.prototype, {
    'font-size' : pAttributes.menu['font-size'],
    fill : pAttributes.menu.fill,
    stroke : pAttributes.menu.stroke,
    rx : pAttributes.menu.rx,
    textFill : pAttributes.text.fill,

    close : function (evt) {
      this.list.close(evt);
      for (var a in this.owner.menu) {
        var menu = this.owner.menu[a];
        this.registerEvents(menu.button, "mouseout", menu);
      }
      this.button.setAttributeNS(null, "opacity", 0);
      this.unregisterEvents(pergola.doc, "mousedown");
      pergola.Menu.opacity = 0.4;
      pergola.Menu.active = false;
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);

      var tgt = evt.target,
      		eType = evt.type,
					owner = this.owner;

      if (tgt == this.button) {
        if (eType == "mouseover") {
          tgt.setAttributeNS(null, "opacity", pergola.Menu.opacity);
          if (pergola.Menu.active) {
            var handle = pergola.Menu.handle;
            if (this != handle) {
              handle.button.setAttributeNS(null, "opacity", 0);
              handle.list.close(evt);
              this.unregisterEvents(pergola.doc, "mousedown", handle);
              this.registerEvents(pergola.doc, "mousedown");
            }
            this.list.container.setAttributeNS(null, "display", "block");
          }
        }
        else if (eType == "mouseout") tgt.setAttributeNS(null, "opacity", 0);
        else if (eType == "mousedown") {
        	if (pergola.Selector.handle) pergola.Selector.handle.close(evt);
        	if (pergola.Panel.checkPreemptive(null, true)) return;
          if (owner.constructor == pergola.Window) owner.bringToFront();
          for (var a in owner.menu) {
            var menu = owner.menu[a];
            this.unregisterEvents(menu.button, "mouseout", menu);
          }
          pergola.Menu.active = true;
          this.registerEvents(pergola.doc, "mousedown");
          pergola.Menu.opacity = 0.8;
          tgt.setAttributeNS(null, "opacity", pergola.Menu.opacity);
          this.list.container.setAttributeNS(null, "display", "block");
        }
      }
      else if (evt.currentTarget == pergola.doc) pergola.Menu.handle.close(evt);
      pergola.Menu.handle = this;
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Menu");
        return;
      };

      var height = this.owner.menubar.height - 1,
          width = this.title.width(this['font-size'], this['font-family']) + 20,
          textPos = height - (height - parseInt(this['font-size'])) / 2 - 2;

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      this.button = $C({element : "rect", id : (this.id + "_button"), x : .5, y : .5, rx : this.rx, width : width, height : height, fill : this.fill, stroke : this.stroke, opacity : 0, appendTo : this.container});
      $C({element : 'text', x : width / 2 - 1, y : textPos, fill : this.textFill, 'text-anchor' : 'middle', 'pointer-events' : 'none', textNode : this.title, appendTo : this.container});
      this.registerEvents(this.button, ["mousedown", "mouseover", "mouseout"]);

      this.owner.menubar.add(this);

      this.list = new pergola.MenuList(this.id + "_list")
      .build({
        owner : this,
        menuObject : this,
        parent : this.container,
        y : height + 1,
        items : this.items,
        "font-family" : this["font-family"],
        "font-size" : this["font-size"]
      });

      return this;
    }
  });


  extend(pergola.Menubar.prototype, {
  	"font-size" : pAttributes.menuBar["font-size"],
    height : pAttributes.menuBar.height,
    fill : pAttributes.menuBar.fill,
    advanceX : 0,

		width : function () {return parseInt(this.container.getBBox().width) + 1;},

    add : function (menu) {
    	menu.container.setAttributeNS(null, "transform", "translate(" + this.advanceX + " 0)");
    	this.bar.setAttributeNS(null, "width", (this.advanceX = this.width()));
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", "font-size" : this["font-size"], appendTo : this.parent});
      this.bar = $C({element : "rect", width : 0, height : this.height, fill : this.fill, appendTo : this.container});

      return this;
    }
  });


  extend(pergola.MenuItem.prototype, {
    height : pAttributes.menuItem.height,
    fill : pAttributes.menuItem.fill,
    textFill : "black",
    textFillInverse : "white",
    active  : true,
    exclusive : false,

    effects : function (evt) {
      this.button.setAttributeNS(null, "opacity", evt.type == "mouseover" | 0);
      this.videoInverse(evt);
    },

    activate : function () {
      if (this.active) return;
      this.active = true;
      this.text.setAttributeNS(null, "opacity", 1);
      if (this.submenu) this.arrow.setAttributeNS(null, "opacity", 1);
    },

    deactivate : function () {
      this.active = false;
      this.text.setAttributeNS(null, "opacity", .5);
      if (this.checkmark) {
        this.check = false;
        this.checkmark.setAttributeNS(null, "display", "none");
      }
      if (this.submenu) {
        this.arrow.setAttributeNS(null, "opacity", .5);
      }
    },

    handleEvent : function (evt) {
      if (evt.detail > 1) evt.preventDefault();
      if (evt.type == "mousedown") evt.stopPropagation();
      else if (evt.type == "mouseup") {
        if (!this.active) {
          evt.stopPropagation();
          return;
        }
        if (this.checkmark) {
          if (this.exclusive) {
            for (var i in this.owner.items) {
              var listItem = this.owner[i];
              if (listItem.check && listItem.exclusive) {
                listItem.check = false;
                listItem.checkmark.setAttributeNS(null, "display", "none");
              }
            }
          }
          this.check = this.check ? false  : true;
          this.checkmark.setAttributeNS(null, "display", this.check ? "block" : "none");
        }
        this.effects(evt);
        this.checkUserFunction(evt);
        this.menuObject.close(evt);
      }
      else {
        this.effects(evt);
        if (evt.type == "mouseover") {
          for (var i in this.owner) {
            var owner = this.owner[i];
            var sub = owner.submenu;
            if (sub && sub.up) {
              owner.list.close(evt);
              sub.up = false;
            }
          }
          if (this.submenu) {
            this.list.container.setAttributeNS(null, "display", "block");
            this.submenu.up = true;
          }
          this.unregisterEvents(pergola.doc, "mousedown", this.menuObject);
        }
        else if (evt.type == "mouseout") this.registerEvents(pergola.doc, "mousedown", this.menuObject);
      } 
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "MenuItem");
        return;
      };

      var owner = this.owner;

      this.container = $C({element : "g", id : this.id, transform : "translate(0 " + owner.advanceY + ")", appendTo : this.parent});
      this.button = $C({element : "rect", x : 1, y : 1, width : owner.width, height : this.height, fill : this.fill, opacity : 0, appendTo : this.container});
      this.registerEvents(this.container, ["mouseover", "mouseout", "mousedown"]);
      if (!this.submenu) this.registerEvents(this.button, "mouseup");
      this.text.opacity = +this.active || .5;
      this.text = $C(pergola.extend({element : 'text', appendTo : this.container}, this.text));
      if ("check" in this) {
        var display = this.check ? "block" : "none";
        this.checkmark = pergola.symbol.call(this, {
          symbol : pergola.symbols.checkmark,
          x : 3.5,
          y : 6
        });
        this.checkmark = this.checkmark.firstChild;
        this.checkmark.setAttributeNS(null, "display", display);
      }
      if (this.submenu) {
        this.arrow = pergola.symbol.call(this, {
          symbol : pergola.symbols.menuArrow,
          x : owner.width - 9,
          y :6
        });
        this.arrow = this.arrow.firstChild;
        this.list = new pergola.MenuList(this.id + "_list")
        .build({
          owner : this,
          menuObject : this.menuObject,
          parent : this.container,
          x : owner.width - 1,
          y : -3,
          items : this.submenu.items,
          "font-family" : this["font-family"],
          "font-size" : this["font-size"],
          isSubmenu : true
        });
      }
      if (this.separator) {
        this.separator.build({
          parent : this.container,
          type : "horizontal",
          x : 1,
          y : this.height + 1,
          width : owner.width
        });
        owner.advanceY += this.separator.height;
      }
      owner.advanceY += this.height;

      return this;
    }
  });


  extend(pergola.MenuList.prototype, {
    height : 0,
    margin : pAttributes.menuList.margin,
    advanceY : 0,
    filter : pAttributes.menuList.filter,

    close : function (evt) {
      for (var i in this.items) {
        var listItem = this[i];
        if (evt.type != "mouseover") listItem.effects(evt);
        if (listItem.submenu) {
          listItem.list.close(evt);
        }
      }
      this.container.setAttributeNS(null, "display", "none");
    },

    build : function () {
      pergola.extend(this, arguments[0]);

      var l = [],
          obj;

      for (var prop in this.items) {
        this[prop] = new pergola.MenuItem(this.id + "_" + prop);
        var p = this.items[prop];
        this.height += this[prop].height + (p.separator ? p.separator.height : 0);
        l.push(p.string.width(this['font-size'], this['font-family']));
      }
      this.width = Math.max.apply(null, l) + this.margin + 34;
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      $C({element : "rect", x : .5, y : .5, width : this.width + 5, height : this.height + 5, fill : "#FFFFFF", stroke : this.stroke, filter : this.filter, appendTo : this.container});
      $C({element : "rect", x : 3, y : 3, width : this.width, height : this.height, fill : "#F7F7F7", appendTo : this.container});
    
      var g = $C({element : "g", id : this.id + "_itemsGroup", transform : "translate(2 2)", appendTo : this.container});
  
      for (var prop in this.items) {
        pergola.extend((obj = {}), {
          owner : this,
          menuObject : this.menuObject,
          parent : g,
          "font-family" : this["font-family"],
          "font-size" : this["font-size"],
          text : {
            x : this.margin,
            y : parseInt(this["font-size"]) + 2,
            textNode : this.items[prop].string,
            "pointer-events" : "none"
          }
        });
        pergola.extend(obj, this.items[prop]);              

        this[prop].build(obj);
      }


      this.container.setAttributeNS(null, "display", "none");

      return this;
    }
  });


  extend(pergola.Panel, {
    panels : [],
    checkPreemptive : function (p, flash) {
			for (var i in pergola.Panel.panels) {
				panel = pergola.Panel.panels[i];
				if (panel.isOpen && panel.preemptive) {
					if (p == panel) return false;
					if (flash) {
						panel.timer = pergola.Timer()
			      .initialize({
			        handle : panel,
			        callback : panel.topBar.preemptiveWarning,
			        frequence : 120,
			        repeatCount: 0,
							repeats: 8
			      });
		      }
					return true;
				}
			}
			return false;
		}
  });


  extend(pergola.Panel.prototype, {
    type : "dialog",
    x : pAttributes.panel.x,
    y : pAttributes.panel.y,
    width : pAttributes.panel.width,
    height : pAttributes.panel.height,
    fill : pAttributes.panel.fill,
    'fill-opacity' : 1,
    'stroke-opacity' : 1,
    stroke : pAttributes.frame.stroke,
    footerFill : pAttributes.panel.footerFill,
    margin : pAttributes.panel.margin,
    footerHeight : pAttributes.panel.footerHeight,
    filter : pAttributes.panel.filter,
    display : "none",
    validated : true,
    preemptive : false,
    isOpen : false,
    move : pergola.Panel.superClass.moveByTranslation,

    open : function () {
    	if (pergola.Panel.checkPreemptive(null, true)) return;
    	this.container.setAttributeNS(null, "display", "block");
      this.bringToFront();
      this.isOpen = true;
    },

    bringToFront : function () {
      this.parent.appendChild(this.container);
    },

    close : function (evt) {
      this.container.setAttributeNS(null, "display", "none");
      this.isOpen = false;
    },

    release : function () {
      this.frame.rect.setAttributeNS(null, "filter", this.frame.filter);
      if (pergola.Key.active) pergola.Key.active.stop = true;
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var tgt = evt.target,
      		eType = evt.type,
					panel,
					combo = this.comboBox,
					ok = (this.ok && tgt == this.ok.button);

      if (eType == "mousedown") {
      	if (pergola.Panel.checkPreemptive(this, true)) return;
      	this.bringToFront();
				if (pergola.Selector.handle) pergola.Selector.handle.close(evt);
      	
	      if (combo && !ok) this.comboBox.cancelSelection(evt, true);
      }

      else if (eType == "mouseup") {
      	if (pergola.Panel.checkPreemptive(this)) return;
      	if (pergola.Key.active) {
      		if (this.cancel && tgt == this.cancel.button) pergola.Key.active.warn = false;
      		else pergola.Key.active.warn = true;
					pergola.Key.active.release();
				}
        if (ok) {
					this.checkUserFunction(evt);
          if (this.validated) this.close(evt);
          this.validated = true;
				}
				else this.close(evt);
        if (combo) {
					combo.cancelSelection(evt, true);
				}
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.topBar = new pergola.TopBar(this.id + "_topBar");
      if (this == pergola.notification) this.topBar.fill = pAttributes.topBar.fill.messageBox;
      this.frame = new pergola.Frame(this.id + "_frame")
      .build({
        owner : this,
        parent : this.container,
        width : this.width,
        height : this.height,
        fill : this.fill,
        stroke : this.stroke,
        'fill-opacity' : this['fill-opacity'],
        'stroke-opacity' : this['stroke-opacity'],
        filter : this.filter
      });
  
      if (this.type != "basic") {
        this.registerEvents(this.container, "mousedown");
        this.topBar.build({
          owner : this,
          parent : this.container,
          'font-size' : '9pt'
        });
        this.closeBtn = new pergola.CommandButton(this.id + "_closeButton");
        this.closeBtn.build({
          owner : this,
          parent : this.container,
          x : (this.width - this.closeBtn.width - 6),
          y : 7,
          symbol : {
            symbol : pergola.symbols.winClose,
            x : 5,
            y : 4
          },
          ev : "mouseup",
          fn : function (evt) {
          	if (pergola.Panel.checkPreemptive(this.owner, true)) return;
          	if (pergola.Key.active) {
              pergola.Key.active.warn = false;            
              pergola.Key.active.release();
            }
            this.owner.close(evt);
          }
        });
      }
      else delete this.topBar;

      if (this.type == "dialog") {
        this.footerGroup = $C({element : "g", id : this.id + "_footer", transform : "translate(1 " + (this.height - this.footerHeight) + ")", appendTo : this.container});
        var h = this.footerHeight,
        		rx = this.frame.rx,
						parent = (this.layout && this.layout.type == "colorpickerExtended") ? this.container : this.footerGroup,
        		d = "M0 0h" + (this.width - 1) + "v" + (h - rx) + "q0," + rx + ",-" + rx + "," + rx + "h-" + (this.width - 1 - rx * 2) + "q-" + rx + ",0,-" + rx + ",-" + rx + "z",
						button,
						text;

        $C({element : "path", d : d, fill : this.footerFill, appendTo : this.footerGroup});
        if (this.okButton) {
        	button = this.okButton;
          text = button.text || "OK";
          this.ok = new pergola.DialogButton(this.id + "_ok")
          .build({
            owner : this,
            parent : parent,
            x : button.x || this.width - (button.marginRight || 260),
            y : button.y || 16,
            text : text
          });
        }

        if (this.cancelButton) {
        	button = this.cancelButton;
          text = button.text || "Cancel";
          this.cancel = new pergola.DialogButton(this.id + "_cancel");
          this.cancel.build({
            owner : this,
            parent : parent,
            x : this == pergola.notification ? (this.width - this.cancel.width) / 2 : button.x || this.width - (button.marginRight || 130),
            y : button.y || 16,
            text : text
          });
        }
      }

      if (this.layout) {
        var layout = this.layout;
        this.layout = new pergola.Layout();
        for (var p in layout) this.layout[p] = layout[p];
        var x = layout.x || 0;
        var y = (layout.y || 0) + this.topBar.height * (this.type != "basic");
        this.layout.build({
          owner : this,
          parent : this.container,
          x : x,
          y : y,
          width : layout.width || this.width,
          height : layout.height || this.height - (this.topBar.height * (this.type != "basic") + this.footerHeight * (this.type == "dialog"))
        });
        layout = null;
      }
      this.container.setAttributeNS(null, "display", this.display);
      if (this.contains) {
        if (typeof this.contains == "function") this.contains();
        else this.container.appendChild(this.contains);
      }
      pergola.Panel.panels.push(this);
      this.sweep(['okButton', 'cancelButton']);

      return this;
    }
  });


  extend(pergola.PopupList.prototype, {
    width : pAttributes.popupList.width,
    listItemHeight : pAttributes.popupList.listItemHeight,
    listMargin : pAttributes.popupList.listMargin,
    advanceY : 0,

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "PopupList");
        return;
      };

      var length = this.list.length,
          listItemWidth = this.width - 5;

      this.height = length * this.listItemHeight + 2 * length + 3;
      this.y =  -this.owner.index * this.listItemHeight - 2;
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", display : "none", appendTo : this.parent});
      $C({element : "rect", x : .5, y : .5, width : this.width, height : this.height, fill : "white", stroke : this.stroke, appendTo : this.container});
      this.listGroup = $C({element : "g", transform : "translate(3 3)", appendTo : this.container});
      for (var a in this.list) {
        var prop = this.list[a];
        this[prop] = new pergola.PopupListItem(this.name + '_' + prop)
        .build({
          owner : this,
          parent : this.listGroup,
          width : listItemWidth,
          string : prop
        });
      }

      return this;
    }
  });


  extend(pergola.PopupListItem.prototype, {
    handleEvent : function (evt) {
      var eType = evt.type;

      this.eventBehavior(evt);
      this.effects(evt);
      if (eType == "mousedown") {
        this.registerEvents(this.button, "mouseup");
      }
      else if (eType == "mouseup") {
        var owner = this.owner.owner;
        this.unregisterEvents(this.button, "mouseup");
        owner.selection = this;
        owner.close(evt);
        if (owner.fn) owner.fn(evt);
      }
    }
  });


  extend(pergola.Preview.prototype, {
    rx : pAttributes.preview.rx,
    width : pAttributes.preview.width,
    height : pAttributes.preview.height,
    fill : pAttributes.frame.fill,
    stroke : pAttributes.frame.stroke,
    opacity : pAttributes.preview.opacity,

    show : function () {
      this.container.setAttributeNS(null, "display", "block");
    },
  
    hide : function () {
      this.container.setAttributeNS(null, "display", "none");
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Preview");
        return;
      };
      if (!document.getElementById("previewClip")) {
        var clp = $C({element : "clipPath", id : "previewClip", appendTo : pergola.defs});
        $C({element : "rect", width : this.width - 5, height : this.height - 5, appendTo : clp});
      }
      var doc = this.owner.doc;
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", display : "none", appendTo : this.parent});
      $C({element : "rect", x : .5, rx : this.rx, width : this.width, height : this.height, fill : this.fill, opacity : this.opacity, stroke : this.stroke, appendTo : this.container});
      $C({element : "rect", x : 2.5, y : 2, width : this.width - 4, height : this.height - 4, fill : this.owner.fill, stroke : this.stroke, appendTo : this.container});
      var g = $C({element : "g", "clip-path" : "url(#previewClip)", transform : "translate(3 2.5)", appendTo : this.container});
      doc.cloneGroup = $C({element : "g", transform : "scale(" + this.width / this.owner.width + ")", appendTo : g});
      var clone = doc.transformable.cloneNode(true);
      clone.id = this.owner.id + "_preview";
      var nodes = clone.getElementsByTagNameNS(pergola.ns.svg,"*");
      for (var i = 0; i < nodes.length; i ++) nodes.item(i).id = "";
      doc.cloneGroup.appendChild(clone);

      return this;
    }
  });


  extend(pergola.Progress.prototype, {
    width : pAttributes.progress.width,
    height : pAttributes.progress.height,
    fill : pAttributes.progress.fill,
    stroke : pAttributes.progress.stroke,
    statusFill : pAttributes.progress.statusFill,
    type : "bar",
    advance : 0,

    start : function () {
      this.container.setAttributeNS(null, "display", "block");
    },

    stop : function () {
      this.advance = 0;
      this.status.setAttributeNS(null, "width", 0);
      this.container.setAttributeNS(null, "display", "none");
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Progress");
        return;
      };
      var attributes = {
        width : this.width,
        height : this.height,
        fill : this.fill,
        stroke : this.stroke
      };

      if (this.extra) {
        pergola.extend(attributes, this.extra);
        delete this.extra;
      }

      var strokeWidth = attributes['stroke-width'] || 1 * (this.stroke != "none");
      this.extent = this.width - strokeWidth;
      
      var x = this.setPositionToStrokeWidth(strokeWidth);
      if (x) attributes.y = attributes.x = x;
      var statusPos = strokeWidth / 2 + x;
      
      this.container = $C({element : "g", transform : "translate(" + this.x + " " + this.y + ")", display : "none", appendTo : this.parent});

      if (this.type == "bar") {
        this.rect = $C(pergola.extend({element : 'rect', appendTo : this.container}, attributes));
        this.status = $C({element : "rect", id : this.id, x : statusPos, y : statusPos, rx : attributes.rx | 0, width : 0, height : this.height - strokeWidth, fill : this.statusFill, appendTo : this.container});
      }
      else if (this.type == "working") {
        
      }

      return this;
    }
  });


  extend(pergola.QuickTip.prototype, {
    'font-size' : pAttributes.quickTip['font-size'],
    rx : pAttributes.quickTip.rx,
    fill : pAttributes.quickTip.fill,
    stroke : pAttributes.quickTip.stroke,
    textFill : pAttributes.quickTip.textFill,
    delay : pAttributes.quickTip.delay,

    update : function (d) {
      pergola.removeChildren(this.textGroup);
      d = this.convert(d);

      for (var t in d) $C({element : "text", x : 4, y : (12 + 12 * t), textNode : d[t], appendTo : this.textGroup});
    },

    dims : function () {
      var bbox = this.textGroup.getBBox(),
          browser = pergola.browser;
      
      return {
        width : Math.ceil(bbox.width) + 10,
        height : Math.ceil(bbox.height) + 7 * browser.firefox + 4 * (browser.opera || browser.safari || browser.chrome)
      }
    },

    convert : function (d) {
      var LF = d.indexOf("\n") != -1;

      return (typeof d == "string") ? LF ? d.split("\n") : [d] : d;
    },

    show : function () {
      this.timer.clear();
  
      var g = this.container,
          o = this.owner;
  
      g.setAttributeNS(null, "transform", "translate(" + (this.x || (o.mouse.x - 20 | 0)) + " " + (this.y || (o.mouse.y + 20 | 0)) + ")");
      g.setAttributeNS(null, "display", "block");
      this.unregisterEvents(o.button, ["mousemove"]);
    },

    
    hide : function () {
      this.container.setAttributeNS(null, "display", "none");
    },


    build : function () {
      var dims;

      pergola.extend(this, arguments[0]);
      this.container = $C({element : "g", id : this.id, opacity : this.opacity || 1, appendTo : pergola.quicktips.container});
      this.textGroup = $C({element : "g", 'font-size' : this['font-size'], 'font-family' : this['font-family'], fill : this.textFill, 'pointer-events' : "none", appendTo : this.container});

      this.tip = this.convert(this.tip);
      for (var t in this.tip) $C({element : "text", x : 4, y : (12 + 12 * t), textNode : this.tip[t], appendTo : this.textGroup});
      dims = this.dims();
      this.bubble = $C({element : "rect", x : .5, y : .5, rx : this.rx, width : dims.width, height : dims.height, fill : this.fill, stroke : this.stroke, appendTo : this.container});
      this.container.appendChild(this.textGroup);
      this.container.setAttributeNS(null, "display", "none");

      return this;
    }
  });


  extend(pergola.RadioButton.prototype, {
    fill : pAttributes.radioButton.fill,
    stroke : pAttributes.radioButton.stroke,
    dotFill : pAttributes.radioButton.dotFill,
    r : pAttributes.radioButton.r,
    dotR : pAttributes.radioButton.dotR,

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null, true)) return;
      for (var a in this.owner.options) {
        this.owner.options[a].dot.setAttributeNS(null, "display", "none");
      }
      this.dot.setAttributeNS(null, "display", "block");
      this.owner.selection = this;
      if (this.fn) this.checkUserFunction(evt);
      if (this.owner.fn) this.owner.fn();
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert(pergola.messages.missingOwner + "RadioButton");
        return;
      };
      if (!this.owner.options) this.owner.options = {};
      this.owner.options[this.name] = this;
      if (this.isDefault) {
        this.owner.selection = this;
        var display = "block";
      }
      else var display = "none";

      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.registerEvents(this.container, "mouseup");
      if (this.ev) {
        if (typeof this.ev == "string") this.ev = [this.ev];
        this.registerEvents(this.container, this.ev);
      }
      $C({element : "circle", cx : this.r, cy : this.r, r : this.r, fill : this.fill, stroke : this.stroke, appendTo : this.container});
      this.dot = $C({element : "circle", cx : this.r, cy : this.r, r : this.dotR, fill : this.dotFill, display : display, appendTo : this.container});

      if (this.caption) {
      	this.caption = new pergola.Caption().build(pergola.extend(this.caption, {owner : this, parent : this.container}));
			}

      return this;
    }
  });


  extend(pergola.RubberBox.prototype, {
    stroke : pAttributes.rubberBox.stroke,
    'stroke-width' : pAttributes.rubberBox['stroke-width'],
    'stroke-dasharray' : pAttributes.rubberBox['stroke-dasharray'],
    maskStroke : pAttributes.rubberBox.maskStroke,

    set : function () {
      pergola.extend(this, arguments[0]);
      this.x = this.point.x;
      this.y = this.point.y;
      this.rect.setAttributeNS(null, "x", this.point.x - .5);
      this.rect.setAttributeNS(null, "y", this.point.y - .5);
      this.mask.setAttributeNS(null, "x", this.point.x - .5);
      this.mask.setAttributeNS(null, "y", this.point.y - .5);
      this.container.setAttributeNS(null, "display", "block");
    },

    release : function (evt) {
      var point = pergola.mousePoint(evt);
      this.x = this.tempX ? this.tempX : point.x;
      this.y = this.tempY ? this.tempY : point.y;
      var points = (this.width != 0 && this.height != 0) ?
        [
          {x : this.x, y : this.y},
          {x : this.x + this.width, y : this.y},
          {x : this.x + this.width, y : this.y + this.height},
          {x : this.x, y : this.y + this.height}
        ]
        : 0; 
      this.user[this.fn](evt, points);
      this.reset();
    },

    update : function (evt) {
      var handle = this.handle,
      		mask = handle.mask,
      		rect = handle.rect,
          point = pergola.mousePoint(evt),
          x = handle.x,
          y = handle.y;

      handle.width = point.x - handle.x;
      if (handle.width < 0) {
        x = point.x;
        handle.width *= ~0X0;
      }
      handle.height = point.y - handle.y;
      if (handle.height < 0) {
        y = point.y;
        handle.height *= ~0X0;
      }
      rect.setAttributeNS(null, "x", x);
      mask.setAttributeNS(null, "x", x);
      rect.setAttributeNS(null, "y", y);
      mask.setAttributeNS(null, "y", y);
      rect.setAttributeNS(null, "width", handle.width);
      mask.setAttributeNS(null, "width", handle.width);
      rect.setAttributeNS(null, "height", handle.height);
      mask.setAttributeNS(null, "height", handle.height);
      handle.tempX = x;
      handle.tempY = y;
    },

    reset : function () {
      this.x = this.tempX = this.y = this.tempY = this.width = this.height = 0;
      this.setXYWH(this.rect);
      this.setXYWH(this.mask);
      this.container.setAttributeNS(null, "display", "none");
      this.sweep(["user", "fn", "point"]);
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.parent = pergola.systemComponents;
      this.x = this.y = this.width = this.height = 0;
      this.container = $C({element : "g", id : this.id, fill : "none", 'stroke-width' : this['stroke-width'], "pointer-events" : "none", display : "none", appendTo : this.parent});
      this.mask = $C({element : "rect", width : 0, height : 0, stroke : this.maskStroke, appendTo : this.container});
      this.rect = $C({element : "rect", width : 0, height : 0, stroke : this.stroke, "stroke-dasharray" : this['stroke-dasharray'], appendTo : this.container});

      return this;
    }
  });


  extend(pergola.Scrollbar.prototype, {
    fill : pAttributes.scrollBar.fill,
    stroke : pAttributes.scrollBar.stroke,
    horTrackFill : pAttributes.scrollBar.horTrackFill,
    vertTrackFill : pAttributes.scrollBar.vertTrackFill,
    rx : pAttributes.scrollBar.rx,
    size : pAttributes.scrollBar.size,
    padding : pAttributes.scrollBar.padding,
    min : 0,
    extent : 0,
    scrollType : "regular",
    initial : "start",
    sliderClearance : 2,
    step : pAttributes.scrollBar.step,
    delay : pAttributes.scrollBar.delay,
    stepFrequence : pAttributes.scrollBar.stepFrequence,
    pageFrequence : pAttributes.scrollBar.pageFrequence,
    mapPageFrequence : pAttributes.scrollBar.mapPageFrequence,

    stepScrollTimerInit : function (evt, button, callback) {
      this.callback = callback;
      if (evt.type == "mousedown") {
        this.registerEvents(evt.target, ["mouseout", "mouseup"]);
        this.callback(evt);
        if (button.timer) button.timer.initialize();
        else {
          button.timer = pergola.Timer()
          .initialize({
            handle : this,
            callback : callback,
            delay : this.delay,
            frequence : this.stepFrequence
          });
        }
      }
      else if (evt.type == "mouseup" || evt.type == "mouseout") {
        button.timer.clear();
        this.unregisterEvents(evt.target, ["mouseout", "mouseup"]);
      }
    },

	  horScrollBySliderInit : function (evt) {
	  	this.callback = this.owner.map ? this.mapHorScrollBySlider : this.horScrollBySlider;
	    return {
	      callback : this.callback,
	      handle : this,
	      offset : evt.clientX - this.slider.pos,
	      updateCoordinates : false
	    };
	  },
	
	  vertScrollBySliderInit : function (evt) {
	  	this.callback = this.owner.map ? this.mapVertScrollBySlider : this.vertScrollBySlider;
	    return {
	      callback : this.callback,
	      handle : this,
	      offset : evt.clientY - this.slider.pos,
	      updateCoordinates : false
	    };
	  },
	
	  horScrollBySlider : function (evt) {
	    var handle = this.handle,
	        slider = handle.slider;
	
	    slider.pos = evt.clientX - this.offset;
	    handle.overflow(slider);
	    slider.container.setAttributeNS(null, "transform", "translate(" +  Math.round(slider.pos) + " 0)");
	    slider.scrollTarget.e = Math.round(slider.pos * handle.scrollTarget.wRatio()) * ~0X0;
	    handle.scrollTarget.matrixTransformApply();
	  },
	
	  vertScrollBySlider : function (evt) {
	    var handle = this.handle,
	        slider = handle.slider;
	
	    slider.pos = evt.clientY - this.offset;
	    handle.overflow(slider);
	    slider.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(slider.pos) + ")");
	    slider.scrollTarget.f = Math.round(slider.pos * handle.scrollTarget.hRatio()) * ~0X0;
	    handle.scrollTarget.matrixTransformApply();
	  },
	
	  mapHorScrollBySlider : function (evt) {
	  	if (evt.clientY == this.evtY) return;
	
	    var handle = this.handle,
	        slider = handle.slider,
	        owner = handle.owner,
					map = owner.map;
	
	    slider.pos = evt.clientX - this.offset;
	    handle.overflow(slider);
	    slider.container.setAttributeNS(null, "transform", "translate(" +  Math.round(slider.pos) + " 0)");
	
	 		dir = (evt.clientX > this.evtX) ? -1 : 1;
	    map.panBy({x : dir * 18, y : 0});
	    this.evtX = evt.clientX;
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	  },
	
	  mapVertScrollBySlider : function (evt) {
	  	if (evt.clientY == this.evtY) return;
	
	    var handle = this.handle,
	        slider = handle.slider,
	        owner = handle.owner,
					map = owner.map;
	
	    slider.pos = evt.clientY - this.offset;
	    handle.overflow(slider);
	    slider.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(slider.pos) + ")");
	
	 		dir = (evt.clientY > this.evtY) ? -1 : 1;
	    map.panBy({x : 0, y : dir * 18});
	    this.evtY = evt.clientY;
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	  },
	
		overflow : function (s) {
			if (s.pos < 0) s.pos = 0;
	    else if (s.pos > s.max) s.pos = s.max;
		},
	
	
	  horFwdStepScroll : function () {
	    var s = this.slider,
	        t = this.scrollTarget,
	        owner = this.owner;
	
	    s.pos += this.step / t.wRatio();
	    this.positiveOverflow(s);
			t.ctm.e = Math.round(s.pos * t.wRatio()) * ~0X0;
	    t.matrixTransformApply();
	    s.container.setAttributeNS(null, "transform", "translate(" + Math.round(s.pos) + " 0)");
	  },
	
	  vertFwdStepScroll : function () {
	    var s = this.slider,
	        t = this.scrollTarget,
	        owner = this.owner;
	
	    s.pos += this.step / t.hRatio();
	    this.positiveOverflow(s);
	    t.ctm.f = Math.round(s.pos * t.hRatio()) * ~0X0;
	    t.matrixTransformApply();
	    s.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(s.pos) + ")");
	  },
	
	  horBackStepScroll : function () {
	    var s = this.slider,
	        t = this.scrollTarget,
	        owner = this.owner;
	
	    s.pos -= this.step / t.wRatio();
	    this.negativeOverflow(s);
	    t.ctm.e = Math.round(s.pos * t.wRatio()) * ~0X0;
	    t.matrixTransformApply();
	    s.container.setAttributeNS(null, "transform", "translate(" + Math.round(s.pos) + " 0)");
	  },
	
	  vertBackStepScroll : function () {
	    var s = this.slider,
	        t = this.scrollTarget,
	        owner = this.owner;
	
	    s.pos -= this.step / t.hRatio();
	    this.negativeOverflow(s);
	    t.ctm.f = Math.round(s.pos * t.hRatio()) * ~0X0;
	    t.matrixTransformApply();
	    s.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(s.pos) + ")");
	  },
	
	  mapHorFwdStepScroll : function () {
	    var s = this.slider,
	        owner = this.owner,
					map = owner.map;
	
	    map.panBy({x : -16, y : 0});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	    s.pos += ((map.zoomRange()[1] - map.zoom() + 1) / 100);
	    s.container.setAttributeNS(null, "transform", "translate(" + s.pos + " 0)");
	  },
	
	  mapVertFwdStepScroll : function () {
	    var s = this.slider,
	        owner = this.owner,
					map = owner.map;
	
	    map.panBy({x : 0, y : -16});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	    s.pos += ((map.zoomRange()[1] - map.zoom() + 1) / 100);
	    s.container.setAttributeNS(null, "transform", "translate(0 " + s.pos + ")");
	  },
	
	  mapHorBackStepScroll : function () {
	    var s = this.slider,
	        owner = this.owner,
					map = owner.map;
	
	    map.panBy({x : 16, y : 0});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	    s.pos -= ((map.zoomRange()[1] - map.zoom() + 1) / 100);
	    s.container.setAttributeNS(null, "transform", "translate(" + s.pos + " 0)");
	  },
	
	  mapVertBackStepScroll : function () {
	    var s = this.slider,
	        owner = this.owner,
					map = owner.map;
	
	    map.panBy({x : 0, y : 16});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	    s.pos -= ((map.zoomRange()[1] - map.zoom() + 1) / 100);
	    s.container.setAttributeNS(null, "transform", "translate(0 " + s.pos + ")");
	  },
	
	  positiveOverflow : function (s) {
	    if (s.pos > s.max) s.pos = s.max;
	  },
	
	  negativeOverflow : function (s) {
	    if (s.pos < 0) s.pos = 0;
	  },
	
	  horScrollByPageInit : function (evt) {
	    var point = pergola.mousePoint(evt),
	        absX = point.x - this.absoluteX(this.container.parentNode) - this.x;
	/*
	 * workaround for Firefox getCTM() bug (https://bugzilla.mozilla.org/show_bug.cgi?id=873106)
	*/
			if (pergola.browser.firefox) absX = point.x - this.ff_absoluteX() - this.x;
	
	    if (evt.type == "mousedown") {
	      this.removePointerEvents([this.slider.bar]);
	      this.registerEvents(evt.target, ["mousemove", "mouseout", "mouseup"]);
	      this.offset = absX;
	      if (this.offset > this.slider.pos + this.slider.size) {
	      	this.dir = -1;
					this.callback = this.owner.map ? this.mapHorScrollByPage : this.horScrollByPage;
				}
				else if (this.offset < this.slider.pos) {
					this.dir = 1;
					this.callback = this.owner.map ? this.mapHorScrollByPage : this.horScrollByPage;
				}
	      this.callback();
	      if (this.timer) this.timer.initialize();
	      else {
	        this.timer = pergola.Timer()
	        .initialize({
	          handle : this,
	          callback : this.callback,
	          delay : 250,
	          frequence : this.pageFrequence
	        });
	      }
	    }
	    else if (evt.type == "mousemove") {
	      this.offset = absX;
	    }
	    else if (evt.type == "mouseup" || evt.type == "mouseout") {
	      this.timer.clear();
	      this.unregisterEvents(evt.target, ["mousemove", "mouseout", "mouseup"]);
	      this.setPointerEvents([this.slider.bar]);
	    }
	  },
	
	  vertScrollByPageInit : function (evt) {
	    var point = pergola.mousePoint(evt),
	        absY = point.y - this.absoluteY(this.container.parentNode) - this.y;
	/*
	 * workaround for Firefox getCTM() bug (https://bugzilla.mozilla.org/show_bug.cgi?id=873106)
	*/
			if (pergola.browser.firefox) absY = point.y - this.ff_absoluteY() - this.y;
	
	    if (evt.type == "mousedown") {
	      this.removePointerEvents([this.slider.bar]);
	      this.registerEvents(evt.target, ["mousemove", "mouseout", "mouseup"]);
	      this.offset = absY;
	      if (this.offset > this.slider.pos + this.slider.size) {
	      	this.dir = -1;
					this.callback = this.owner.map ? this.mapVertScrollByPage : this.vertScrollByPage;
				}
				else if (this.offset < this.slider.pos) {
					this.dir = 1;
					this.callback = this.owner.map ? this.mapVertScrollByPage : this.vertScrollByPage;
				}
	      this.callback();
	      if (this.timer) this.timer.initialize();
	      else {
	        this.timer = pergola.Timer()
	        .initialize({
	          handle : this,
	          callback : this.callback,
	          delay : 250,
	          frequence : this.pageFrequence
	        });
	      }
	    }
	    else if (evt.type == "mousemove") {
	      this.offset = absY;
	    }
	    else if (evt.type == "mouseup" || evt.type == "mouseout") {
	      this.timer.clear();
	      this.unregisterEvents(evt.target, ["mousemove", "mouseout", "mouseup"]);
	      this.setPointerEvents([this.slider.bar]);
	    }
	  },
	
	  horScrollByPage : function () {
	    var t = this.scrollTarget,
					ctm = t.ctm,
					s = this.slider,
					max = t.canvas.width * t.scaleFactor - t.visibleW;
	
	    if (this.offset > s.pos && this.offset < s.pos + s.size) {
				this.timer.clear();
				return;
			}
	
			ctm.e += this.page * this.dir;
			if (ctm.e > 0) ctm.e = 0;
	    else if (Math.abs(ctm.e) > max) ctm.e = max * ~0X0;
	    s.pos = ctm.e / t.wRatio() * ~0X0;
			s.container.setAttributeNS(null, "transform", "translate(" + Math.round(s.pos) + " 0)");
			t.matrixTransformApply();
	  },
	
	  vertScrollByPage : function () {
	    var t = this.scrollTarget,
					ctm = t.ctm,
					s = this.slider,
					max = t.canvas.height * t.scaleFactor - t.visibleH;
	
	    if (this.offset > s.pos && this.offset < s.pos + s.size) {
				this.timer.clear();
				return;
			}
	
			ctm.f += this.page * this.dir;
			if (ctm.f > 0) ctm.f = 0;
	    else if (Math.abs(ctm.f) > max) ctm.f = max * ~0X0;
	    s.pos = ctm.f / t.hRatio() * ~0X0;
			s.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(s.pos) + ")");
	    t.matrixTransformApply();
	  },
	
	  mapHorScrollByPage : function () {
	    var owner = this.owner,
					map = owner.map,
					s = this.slider;
	
	    if (this.offset > s.pos && this.offset < s.pos + s.size) {
				this.timer.clear();
				return;
			}
			map.panBy({x : owner.doc.visibleW * this.dir, y : 0});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	
	    this.slider.pos -= ((map.zoomRange()[1] - map.zoom() + 1) / 10) * this.dir;
			this.slider.container.setAttributeNS(null, "transform", "translate(" + Math.round(this.slider.pos) + " 0)");
	  },
	
	  mapVertScrollByPage : function () {
	    var owner = this.owner,
					map = owner.map,
					s = this.slider;
	
	    if (this.offset > s.pos && this.offset < s.pos + s.size) {
				this.timer.clear();
				return;
			}
			map.panBy({x : 0, y : this.page * this.dir});
	    if (owner.status) owner.status.center(map.center());
	    if (map.grid) owner.mapGutterPositionOnScroll();
	
	    this.slider.pos -= ((map.zoomRange()[1] - map.zoom() + 1) / 10) * this.dir;
			this.slider.container.setAttributeNS(null, "transform", "translate(0 " + Math.round(this.slider.pos) + ")");
	  },

    release : function () {
      if (pergola.Key.active) pergola.Key.active.stop = true;
    },

    handleEvent : function (evt) {
      var tgt = evt.target,
          eType = evt.type;

      this.eventBehavior(evt);
      if (eType == "mousedown") {
      	if (pergola.Panel.checkPreemptive(null, true)) return;
        if (pergola.Menu.active) pergola.Menu.handle.close(evt);
        if (this.owner.constructor == pergola.Window) this.owner.bringToFront();
        if (pergola.Key.active) pergola.Key.active.stop = false;
      }
      if (tgt == this.slider.bar) pergola.dragarea.activate(this.sliderFn(evt));
      else if (tgt == this.track) this.fn(evt);
      else if (tgt == this.backScroll.button) this.stepScrollTimerInit(evt, this.backScroll.button, this.backScroll.fn);
      else if (tgt == this.fwdScroll.button) this.stepScrollTimerInit(evt, this.fwdScroll.button, this.fwdScroll.fn);
    },

    buildH : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Scrollbar");
        return;
      };

      if (this.scrollType == "navigation") {
        if (this.owner.type = "map") this.pageFrequence = this.mapPageFrequence;
        this.initial = "middle";
        var sliderWidth = 16;
      }

      this.fn = this.fn || this.horScrollByPageInit;
      this.sliderFn = this.sliderFn || this.horScrollBySliderInit;
      this.backScrollFn = this.backScrollFn || (this.owner.type == "map") ? this.mapHorBackStepScroll : this.horBackStepScroll;
      this.fwdScrollFn = this.fwdScrollFn || (this.owner.type == "map") ? this.mapHorFwdStepScroll : this.horFwdStepScroll;

      this.scrollButtonSize = this.size + 2;
      this.margins = this.scrollButtonSize * 2;
      this.x = this.scrollTarget.x + this.scrollButtonSize;

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      this.registerEvents(this.container, "mousedown");

      var o = {element : "rect", x : this.scrollButtonSize - .5, y : .5, rx : this.rx, width : 0, height : this.size - 1, fill : this.horTrackFill, stroke : this.stroke, appendTo : this.container};
      if (this.rx) pergola.extend(o, {rx : this.rx});
      this.track = $C(o);

      this.slider = new pergola.ScrollSlider(this.id + "_slider")
      .buildH({
      	owner : this,
        parent : this.container,
        pos : 0,
        x : this.scrollButtonSize + this.sliderClearance / 4,
        y : this.padding,
        width : sliderWidth | 0,
        height : this.size - this.padding * 2,
        scrollTarget : this.scrollTarget.ctm
      });

      this.backScroll = new pergola.Button(this.id + "_backScroll")
      .build({
        owner : this,
        parent : this.container,
        fn : this.backScrollFn,
        width : this.size + 1,
        height : this.size -1,
        stroke : this.stroke,
        symbol : {
          symbol : this.symbol.left,
          x : "center",
          y : "center"
        }
      });

      this.fwdScroll = new pergola.Button(this.id + "_fwdScroll")
      .build({
        owner : this,
        parent : this.container,
        fn : this.fwdScrollFn,
        width : this.size + 1,
        height : this.size - 1,
        stroke : this.stroke,
        symbol : {
          symbol : this.symbol.right,
          x : "center",
          y : "center"
        }
      });

      return this;
    },

    buildV : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "Scrollbar");
        return;
      };

      if (this.scrollType == "navigation") {
        if (this.owner.type = "map") this.pageFrequence = 500;
        this.initial = "middle";
        var sliderHeight = 16;
      }

      this.fn = this.fn || this.vertScrollByPageInit;
      this.sliderFn = this.sliderFn || this.vertScrollBySliderInit;
      this.backScrollFn = this.backScrollFn || (this.owner.type == "map") ? this.mapVertBackStepScroll : this.vertBackStepScroll;
      this.fwdScrollFn = this.fwdScrollFn || (this.owner.type == "map") ? this.mapVertFwdStepScroll : this.vertFwdStepScroll;

      this.scrollButtonSize = this.size + 2;
      this.margins = this.scrollButtonSize * 2;
      this.y = this.scrollTarget.y + this.scrollButtonSize;
  
      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      this.registerEvents(this.container, "mousedown");

      var o = {element : "rect", x : .5, y : this.scrollButtonSize - .5, rx : this.rx, width : this.size - 1, height : 0, fill : this.vertTrackFill, stroke : this.stroke, appendTo : this.container};
      if (this.rx) pergola.extend(o, {rx : this.rx});
      this.track = $C(o);

      this.slider = new pergola.ScrollSlider(this.id + "_slider")
      .buildV({
      	owner : this,
        parent : this.container,
        pos : 0,
        x : this.padding,
        y : this.scrollButtonSize + this.sliderClearance / 4,
        width : this.size - this.padding * 2,
        height : sliderHeight | 0,
        scrollTarget : this.scrollTarget.ctm
      });
      this.backScroll = new pergola.Button(this.id + "_backScroll")
      .build({
        owner : this,
        parent : this.container,
        fn : this.backScrollFn,
        width : this.size - 1,
        height : this.size + 1,
        stroke : this.stroke,
        symbol : {
          symbol : this.symbol.up,
          x : "center",
          y : "center"
        }
      });
      this.fwdScroll = new pergola.Button(this.id + "_fwdScroll")
      .build({
        owner : this,
        parent : this.container,
        fn : this.fwdScrollFn,
        width : this.size - 1,
        height : this.size + 1,
        stroke : this.stroke,
        symbol : {
          symbol : this.symbol.down,
          x : "center",
          y : "center"
        }
      });

      return this;
    }
  });


  extend(pergola.ScrollSlider.prototype, {
    rx : pAttributes.scrollSlider.rx,
    fill : pAttributes.scrollSlider.fill,
    stroke : pAttributes.scrollSlider.stroke,
    minSize : 16,

    buildH : function () {
      pergola.extend(this, arguments[0]);

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      var o = {element : "rect", x : this.x, y : this.y, width : this.width, height : this.height, fill : this.fill.horizontal, stroke : this.stroke.horizontal, appendTo : this.container};
      if (this.rx) pergola.extend(o, {rx : this.rx});
      this.bar = $C(o);      

      return this;
    },
    buildV : function () {
      pergola.extend(this, arguments[0]);

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      var o = {element : "rect", x : this.x, y : this.y, width : this.width, height : this.height, fill : this.fill.vertical, stroke : this.stroke.vertical, appendTo : this.container};
      if (this.rx) {pergola.extend(o, {rx : this.rx})};			      
      this.bar = $C(o);

      return this;
    }
	});


  extend(pergola.Selector, {
    handle : null
  });

  extend(pergola.Selector.prototype, {
    'font-size' : pAttributes.selector['font-size'],
    fill : pAttributes.selector.fill,
    stroke : pAttributes.selector.stroke,
    'stroke-width' : pAttributes.selector['stroke-width'],
    maskFill : pAttributes.selector.maskFill,
    rx : pAttributes.selector.rx,
    width : pAttributes.selector.width,
    height : pAttributes.selector.height,
    listItemHeight : 20,
    selection : null,
    index : 0,

    close : function (evt) {
      if (this.selection) {
        for (var a in this.list) {
          if (this.selection.string == this.list[a]) {
            this.index = +a;
            this.popUpList.container.setAttributeNS(null, "transform", "translate(0 " + (-(this.popUpList.listItemHeight + 2) * this.index - 2) + ")");
            break;
          }
        }
        this.textDataUpdate(this.selection.string);
      }
      this.unregisterEvents(pergola.doc, "mousedown");
      this.popUpList.container.setAttributeNS(null, "display", "none");
      pergola.Selector.handle = null;
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null, true)) return;
      if (pergola.Selector.handle) pergola.Selector.handle.close(evt);
      this.parent.appendChild(this.container);
      this.popUpList.container.setAttributeNS(null, "display", "block");
      if (evt.target == this.button) this.registerEvents(pergola.doc, "mousedown");
      else if (evt.currentTarget == pergola.doc) {
        this.popUpList.container.setAttributeNS(null, "display", "none");
        this.unregisterEvents(pergola.doc, "mousedown");
      }
      pergola.Selector.handle = this;
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      var g = $C({element : "g", transform : "translate(0 " + this.y + ")", appendTo : this.parent});
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.button = $C({element : "rect", x : .5, y : .5, rx : this.rx, width : this.width, height : this.height, fill : this.fill, stroke : this.stroke, appendTo : this.container});
      this.registerEvents(this.container, "mousedown");
      $C({element : "path", d : ("M8,14v-6.5h" + (this.width - 15)), fill : "none", stroke : "#FFFFFF", 'stroke-width' : 8, "stroke-linecap" : "round", "stroke-linejoin" : "round", 'pointer-events' : "none", appendTo : this.container});
      this.text = $C({element : "text", x : 9, y : 15, 'font-size' : this['font-size'], fill : "#204020", 'pointer-events' : "none", textNode : this.list[this.index], appendTo : this.container});
      
      $C({element : "rect", x : 2, y : 2, rx : this.rx - 1.5, width : this.width - 3, height : this.height - 3, fill : "none", stroke : "#E3F1F6", 'stroke-width' : 2, opacity : 1, 'pointer-events' : "none", appendTo : this.container});
      $C({element : "path", d : ("M" + (this.width - 36) + "," + this.height + "h30q6,0,6-6v-8q0-6-6-6h-10z"), fill : "#E5E9E5", stroke : "none", 'opacity' : .35, 'pointer-events' : "none", appendTo : this.container});
      pergola.symbol.call(this, {
        symbol : pergola.symbols.selectorArrow.up,
        x : this.width - 10,
        y : 5
      });
      pergola.symbol.call(this, {
        symbol : pergola.symbols.selectorArrow.down,
        x : this.width - 10,
        y : 17
      });
      this.popUpList = new pergola.PopupList(this.id + "_popUpList")
      .build({
        owner : this,
        parent : this.container,
        width : this.width,
        list : this.list
      });
      
      if (this.caption) {
      	this.caption = new pergola.Caption().build(pergola.extend(this.caption, {owner : this, parent : this.container}));
			}

    }
  });


  extend(pergola.Separator.prototype, {
    width : pAttributes.separator.width,
    height : pAttributes.separator.height,

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.type) {
        alert(pergola.messages.missingSeparatorType);
        return;
      }
      else if (this.type != "horizontal" && this.type != "vertical") {
        alert(pergola.messages.wrongSeparatorType + this.type);
        return;
      }
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", "pointer-events" : "none", appendTo : this.parent});
      if (this.type == "horizontal") {
        $C({element : "path", d : "M0," + (this.height / 2) + "h" + this.width, stroke : this.stroke, appendTo : this.container});
      }
      else {
        $C({element : "rect", fill : "none", width : this.width, height : this.height, appendTo : this.container});
        $C({element : "path", d : "M" + (this.width / 2 + .5) + ",0v" + this.height, stroke : "white", "stroke-linecap" : "round", appendTo : this.container});
        $C({element : "path", d : "M" + (this.width / 2) + ",0v" + this.height, stroke : this.stroke, "stroke-linecap" : "round", appendTo : this.container});
      }
      this.sweep(['type']);

      return this;
    }
  });


  extend(pergola.Slider.prototype, {
    'font-size' : pAttributes.slider['font-size'],
    shape : pAttributes.slider.shape,
    size : pAttributes.slider.size,
    type : pAttributes.slider.type,
    numberOfSteps : 4,
    initial : pAttributes.slider.initial,
    extent : 127,
    valueTip : pAttributes.slider.valueTip,
    rx : pAttributes.slider.rx,
    fill : pAttributes.slider.fill,
    stroke : pAttributes.slider.stroke,
    trackFill : pAttributes.slider.trackFill,
    trackStroke : pAttributes.slider.trackStroke,
    trackOpacity : pAttributes.slider.trackOpacity,
    slotStroke : pAttributes.slider.slotStroke,
    slotSize : pAttributes.slider.slotSize,

    release : function () {
      this.valueTipGroup.setAttributeNS(null, "display", "none");
      if (this.quickTip) {
        if (this.quickTip.timer) this.quickTip.timer.clear();
        this.quickTip.hide();
      }
    },

	  horizontalSliderInit : function (evt) {
	    return {
	      callback : this.horizontalSlideByButton,
	      handle : this,
	      offset : evt.clientX - this.pos,
	      updateCoordinates : false
	    }
	  },
	
	  verticalSliderInit : function (evt) {
	    return {
	      callback : this.verticalSlideByButton,
	      handle : this,
	      offset : evt.clientY - this.pos,
	      updateCoordinates : false
	    }
	  },

	  horizontalSlideByButton : function (evt) {
	    var handle = this.handle,
	        x = evt.clientX - this.offset;
	
	    if (handle.type == "discrete") {
	      if ((x < handle.pos && x > handle.pos - handle.step) || (x > handle.pos && x < handle.pos + handle.step)) return;
	      else handle.pos += handle.step * (x > handle.pos) - handle.step * (x < handle.pos);
	    }
	    else handle.pos = x;
	    if (handle.pos < 0) handle.pos = 0;
	    if (handle.pos > handle.extent) handle.pos = handle.extent;
	    handle.buttonGroup.setAttributeNS(null, "transform", "translate(" + handle.pos + " 0)");
	    handle.textDataUpdate(handle.pos);
	    if (handle.fn) handle.checkUserFunction(evt);
	  },
	
	  verticalSlideByButton : function (evt) {
	    var handle = this.handle,
	        y = evt.clientY - this.offset;
	    if (handle.type == "discrete") {
	      if ((y < handle.pos && y > handle.pos - handle.step) || (y > handle.pos && y < handle.pos + handle.step)) return;
	      else handle.pos += handle.step * (y > handle.pos) - handle.step * (y < handle.pos);
	    }
	    else handle.pos = y;
	    if (handle.pos < 0) handle.pos = 0;
	    if (handle.pos > handle.extent) handle.pos = handle.extent;
	    handle.buttonGroup.setAttributeNS(null, "transform", "translate(0 " + handle.pos + ")");
	    handle.textDataUpdate(handle.pos);
	    if (handle.fn) handle.checkUserFunction(evt);
	  },
	
	  horizontalSlideByTrack : function (evt) {
	    var absX = this.absoluteX(this.container) + this.pos,
	        point = pergola.mousePoint(evt);
	
	    if (this.type == "discrete") this.pos += this.step * (point.x > absX) - this.step * (point.x < absX);
	    else if (this.type == "linear") this.pos += point.x - absX;
	    this.buttonGroup.setAttributeNS(null, "transform", "translate(" + this.pos + " 0)");
	    this.textDataUpdate(this.pos);
	    if (this.fn) this.checkUserFunction(evt);
	  },
	
	  verticalSlideByTrack : function (evt) {
	    var absY = this.absoluteY(this.container) + this.pos,
	        point = pergola.mousePoint(evt);
	
	    if (this.type == "discrete") this.pos += this.step * (point.y > absY) - this.step * (point.y < absY);
	    else if (this.type == "linear") this.pos += point.y - absY;
	    this.buttonGroup.setAttributeNS(null, "transform", "translate(0 " + this.pos + ")");
	    this.textDataUpdate(this.pos);
	    if (this.fn) this.checkUserFunction(evt);
	  },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null, true)) return;
      if (evt.type == "mousedown" && evt.target == this.button) {
        if (this.valueTip) this.valueTipGroup.setAttributeNS(null, "display", "block");
        if (this.quickTip) this.quickTipInit(evt);
        pergola.dragarea.activate(this.sliderFn(evt));
      }
      else if (evt.target == this.track) this.trackFn(evt);
    },

    buildH : function () {
      pergola.extend(this, arguments[0]);
      this.sliderFn = this.horizontalSliderInit;
      this.trackFn = this.horizontalSlideByTrack;
      this.size = this.size.evenToOdd();

      var size = this.size,
          s = this.shape,
          i = this.initial,
          center = size / 2,
          slotIsEven = this.slotSize.isEven(),
          trackY;

      this.trackSize = this.trackSize || size + 1;
      trackY = center - .5 * slotIsEven - this.trackSize / 2;

      if (s == "round") var trackRx = this.trackSize / 2;
      else var trackRx = 4;
      if (s != "round" && s != "square") {
        alert (pergola.messages.header + this.name + pergola.messages.wrongButtonShape + s);
        return;
      }
      if (i != "start" && i != "middle" && i != "end") {
        alert (pergola.messages.header + this.name + pergola.messages.wrongSliderPosition + i);
        return;
      }

      this.pos = parseInt(this.extent / 2) * (i == "middle") + this.extent * (i == "end");

      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.registerEvents(this.container, "mousedown");
      this.track = $C({element : "rect", y : -.5 -.5 * slotIsEven, rx : trackRx, width : this.extent, height : this.trackSize, fill : this.trackFill, stroke : this.trackStroke, opacity : this.trackOpacity, appendTo : this.container});
      if (this.slotSize != 0) $C({element : "path", d : ("M0," + (center - .5 * slotIsEven) + "h" + this.extent), stroke : this.slotStroke, "stroke-width" : this.slotSize, "pointer-events" : "none", appendTo : this.container});
      if (this.type == "discrete") {
        if (this.initial == "middle") this.numberOfSteps = this.numberOfSteps.oddToEven();
        this.step = this.extent / this.numberOfSteps;
        var g = $C({element : "g", transform : "translate(0 -4)", appendTo : this.container});
        for (var i = 1; i < this.numberOfSteps; i ++) $C({element : "path", d : ("M" + (parseInt(this.step * i) + .5) + ",0v1"), stroke : "#878787", appendTo : g});
      }
      this.buttonGroup = $C({element : "g", id : this.id + "_buttonGroup", transform : "translate(" + this.pos + " 0)", appendTo : this.container});
      if (s == "round") {
        this.button = $C({element : "circle", cx : 0, cy : center - .5 * slotIsEven, r : center, fill : this.fill, stroke : this.stroke, appendTo : this.buttonGroup});
      }
      else {
        this.button = $C({element : "rect", x : -center, y : -.5 * slotIsEven, rx : this.rx, width : size, height : size, fill : this.fill, stroke : this.stroke, appendTo : this.buttonGroup});
      }

      this.valueTipGroup = $C({element : "g", id : this.id + "_valueTipGroup", transform : "translate(0 " + (-size) + ")", display : "none", appendTo : this.buttonGroup});
      $C({element : "rect", x : -8.5, y : -7.5, width : 18, height : 14, fill : "url(#quickTipGrad)", stroke : "#989898", appendTo : this.valueTipGroup});
      this.text = $C({element : "text", y : 3, 'font-size' : this['font-size'], 'text-anchor' : 'middle', 'pointer-events' : "none", textNode : this.pos, appendTo : this.valueTipGroup});
      if (this.quickTip) this.quickTipFormat();

      return this;
    },

    buildV : function () {
      pergola.extend(this, arguments[0]);
      this.sliderFn = this.verticalSliderInit;
      this.trackFn = this.verticalSlideByTrack;
      this.size = this.size.evenToOdd();

      var size = this.size,
          s = this.shape,
          i = this.initial,
          center = size / 2,
          slotIsEven = this.slotSize.isEven(),
          trackX;

      this.trackSize = this.trackSize || size + 1;
      var trackX = center - .5 * slotIsEven - this.trackSize / 2;

      if (s == "round") var trackRx = this.trackSize / 2;
      else var trackRx = 4;
      if (s != "round" && s != "square") {
        alert (pergola.messages.header + this.name + pergola.messages.wrongButtonShape + s);
        return;
      }
      if (i != "start" && i != "middle" && i != "end") {
        alert (pergola.messages.header + this.name + pergola.messages.wrongSliderPosition + i);
        return;
      }

      this.pos = parseInt(this.extent / 2) * (i == "middle") + this.extent * (i == "end");
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.registerEvents(this.container, "mousedown");
      this.track = $C({element : "rect", x : trackX, rx : trackRx, width : this.trackSize, height : this.extent, fill : this.trackFill, stroke : this.trackStroke, opacity : this.trackOpacity, appendTo : this.container});
      if (this.slotSize != 0) $C({element : "path", d : ("M" + (center - .5 * slotIsEven) + ",0v" + this.extent), stroke : this.slotStroke, "stroke-width" : this.slotSize, "pointer-events" : "none", appendTo : this.container});

      if (this.type == "discrete") {
        if (this.initial == "middle") this.numberOfSteps = this.numberOfSteps.oddToEven();
        this.step = this.extent / this.numberOfSteps;
        var g = $C({element : "g", transform : "translate(-4 0)", appendTo : this.container});
        for (var i = 1; i < this.numberOfSteps; i ++) $C({element : "path", d : ("M-3," + (parseInt(this.step * i) + .5) + "h1"), stroke : "#878787", appendTo : this.container});
      }
      
      this.buttonGroup = $C({element : "g", id : this.id + "_buttonGroup", transform : "translate(0 " + this.pos + ")", appendTo : this.container});
      if (s == "round") {
        this.button = $C({element : "circle", cx : center - .5 * slotIsEven, cy : 0, r : center, fill : this.fill, stroke : this.stroke, appendTo : this.buttonGroup});
      }
      else {
        this.button = $C({element : "rect", x : -.5 * slotIsEven, y : -center, rx : this.rx, width : size, height : size, fill : this.fill, stroke : this.stroke, appendTo : this.buttonGroup});
      }
      
      this.valueTipGroup = $C({element : "g", id : this.id + "_valueTipGroup", transform : "translate(-16 0)", display : "none", appendTo : this.buttonGroup});
      $C({element : "rect", x : -8.5, y : -6.5, width : 18, height : 14, fill : "url(#quickTipGrad)", stroke : "#A8A8A8", appendTo : this.valueTipGroup});
      this.text = $C({element : "text", y : 4, 'font-size' : this['font-size'], 'text-anchor' : 'middle', 'pointer-events' : "none", textNode : this.pos, appendTo : this.valueTipGroup});
      if (this.quickTip) this.quickTipFormat();

      return this;
    }
  });


  extend(pergola.StatusBar.prototype, {
    fill : pAttributes.statusBar.fill,
    height : pAttributes.statusBar.height,
    "font-size" : pAttributes.statusBar["font-size"],
    textFill : pAttributes.text.fill,
    advanceX : pAttributes.statusBar.advanceX,

    center : function (c) {
      this.centerLon.firstChild.data = c.lon.toFixed(4);
      this.centerLat.firstChild.data = c.lat.toFixed(4);
    },

		location : function (c) {
			var lonInRange = this.owner.lonToRange(c.lon);
      this.owner.locLon.value = this.owner.locLon.element.firstChild.data = lonInRange.toFixed(4);
      this.owner.locLat.value = this.owner.locLat.element.firstChild.data = c.lat.toFixed(4);
    },

    zoom : function (z) {
      if (!this.owner.zoomLevel) return;

    	var value = (this.owner.map) ? z : (z * 100).trim(3);

			this.owner.zoomLevel.value = value;
      this.owner.zoomLevel.element.firstChild.data = value;
    },

    coordinates : function (evt) {
      var win = pergola.Window.active(),
          doc = win.doc,
          x = (evt.clientX - (doc.absoluteX(doc.container) + doc.ctm.e)) / doc.scaleFactor,
          y = (evt.clientY - (doc.absoluteY(doc.container) + doc.ctm.f)) / doc.scaleFactor;
/*
 * workaround for Firefox getCTM() bug (https://bugzilla.mozilla.org/show_bug.cgi?id=873106)
*/
		if (pergola.browser.firefox) {
			x = (evt.clientX - (doc.ff_absoluteX() + doc.x + doc.ctm.e)) / doc.scaleFactor,
      y = (evt.clientY - (doc.ff_absoluteY() + doc.y + doc.ctm.f)) / doc.scaleFactor;
		}

      if (win.status) {
        win.status.coorX.firstChild.data = window.sketch ? sketch.convertToSI(x) : x;
        win.status.coorY.firstChild.data = window.sketch ? sketch.convertToSI(y) : y;
      }
    },

    addFields : function () {
      var field,
					node,
					m = pergola.messages.fieldExists;

      if (arguments[0]) this.fields = arguments[0];
      for (var a in this.fields) {
        if (a in this) {
          alert(m[0] + a + m[1]);
          continue;
        }
        field = this.fields[a];
        if (field.input) {
        	field.input.owner = field.input.owner || this.owner.doc;
        	field.input.parent = this.container;
          this.owner[a] = new pergola.Input()
          .build(field.input);
        }
        else {
          g = $C({element : "g", transform : "translate(" + this.advanceX + " " + 13 + ")", 'pointer-events' : "none", appendTo : this.container});
          t = $C({element : "text", 'font-size' : this['font-size'], fill : this.textFill, textNode : field.caption + " ", appendTo : g});
          this[a] = $C({element : "tspan", textNode : field.value || " ", appendTo : t});
        }
				node = this.owner[a] ? this.owner[a].container : g;
        this.advanceX += node.getBBox().width + 60;
      }
      delete this.fields;
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.bar = $C({element : "rect", width : 0, height : this.height, fill : this.fill, appendTo : this.container});
      if (this.fields) this.addFields();

      return this;
    }
  });


  extend(pergola.Tab.prototype, {
    'font-size' : pAttributes.tab['font-size'],
    width : pAttributes.tab.width,
    height : pAttributes.tab.height,
    fill : pAttributes.tab.fill,
    'fill-opacity' : pAttributes.tab['fill-opacity'],
    hasVisualEffects : true,
    active : false,

  rollover : function (evt) {
    this.button.setAttributeNS(null, "fill-opacity", evt.type == "mouseover" || this['fill-opacity']);
  },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      if (pergola.Panel.checkPreemptive(null, true)) return;

      var tgt = evt.target,
      		eType = evt.type,
					owner = this.owner,
      		tabs = owner.tabs,
					button;

      if (!this.active) this.rollover(evt);
			if (eType == "mouseup") {
        for (var t in owner.tabs) {
        	if (owner[t] == this) continue;
        	button = owner[t].button;
					button.setAttributeNS(null, "fill-opacity", this['fill-opacity']);
					owner[t].active = false;
				}
        this.button.setAttributeNS(null, "fill-opacity", 1);
        this.parent.appendChild(this.container);
        this.active = true;
        owner.comboBox = this.comboBox;
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert(pergola.messages.missingOwner + "Tab");
        return;
      }
      var owner = this.owner,
          fillOpacity = +this.active || this['fill-opacity'],
          r = 4,
          slant = 4,
          w = this.width - r * 2 - slant * 2,
          h = this.height - r,
          anchorX = (r / (h / r)).trim(3),
          dc = ("M" + this.x + ",0v-.5h.5l" + slant + "-" + (h - .5) + "c" + (anchorX / 2) + ",-" + (r / 2) + " " + anchorX + ",-" + r + "," + (r + anchorX) + ",-" + r + "h" + w + "q" + r + ",0," + r + "," + r + "v" + h);

      this.container = $C({element : "g", id : this.id, transform : "translate(" + owner.margin + " " + this.y + ")", appendTo : this.parent});

      this.pane = {
				width : owner.width - owner.margin * 2  + 2,
				height : owner.height - owner.topBar.height - owner.footerHeight - 12,
				container : $C({element : "g", id : this.id + "_pane", appendTo : this.container})
			};
			$C({element : "rect", x : -.5, y : -.5, width : this.pane.width, height : this.pane.height, fill : "white", stroke : "#A0A0A0", appendTo : this.pane.container});

			this.button = $C({element : "path", d : dc, fill : this.fill, stroke : "#A0A0A0", "fill-opacity" : fillOpacity, appendTo : this.container});
      if (this.hasVisualEffects) this.registerEvents(this.button, ["mouseover", "mouseout"]);
      this.registerEvents(this.button, "mouseup");
      this.text = $C({element : "text", x : this.x + this.width / 2, y : -this.height / 2 + 3, 'font-size' : this['font-size'], 'text-anchor' : "middle", 'pointer-events' : "none", textNode : this.title, appendTo : this.container});

      if (this.layout) {
        var layout = this.layout;

        this.layout = new pergola.Layout();
        pergola.extend(this.layout, layout);
        this.layout.build({
          owner : this,
          parent : this.pane.container,
          x : layout.x,
          y : layout.y,
          width : layout.width || this.pane.width,
          height : layout.height || this.pane.height
        });
      }

      if (this.contains) {
        if (typeof this.contains == "function") this.contains();
        else this.pane.appendChild(this.contains);
      }

      this.sweep(['string']);

      return this;
    }
  });


  extend(pergola.TabsDock.prototype, {
	  reorganize : function () {
	    var tabW = pergola.presentationAttributes.windowTab.width,
	        totalW = (tabW + 1) * pergola.Window.tabs.length,
	        maxW = (pergola.width() - 200),
	        nodes = this.container.childNodes;
	
	    maxW -= maxW % (tabW + 1);
	    if (totalW > maxW) totalW = maxW;
	    for (var i = 0; i < nodes.length; i++) {
	      if (nodes.item(i).nodeType == 1) nodes.item(i).setAttributeNS(null, "transform", "translate(" + (i * (tabW + 1)) + " 0)");
	    }
	  },

    build : function () {
      pergola.extend(this, arguments[0]);
      this.closedGroup = $C({element : "g", id : this.id + "_closedGroup", transform : "translate(" + this.x + " " + this.y + ")", display :  "none", appendTo : this.parent});
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});

      return this;
    }
  });


  extend(pergola.Taskbar.prototype, {
    width : pAttributes.taskBar.width,
    height : pAttributes.taskBar.height,
    position : pAttributes.taskBar.position,
    hasMenu : pAttributes.taskBar.hasMenu,
    fill : pAttributes.taskBar.fill,
    maskFill : pAttributes.taskBar.maskFill,
    display : "none",
    toggleOn : function () {
      var tabs = pergola.Window.tabs;
      for (var a in tabs) {
        tabs[a].mask.setAttributeNS(null, "fill", "none");
      }
      this.display = "block";
      this.toggle();
      this.container.appendChild(pergola.tabsDock.container);
    },
    toggleOff : function () {
      var tabs = pergola.Window.tabs;
      for (var a in tabs) {
        var t = tabs[a]; 
        t.mask.setAttributeNS(null, "fill", t.maskFill);
      }
      this.display = "none";
      pergola.systemComponents.appendChild(pergola.tabsDock.container);
      this.toggle();
    },
    toggle : function () {
      this.container.setAttributeNS(null, "display", this.display);
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (this.position == "top") var y = 0;
      else if (this.position == "bottom") var y = pergola.height() - this.height;
      else {
        alert(pergola.messages.wrongTaskBarPosition + this.position);
        return;
      }
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + y + ")", display : this.display, appendTo : this.parent});
      this.mask = $C({element : "rect", width : this.width, height : this.height, fill : this.maskFill, appendTo : this.container});
      this.rect = $C({element : "rect", width : this.width, height : this.height, fill : this.fill, appendTo : this.container});

      return this;
    }
  });


  extend(pergola.ToolBar.prototype, {
    fill : pAttributes.toolBar.fill,
    width : 0,
    rx : pAttributes.toolBar.rx,
    height : pAttributes.toolBar.height,
//    advanceX : function () {return this.container.getBBox()},           // implementations return different values (w/wout stroke-width)
    advanceX : 6,
    spacing : pAttributes.toolBar.spacing,
    hasDecoration : pAttributes.toolBar.hasDecoration,
    decor : pAttributes.toolBar.decoration,

    build : function () {
      pergola.extend(this, arguments[0]);
      this.container = $C({element : "g", id : this.id, transform : "translate(" + this.x + " " + this.y + ")", appendTo : this.parent});
      this.bar = $C({element : "rect", rx : this.rx, width : this.width, height : this.height, fill : this.fill, appendTo : this.container});
      if (this.hasDecoration) {
        this.decor.height = this.height - 8;
        this.decoration = $C({element : "rect", x : 3.5, y : 2.5, rx : this.decor.rx, width : this.width, height : this.decor.height, fill : this.decor.fill, stroke : this.decor.stroke, appendTo : this.container});
      }
      this.toolGroups = $C({element : "g", appendTo : this.container});

      return this;
    }
  });


  extend(pergola.ToolButton.prototype, {
    fill : pAttributes.toolButton.fill,
    stroke : pAttributes.toolButton.stroke,
    maskFill : pAttributes.toolButton.maskFill,
    rx : pAttributes.toolButton.rx,
    selected : false,
    suspended : false,
    exclusive : false,
    type : "regular",
    cursor : "default"
  });


  extend(pergola.TopBar.prototype, {
    height : pAttributes.topBar.height.panel,
    fill : pAttributes.topBar.fill.panel,
    textFill : pAttributes.text.fill,

    preemptiveWarning : function (timer) {
    	var fill = timer.repeatCount.isEven() ? "#E6E6D4" : this.topBar.fill;

    	if (timer.repeatCount++ == timer.repeats) {
				timer.clear();
				return;
			}
			this.topBar.bar.setAttributeNS(null, "fill", fill);
		},

    handleEvent : function (evt) {
      var owner = this.owner;

      if (pergola.Key.active && owner.isTop) pergola.Key.active.stop = false;
			if (owner.isFull) return;

    	pergola.dragarea.activate({
        callback : owner.move,
        handle : owner,
        target : owner.container,
        offset : {
  				x : evt.clientX - owner.x,
  				y : evt.clientY - owner.y
  			}
      });
		},

    build : function () {
      pergola.extend(this, arguments[0]);
      var owner = this.owner,
					attributes,
					x,
					y,
					h,
					weight,
					rx;

      if (!owner) {
        alert (pergola.messages.missingOwner + "TopBar");
        return;
      };

      if (owner.layout) this.height += (pAttributes.tab.height - .5) * !!owner.layout.tabs;
      if (owner.type == "messageBox") {
				this.fill = pAttributes.topBar.fill.messageBox;
				this.stroke = "#000000";
			}

      attributes = {
        fill : this.fill,
        stroke : this.stroke
      };
      if (this.extra) pergola.extend(attributes, this.extra);

      x = this.setPositionToStrokeWidth(attributes['stroke-width'] || 1);
      h = this.height;
      rx = owner.frame.rx;

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      attributes.d = "M" + x + "," + (h + x) + "v-" + (h - rx) + "q0,-" + rx + "," + rx + ",-" + rx + "h" + (owner.width - (rx * 2)) + "q" + rx + ",0," + rx + "," + rx + "v" + (h - rx) + "z";
      this.bar = $C(pergola.extend({element : 'path', appendTo : this.container}, attributes));

      y = parseInt(this['font-size']) + 11;
      weight = "normal";

      if (owner.layout) {
        if (owner.layout.type == "message") {
          this.textFill = "white";
          weight = "bold";
          y += 1;
        }
      }
      this.title = $C({element : "text", x : owner.width / 2, y : y, fill : this.textFill, 'font-weight' : weight, 'letter-spacing' : 1, kerning : 0, 'text-anchor' : "middle", 'pointer-events' : "none", textNode : owner.title, appendTo : this.container});
      this.registerEvents(this.bar, "mousedown");
      if (this.extra) delete this.extra;

      return this;
    }
  });


  extend(pergola.Window, {
    windows : [],
    tabs : [],
    untitled : 1,
    unnamed : function () {
      return "Untitled_" + pergola.Window.untitled++;
    },
    active : function () {return pergola.Window.windows[pergola.Window.windows.length - 1];},
    exist : function (name) {
      for (var i in pergola.Window.windows) {
        if (name == pergola.Window.windows[i].name) return true;
      }
      return false;
    }
  });


  extend(pergola.Window.prototype, {
    isOpen : true,
    minimized : false,
    isFull : false,
    resizable : true,
    fill : pAttributes.window.fill,
    scrollSize : pAttributes.window.scrollSize,
    x : 200,
    y : 160,
    margin : 4,
    width : 680,
    height : 520,
    minWidth : 340,
    minHeight : 200,
    hasCommands : true,
    hasToolBar : true,
    hasZoomAndPan : true,
    hasStatus : true,
    hasUndo : false,
    mousewheel : true,
    docBgResizable : true,
    fullW : function () {return pergola.width() + 1;},
    fullH : function () {return pergola.height() - pergola.taskbar.height + 1;},
    type : "regular",
    scrollType : "regular",
    dependencies : false,

    move : pergola.Window.superClass.moveXY,

    closeGroupPos : function () {
			return (this.width - this.margin - (pergola.CommandButton.prototype.width + 6) * 3) + " " + (this.margin + 2);
		},

    bringToFront : function () {
    	if (this.isTop) return;

      if (pergola.Key.active) pergola.Key.active.release();
			pergola.user.appendChild(this.container);
      for (var a in pergola.Window.windows) {
        if (pergola.Window.windows[a] == this) {
          pergola.Window.windows.splice(a, 1);
          break;
        }
      }
      for (var a in pergola.Window.windows) {
        var win = pergola.Window.windows[a];
				if (win.isTop) {
	        win.isTop = false;
	        win.topBar.mask.setAttributeNS(null, "opacity", pergola.themeLuminosity / 25.6);
	        win.topBar.title.setAttributeNS(null, "opacity", .5);
	        win.removePointerEvents([win.doc.container]);
	        win.background.cursor = "default";
	        win.background.rect.setAttributeNS(null, "cursor", "default");
          win.doc.container.setAttributeNS(null, "cursor", "default");
          if (win.undoManager) win.undoManager.disable();
	        for (var t in win.exclusiveTools) {
	          if (win.exclusiveTools[t].selected) {
	            win.exclusiveTools[t].suspended = true;
	            break;
	          }
	        }
          break;
				}
      }
      pergola.Window.windows.push(this);
      this.isTop = true;
      for (var t in this.exclusiveTools) {
        if (this.exclusiveTools[t].selected) {
          this.exclusiveTools[t].suspended = false;
          this.background.cursor = this.exclusiveTools[t].cursor;
          break;
        }
      }

      this.setPointerEvents([this.doc.container]);
      this.background.rect.setAttributeNS(null, "cursor", this.background.cursor);
      this.doc.container.setAttributeNS(null, "cursor", this.background.cursor);
      if (this.undoManager) this.undoManager.register();

      this.topBar.mask.setAttributeNS(null, "opacity", 0);
      this.topBar.title.setAttributeNS(null, "opacity", 1);
      if (this.dependencies) this.updateDependencies();
    },

    getCandidate : function () {
      var windows = pergola.Window.windows,
					w;

	    for (var i = windows.length - 1; i >= 0; i--) {
	      w = windows[i];
	      if (w == this) continue;
	      if (!w.minimized && w.isOpen) {
	      	w.bringToFront();
	        break;
	      }
	    }
    },

    release : function () {
      if (pergola.Key.active) pergola.Key.active.stop = true;
      if (this.type == "map") {
        polymaps.origin.x = this.x + this.doc.x;
        polymaps.origin.y = this.y + this.doc.y;
        this.centerMap(this.map.center());
      }
      else if (this.type == "editor") {
        this.yardstick.update();
        this.compass.update();
      }
    },

	  minimize : function () {
	    this.container.setAttributeNS(null, "display", "none");
	    this.minimized = true;
	    this.preview.hide();
	    this.getCandidate();
	  },

	  restore : function (evt) {
	     this.container.setAttributeNS(null, "display", "block");
	     this.preview.hide();
	     this.minimized = false;
	     this.bringToFront();
	  },

	  maximizeInit : function (evt) {
	      if (this.isFull) this.reduce();
	      else this.maximize();
	      this.doc.transformUpdate();
	      if (this.type == "editor") {
	        this.yardstick.update();
	        this.compass.update();
	      }
	  },

	  maximize : function () {
	    this.reducedX = this.x;
	    this.reducedY = this.y;
	    this.reducedWidth = this.width;
	    this.reducedHeight = this.height;
	    this.x = 0;
	    this.y = pergola.taskbar.height;
	    this.width = this.fullW();
	    this.height = this.fullH();
	    this.setXYWH(this.container);
	    this.doc.transformUpdate();
	    this.release();
	    this.isFull = true;
	  },

	  reduce : function () {
	    this.x = this.reducedX;
	    this.y = this.reducedY;
	    this.width = this.reducedWidth;
	    this.height = this.reducedHeight;
	    this.setXYWH(this.container);
	    this.doc.transformUpdate();
	    this.release();
	    this.isFull = false;
	  },

	  resizeInit : function (evt) {
	  	if (this.isFull) return;
	
	  	if (pergola.Key.active) pergola.Key.active.stop = false;
	    
	    var point = pergola.mousePoint(evt);
	
	    if (this.type == "editor") {
	      this.yardstick.container.setAttributeNS(null, "display", "none");
	      this.compass.container.setAttributeNS(null, "display", "none");
	    }
	
	    return {
	      callback : this.resizeSE,
	      handle : this,
	      target : this,
	      offset : {
					x : this.x + this.width - point.x,
					y : this.y + this.height - point.y
				},
	      updateCoordinates : false,
	      cursor : "se-resize"
	    }
	  },

	  close : function (evt) {
			if (this.type == "editor") {
				if (confirm(pergola.messages.closeConfirm)) this.terminate();
				return;
			}

      this.container.setAttributeNS(null, "display", "none");
      this.isOpen = false;
      this.tab.close();

      this.getCandidate();
	  },

	  terminate : function () {
			var windows = pergola.Window.windows;

	   	this.getCandidate();
	    this.tab.remove();
			pergola.user.removeChild(this.container);
			for (var i in windows) {
				if (this == windows[i]) windows.splice(i, 1);
			}

			if (this.undoManager) this.undoManager.disable();

      for (var t in this.exclusiveTools) {
        if (this.exclusiveTools[t].selected) {
        	this.releaseTool(this.exclusiveTools[t]);
          break;
        }
      }

	    pergola.xmlName.remove(this.name);
			pergola.destroy(this.owner, this.name);
	  },

	  reopen : function () {
	    this.isOpen = true;
	    pergola.Window.tabs.push(this.tab);
	    pergola.user.appendChild(this.container);
	    this.container.setAttributeNS(null, "display", "block");
	    this.tab.open();
	  },

    centerMap : function (c) {
      this.map.center(c);
      if (this.status) this.status.center(c);
    },

    getLocation : function (evt) {
      var win = pergola.Window.active(),
          doc = win.doc,
					origin = polymaps.origin,
					point = pergola.mousePoint(evt),
          geoPoint = win.map.pointLocation({x : point.x - (doc.ctm.e + origin.x), y : point.y - (doc.ctm.f + origin.y)});

      if (win.status) win.status.location(geoPoint);
    },

    setLocation : function () {
    	var c = {lat : this.locLat.value, lon : this.locLon.value};
      this.map.center(c);
      if (this.status) this.status.center(c);
    },

    lonToRange : function (lon) {
    	if (lon < -180) lon += 360;
      else if (lon > 180) lon -= 360;
      return lon;
    },

    mapZoom : function (z) {
      this.map.zoom(z);
      this.mapZoomLevelMark(z);
      if (this.status) this.status.zoom(z);
    },

    showMapFeatureLayer : function (o) {
      o.display = o.display == "block" ? "none" : "block";
      o.container.setAttributeNS(null, "display", o.display);
    },

    mapZoomLevelMark : function (z) {
      if (!this.menu.zoomLevel) return;
      z = z || this.map.zoom();
      var list = this.menu.zoomLevel.list;
      for (var p in list) {
        var i = list[p];
        if (i.check) {  
          i.check = false;
          i.checkmark.setAttributeNS(null, "display", "none");  
        }
        if (i.string == z) {
          i.checkmark.setAttributeNS(null, "display", "block");
          i.check = true;
        }
      }
    },

    mapViewsToggle : function (view) {
      var menu = this.menu.views.list,
					i, is;

      for (var a in menu.items) {
        i = menu[a],
        is = (i.view == view);
        if (is) {
          i.fn();
          i.check = true;
          i.checkmark.setAttributeNS(null, "display", "block");
        }
        else {
          i.check = false;
          i.checkmark.setAttributeNS(null, "display", "none");
        }
      }

    },

	  mapGutterPositionOnScroll : function () {
	    this.map.lonG.setAttributeNS(null, "transform", "translate(0 " + (-this.doc.ctm.f) + ")");
	    this.map.latG.setAttributeNS(null, "transform", "translate(" + (-this.doc.ctm.e) + " 0)");
	  },

    mapMaker : function () {
      var doc = this.doc;
      polymaps.origin = {x : this.x + doc.x, y : this.y + doc.y};

      this.map = polymaps.map(doc)
      .container(doc.transformable.appendChild($C({element : "svg", id : this.id + "_tiles", width : this.canvas.width, height : this.canvas.height})));
      this.interact = polymaps.interact();
      this.map.add(this.interact);

// force zoom level checkmark in menu "Levels"
      if (this.menu.zoomLevel) this.mapZoomLevelMark();

      this.currentView = this.views[this.view];

      this.status.center(this.map.center());
      this.status.zoom(this.map.zoom());

// @param {String} "Aerial"; "AerialWithLabels"; "Road". Default: "AerialWithLabels".
      queryBing(this.view);
    },

    addTools : function () {
      var args = arguments[0],
          arg,
          barH = this.toolBar.height - 1,
          buttonH = this.toolBar.hasDecoration ? this.toolBar.decor.height - 7 : barH - 6,
          buttonW = this.toolBar.hasDecoration ? this.toolBar.decor.height - 4 : barH,
          spacing = this.toolBar.spacing,
          adv = buttonW + spacing,
          advX = 0,
          rx = pAttributes.toolButton.rx,
          tool;

      for (var a in args) {
        arg = args[a];
        if (arg.separator) {
          var sep = new pergola.Separator()
          .build({
            parent : this.toolBar.toolGroups,
            x : this.toolBar.advanceX,
            y : (barH - buttonH) / 2,
            height : buttonH,
            type : "vertical"
          });
          this.toolBar.advanceX += sep.width;
          delete arg.separator;
        }
        this.toolBar[a] = $C({element : "g", transform : "translate(" + this.toolBar.advanceX + " " + ((barH - buttonH) / 2) + ")", appendTo : this.toolBar.toolGroups});
        for (var b in arg) {
          tool = arg[b];
          this[b] = new pergola.ToolButton(b);
          pergola.extend(tool, {
            owner : this,
            parent : this.toolBar[a],
            x : advX,
            width : tool.width || buttonW,
            height : buttonH,
            extra : {
              rx : rx
            }
          });
          this[b].build(tool);
          advX += (tool.width + spacing) || adv;
          if (tool.exclusive) this.exclusiveTools.push(this[b]);
          this.tools.push(this[b]);
        }
        this.toolBar.advanceX += advX - spacing;
      }
    },

    addMenu : function (o) {
    	if (!this.menu) {
	    	this.menu = {};
	    	this.menubar = new pergola.Menubar(this.id + "_menuBar");
	      this.menubar.build({
	        owner : this,
	        parent : this.commands,
	        y : this.topBar.height - this.menubar.height - 1,
	        fill : "none"
	      });
	    }

    	for (var p in o) {
				this.menu[p] = new pergola.Menu(p).build(pergola.extend(o[p], {owner : this, parent : this.menubar.container}));
			}
    },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var tgt = evt.target,
          eType = evt.type,
          comms = this.hasCommands,
					tool;

      if (eType == "mousedown") {
      	if (pergola.Panel.checkPreemptive(null, true)) return;
				if (pergola.Menu.active) pergola.Menu.handle.close(evt);
				this.bringToFront();
			}

      if (tgt == this.background.rect) this.toolHandler(evt);

      for (var t in this.tools) {
      	tool = this.tools[t];
        if (tgt == tool.button) {
        	if (tool.exclusive) {        	        	
	          this.toolInit(evt, tool);
	          break;
          }          
          else {
						if (this[tool.initFn]) {
							this[tool.initFn](evt);
	          	break;
	          }
					}          
        }
      }
    },

    build : function () {
      pergola.extend(this, arguments[0]);

      pergola.Window.windows.push(this);

      this.tools = [];
      this.exclusiveTools = [];
      if (this.hasUndo) this.undoManager = new pergola.Undo();
      this.title = this.name;
      if (this.title.indexOf("Untitled") != -1) {
				this.title = this.title.replace("_", "-");
        pergola.Window.untitled++;
			}

      if (!this.hasToolBar && this.hasZoomAndPan) this.hasToolBar = true;

      if (this.type == "editor") {
        this.minWidth = 656;
        this.minHeight = 200;
      }

      if (this.type == "map") {
        var scrollType = "navigation",
            lensQTip = "mapLensTool",
            handQTip = "mapHandTool",
            msg = pergola.messages,
            helpItems = {
							"Keyboard" : msg.kbdNavHelp,
							"Mouse" : msg.mouseNavHelp,
							"Scrollbars" : msg.scrollHelp,
							"Lens tool" : msg.lensToolHelp,
							"Hand tool" : msg.handToolHelp,
							"Minus tool" : msg.minusToolHelp,
							"Plus tool" : msg.plusToolHelp,
							"Itinerary tool" : msg.itineraryToolHelp,
							"Measure tool" : msg.measureToolHelp
						},
						text = $C({element : "text", y : 28, "font-family" :"'Trebuchet MS'", "font-size" : "7.5pt", appendTo : pergola.user}),
						title = $C({element : "tspan", x : 10, y : 20, "font-family" :"'Trebuchet MS'", "font-size" : "11px", "font-weight" : "bold", "letter-spacing" : "1pt", kerning : 0, textNode : "NAVIGATION HELP", appendTo : text});

				for (var p in helpItems) {
					$C({element : "tspan", x : 10, dy : 18, "font-weight" : "bold", fill : "#313133", textNode : p, appendTo : text});
        	for (var i in helpItems[p]) $C({element : "tspan", x : 16, dy : 12, textNode : helpItems[p][i], appendTo : text});
				}
				var bbox = text.getBBox();

				this.navHelpPanel = new pergola.Panel();
        this.navHelpPanel.build({
          type : "basic",
          title : "NAVIGATION HELP",
          filter : "none",
          fill : "#FFFFFE",
          x : 1,
          y : 1,
          width : bbox.width + 18,
          height : bbox.height + 20,
          contains : text
        });
      }

      else {
        var scrollType = "regular",
            lensQTip = "lensTool";
            handQTip = "handTool";
      }

      this.container = $C({element : "svg", id : this.id, x : this.x, y : this.y, width : this.width, height : this.height, appendTo : pergola.user});
      this.registerEvents(this.container, "mousedown");

      this.doc = new pergola.Doc(this.id + "_doc");
      this.frame = new pergola.Frame(this.id + "_frame")
      .build({
        owner : this,
        parent : this.container
      });

      if (this.hasStatus) {
        var fields;
        if (this.type == "map") {
          fields = {
            centerLon : {
              caption : "Lon:"
            },
            centerLat : {
              caption : "Lat:"
            },
            zoomLevel : {
              input : {
              	owner : this,
	              type : pergola.keypressPosInt, 
								x : 200,
		            y : 2,
		            width : 20,
								height : 15,        
	              "font-size" : 10,
	              hasButtons : false,
	              caption : {
		              x : -2,
		              "font-size" : 10,
		              textNode : "Level:",
		            },
	              max : 21,
	            	min : 1,
							  maxLength : 2,
							  value : 1,
							  message : pergola.messages.mapLevelRange,
		            fn : "mapZoom"
							}
            },
            locLon : {
              input : {
              	owner : this,
	              type : pergola.keypressNum, 
								x : 300,
		            y : 2,
		            width : 48,
		            height : 15,
	              "font-size" : 10,
	              hasButtons : false,
	              caption : {
		              x : -2,
		              "font-size" : 10,
		              textNode : "Location Lon:",
		            },
		            appendix : {
		            	x : -5,
		              "font-size" : 10,
		              textNode : "\u00B0"
		            },              
	              max : 180,
	            	min : -180,
							  maxLength : 8,
							  value : 0.0000,
		            target : this.status,
		            propName : "locLon",
		            fn : "setLocation"
							}
            },
            locLat : {
              input : {
              	owner : this,
	              type : pergola.keypressNum, 
								x : 420,
		            y : 2,
		            width : 44,
								height : 15,             
	              "font-size" : 10,
	              hasButtons : false,
	              caption : {
		              x : -2,
		              "font-size" : 10,
		              textNode : "Location Lat:",
		            },
		            appendix : {
		            	x : -5,
		              "font-size" : 10,
		              textNode : "\u00B0"
		            },              
	              max : 180,
	            	min : -180,
							  maxLength : 8,
							  value : 0.0000,
		            target : this.status,
		            propName : "locLat",
		            fn : "setLocation"
							}
            }
          };
        }
        else {
          fields = {
            zoomLevel : {
              input : {
	              type : pergola.keypressNum, 
								x : 52,
		            y : 2,
		            width : 36,
								height : 15,    
	              "font-size" : 10,
	              hasButtons : false,
	              caption : {
		              x : -2,
		              "font-size" : 10,
		              textNode : "Zoom:",
		            },
		            appendix : {
		            	x : -5,
		              "font-size" : 10,
		              textNode : "%"
		            },              
	              max : this.doc.maxScale,
	            	min : this.doc.minScale,
							  maxLength : 6,
							  value : this.doc.scaleFactor * 100,
		            target : this.doc,
		            propName : "scaleFactor",
		            fn : "zoomFromValue"
							}
            },
            coorX : {
              caption : "x :",
              value : "0"
            },
            coorY : {
              caption : "y :",
              value : "0"
            }
          };
        }
        this.status = new pergola.StatusBar(this.id + "_statusBar")
        .build({
          owner : this,
          parent : this.container,
          fields : fields
        });
      }

      this.commands = $C({element : "g", id : this.id + "_commands", transform : "translate(" + this.margin + " " + this.margin + ")", appendTo : this.container});
      this.topBar = new pergola.WindowTopBar(this.id + "_topBar")
      .build({
        owner : this,
        parent : this.commands
      });

      this.progress = new pergola.Progress(this.id + "progress")
      .build({
        owner : this,
        parent : this.commands,
        y : this.topBar.height - 1,
        width : this.topBar.width - 2,
        height : 1.5,
        fill : "none",
        stroke : "none"
      });

      if (this.hasCommands) {
        this.closeGroup = $C({element : "g", id : this.id + "_closeGroup", transform : "translate(" + this.closeGroupPos() + ")", appendTo : this.topBar.container});
        this.minimizeBtn = new pergola.CommandButton(this.id + "_minimizeButton")
        .build({
          owner : this,
          parent : this.closeGroup,
          symbol : {
            symbol : pergola.symbols.winMinimize,
            x : 3.5,
            y : 9.5
          },
          ev : "mouseup",
          fn : function (evt) {
          	if (pergola.Panel.checkPreemptive(this.owner, true)) return;
          	if (pergola.Key.active) {
              pergola.Key.active.warn = false;            
              pergola.Key.active.release();
            }
            this.owner.minimize();
          }
        });
        this.full = new pergola.CommandButton(this.id + "_full");
        this.full.build({
          owner : this,
          parent : this.closeGroup,
          x : this.full.width + 4,
          symbol : {
            symbol : pergola.symbols.winFull,
            x : 4,
            y : 3
          },
          ev : "mouseup",
          fn : function (evt) {
          	if (pergola.Panel.checkPreemptive(this.owner, true)) return;
            this.owner.maximizeInit(evt);
          }
        });
        this.closeBtn = new pergola.CommandButton(this.id + "_closeButton");
        this.closeBtn.build({
          owner : this,
          parent : this.closeGroup,
          x : this.closeBtn.width * 2 + 8,
          symbol : {
            symbol : pergola.symbols.winClose,
            x : 5,
            y : 4
          },
          ev : "mouseup",
          fn : function (evt) {
          	if (pergola.Panel.checkPreemptive(this.owner, true)) return;
          	if (pergola.Key.active) {
              pergola.Key.active.warn = false;            
              pergola.Key.active.release();
            }
            this.owner.close(evt);
          }
        });
      }

      if (this.hasToolBar) {
        this.toolBar = new pergola.ToolBar(this.id + "_toolBar")
        .build({
          owner : this,
          parent : this.commands,
          y : this.topBar.height
        });
      }

      this.paneY = this.margin + this.topBar.height + (this.hasToolBar ? this.toolBar.height : 0);

      if (this.menu) {
        this.menubar = new pergola.Menubar(this.id + "_menuBar");
        this.menubar.build({
          owner : this,
          parent : this.commands,
          y : this.topBar.height - this.menubar.height - 1,
          fill : "none"
        });
        for (var p in this.menu) {
          var props = this.menu[p];
          props.owner = this;
          props.parent = this.menubar.container;
          this.menu[p] = new pergola.Menu(this.id + props.title)
          .build(props);
        }
      }

      if (this.hasZoomAndPan) {
        this.addTools({
          zoomGroup : {
            lensTool : {
              type : "zoom&pan",
              actOnCanvas : true,
              symbol : {
                symbol : pergola.symbols.lens,
                x : 12,
                y : 10
              },
              exclusive : true,
              quickTip : lensQTip,
              cursor : "url(#plusCursor)",
              altCursor : "url(#minusCursor)",
              fn : "zoomBylensToolInit"
            },
            zoomDown : {
              symbol : {
                symbol : pergola.symbols.zoomMinus,
                x : 8,
                y : 11
              },
              quickTip : "zoomdown",
              initFn : "zoomDownByButtonInit"
            },
            zoomUp : {
              symbol : {
                symbol : pergola.symbols.zoomPlus,
                x : 8,
                y : 11
              },
              quickTip : "zoomup",
              initFn : "zoomUpByButtonInit"
            },
            handTool : {
              type : "zoom&pan",
              actOnCanvas : true,
              symbol : {
                symbol : pergola.symbols.hand,
                x : 8,
                y : 7
              },
              exclusive : true,
              quickTip : handQTip,
              cursor : "move",
              fn : "scrollByhandToolInit"
            }
          }
        });
        if (this.type == "map") {
          this.addTools({
            rulerGroup : {
              separator : true,
              itineraryTool : {
              	type : "measure",
              	actOnCanvas : true,
              	location : true,
                symbol : {
                  symbol : pergola.symbols.itinerary,
                  x : 4.5,
                  y : 4
                },
                exclusive : true,
                quickTip : "itineraryTool",
                cursor : "crosshair",
                fn : "measureItinerary"
              },
              measureTool : {
              	type : "measure",
              	actOnCanvas : true,
              	location : true,
                symbol : {
                  symbol : pergola.symbols.measure,
                  x : 4.5,
                  y : 4
                },
                exclusive : true,
                quickTip : "measureTool",
                cursor : "crosshair",
                fn : "measureByRulerTool"
              }
            }
          });
          this.addTools({
            helpGroup : {
              separator : true,
              mapHelp : {
                symbol : {
                  symbol : pergola.symbols.question,
                  x : 9.75,
                  y : 18
                },
                quickTip : "mapHelp",
                initFn : "mapHelpInit"
              }
            }
          });
        }

      }

      this.background = new pergola.Background(this.id + "_background")
      .build({
        parent : this.container,  
        x : this.margin + .5,
        y : this.paneY + .5,
        width : 0,
        height : 0, 
        fill : this.fill,
        stroke : "#A0A0A0"
      });

      this.doc.build({
        owner : this,
        parent : this.container,
        x : this.margin + 1,
        y : this.paneY + 1,
        docBgResizable : this.docBgResizable
      });

      if (this.type == "map") this.doc.container.appendChild(this.navHelpPanel.container);

      this.hScrollBar = new pergola.Scrollbar(this.id + "_hScrollBar")
      .buildH({
        owner : this,
        parent : this.container,
        scrollTarget : this.doc,
        scrollType : scrollType,
        size : this.scrollSize,
        symbol : pergola.symbols.arrow.small
      });

      this.vScrollBar = new pergola.Scrollbar(this.id + "_vScrollBar")
      .buildV({
        owner : this,
        parent : this.container,
        scrollTarget : this.doc,
        scrollType : scrollType,
        size : this.scrollSize,
        symbol : pergola.symbols.arrow.small
      });

      this.resizeBtn = new pergola.Button(this.id + "_resizeButton")
      .build({
        owner : this,
        parent : this.container,
        width : this.hScrollBar.size,
        height : this.hScrollBar.size,
        maskFill : "none",
        fill : pAttributes.scrollBar.fill,
        stroke : "none",
        'fill-opacity' : 1,
        hasVisualEffects : false,
        x : this.hScrollBar.size + this.margin + 1.5,
        y : this.hScrollBar.size + this.margin + 1.5,
        symbol : {
          symbol : pergola.symbols.resize,
          x : this.hScrollBar.size - 10,
          y : this.hScrollBar.size - 10
        },
        extra : {
					cursor : "se-resize"
        },
				ev : "mousedown",
        fn : function (evt) {
          pergola.dragarea.activate(this.owner.resizeInit(evt));
        }
      });

      this.container.appendChild(this.commands);

      if (this.hasCommands) {
        pergola.taskbar.toggleOn();
        this.tab = new pergola.WindowTab(this.id + "_tab")
        .build({
          owner : this,
          parent : pergola.tabsDock.container
        });
        this.preview = new pergola.Preview(this.id + "_preview")
        .build({
          owner : this,
          parent : this.tab.container,
          y : this.tab.height + 7.5
        });
      }

      if (this.isFull && this.type != "map") this.maximize();

      if (this.contains) {
        if (typeof this.contains == "function") this.contains();
        else this.doc.transformable.appendChild(this.contains);
      }

      this.doc.updateOnMutationEvent();

			this.bringToFront();
      if (this.isFull && this.type == "map") this.maximize();
      if (this.minimized) this.minimize();

      return this;
    }
  });


  extend(pergola.WindowTab.prototype, {
    'font-size' : pAttributes.windowTab['font-size'],
    height : pAttributes.taskBar.height - 6,
    width : pAttributes.windowTab.width,
    fill : pAttributes.windowTab.fill,
    maskFill : pAttributes.windowTab.maskFill,
    maskStroke : pAttributes.windowTab.maskStroke,
    textFill : pAttributes.text.fill,
    textFillInverse : pAttributes.text.fillInverse,
    hasTextEffect : false,

		setTitle : function () {
      var text = this.owner.title.truncate(this.width - 10, this['font-size']);
			this.text.firstChild.data = text;
		},

  	close : function () {
	    var tabs = pergola.Window.tabs;
	
	    for (var a in tabs) {
        if (tabs[a] == this.tab) {
          tabs.splice(a, 1);
          break;
        }
      }
      pergola.tabsDock.closedGroup.appendChild(this.container);
	    pergola.tabsDock.reorganize();
	  },

	  open : function () {
	    pergola.Window.tabs.push(this);
	    pergola.tabsDock.container.appendChild(this.container);
	    pergola.tabsDock.reorganize();
	  },

  	remove : function () {
	    var tabs = pergola.Window.tabs;
	
	    for (var a in tabs) {
	      if (tabs[a] == this) {
	        tabs.splice(a, 1);
	        break;
	      }
	    }
	    pergola.tabsDock.container.removeChild(this.container);
	    pergola.tabsDock.reorganize();
	  },

    handleEvent : function (evt) {
      this.eventBehavior(evt);
      var tgt = evt.target,
          eType = evt.type,
          owner = this.owner;

			if (evt.type == "mouseup") {
	      if (pergola.Panel.checkPreemptive(null, true)) return;
	      owner.preview.hide();
				if (pergola.Menu.active) pergola.Menu.handle.close(evt);
				if (owner.minimized) owner.restore(evt);
				else {
					if (owner.isTop) owner.minimize();
					else owner.bringToFront();
				}
	    }
	    else if (evt.type == "mouseover") {
	    	this.rollover(evt);
	      owner.doc.updatePreview();
        owner.preview.show();
	    }
	    else if (evt.type == "mouseout") {
				owner.preview.hide();
				this.rollover(evt);
			}
    },

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "WindowTab");
        return;
      };

      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      this.mask = $C({element : "rect", x : 2.5, y : 3.5, width : this.width - 2, height : this.height - 2, fill : "none", stroke : this.maskStroke, 'pointer-events' : "none", appendTo : this.container});
      this.button = $C({element : "rect", x : 1.5, y : 2.5, width : this.width, height : this.height, fill : this.fill, "fill-opacity" : 0, stroke : this.stroke, appendTo : this.container});
      this.registerEvents(this.button, ["mouseup", "mouseover", "mouseout"]);
      
      this.text = $C({element : "text", x : this.width / 2, y : 19, 'font-size' : this['font-size'], 'text-anchor' : 'middle', fill : this.textFill, 'pointer-events' : "none", textNode : " ", appendTo : this.container});
			this.setTitle();

			pergola.Window.tabs.push(this);
      pergola.tabsDock.reorganize();

      return this;
    }
  });


  extend(pergola.WindowTopBar.prototype, {
    height : pAttributes.topBar.height.window,
    fill : pAttributes.topBar.fill.window,
    maskFill : pAttributes.topBar.maskFill,
    maskOpacity : 0,

    build : function () {
      pergola.extend(this, arguments[0]);
      if (!this.owner) {
        alert (pergola.messages.missingOwner + "WindowTopBar");
        return;
      };

      this.width = this.owner.width - this.owner.margin * 2 + 2;
      this.container = $C({element : "g", id : this.id, appendTo : this.parent});
      this.bar = $C({element : "rect", x : this.x, y : this.y, width : this.width, height : this.height, fill : this.fill, appendTo : this.container});
      this.mask = $C({element : "rect", x : this.x, y : this.y, width : this.width, height : this.height, fill : this.maskFill, opacity : this.maskOpacity, "pointer-events" : "none", appendTo : this.container});
      this.title = $C({element : "text", x : this.owner.width / 2, y : this["font-size"] + 7, "font-size" : this["font-size"], 'letter-spacing' : 1, kerning : 0, fill : this.textFill, 'text-anchor' : "middle", 'pointer-events' : "none", textNode : this.owner.title, appendTo : this.container});
      this.registerEvents(this.bar, "mousedown");

      return this;
    }
  });

})();



(function () {
  var container = pergola.container,
      pAttributes = pergola.presentationAttributes;

  if (container) pergola.origin = {x : pergola.container.offsetLeft, y : pergola.container.offsetTop};
  pergola.doc.setAttributeNS(null, "font-family", pergola.presentationAttributes.all['font-family']);
  pergola.doc.setAttributeNS(null, "font-size", pergola.presentationAttributes.all['font-size']);

  pergola.symbols = pergola.symbols();
  pergola.cursors();

  pergola.desktop = $C({element : "g", id : "pergola_desktop", appendTo : pergola.doc});

  pergola.background = new pergola.Background("pergola_background")
  .build({parent : pergola.desktop});

  pergola.user = $C({element : "g", id : "pergola_userLayer", appendTo : pergola.desktop});

  pergola.Class.prototype.parent = pergola.user;

  pergola.systemComponents = $C({element : "g", id : "pergola_systemComponents", appendTo : pergola.desktop});

	if (pergola.debug) pergola.debugger();

  pergola.dialogs = $C({element : "g", id : "pergola_dialogs", appendTo : pergola.systemComponents});

  pergola.preemptive = $C({element : "g", id : "pergola_preemptive", appendTo : pergola.systemComponents});

  pergola.Panel.prototype.parent = pergola.dialogs;

  if (colorpicker) {
		if (colorpicker == "extended") {
			pergola.color.rgbTablePopulate.extended();
			pergola.colorpickerDialog = new pergola.Panel("pergola colorpicker dialog")
		  .build({
		    parent : pergola.preemptive,
		    title : "COLOR PICKER",
		    x : 2,
		    width : 1552,
		    height : 580,
		    okButton : {x : 40, y : 390},
		    cancelButton : {x : 40, y : 450},
		    layout : {type : "colorpickerExtended"},
		    preemptive : true
		  });
		}
		else {
			pergola.color.rgbTablePopulate.basic();
			pergola.colorpickerDialog = new pergola.Panel("pergola colorpicker dialog")
		  .build({
		    parent : pergola.preemptive,
		    title : "COLOR PICKER",
		    width : 500,
		    height : 412,
		    okButton : {},
		    cancelButton : {},
		    layout : {type : "colorpicker"},
		    preemptive : true
		  });
		}
	}
	

  pergola.notification = new pergola.Panel("pergola notification");
  pergola.notification.build({
    parent : pergola.preemptive,
    title : "MESSAGE",
    x : pAttributes.notification.x,
    y : pAttributes.notification.y,
    width : pAttributes.notification.width,
    height : pAttributes.notification.height,
    fill : pAttributes.notification.fill,
    cancelButton : {
      text : "Close"
    },
    layout : {
      type : "message"
    },
    preemptive : true
  });

  pergola.quicktips = {};
  var q = pergola.quicktips;
  q.container = $C({element : "g", id : "pergola_quickTips", appendTo : pergola.systemComponents});
  for (var t in pergola.quickTip) {
    q[t] = new pergola.QuickTip("quickTip_" + t)
    .build(pergola.quickTip[t]);
  }

  pergola.rubberBox = new pergola.RubberBox("pergola_rubberBox").build();

  pergola.coordinates = new pergola.Coordinates()
  .build({parent : pergola.systemComponents});

  pergola.dragarea = new pergola.Dragarea("pergola_dragarea")
  .build();

  pergola.taskbar = new pergola.Taskbar("pergola_taskbar").build({parent : pergola.systemComponents});

  pergola.tabsDock = new pergola.TabsDock("pergola_tabsDock")
  .build({
    parent : pergola.taskbar.container,
    x : 140 * pergola.taskbar.hasMenu
  });

})();









