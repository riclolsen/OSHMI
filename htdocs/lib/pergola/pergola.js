
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/


var pergola = {

  version : "2.0.91",

  ns : {
    svg : "http://www.w3.org/2000/svg",
    xlink : "http://www.w3.org/1999/xlink",
    xhtml: "http://www.w3.org/1999/xhtml",
    xml: "http://www.w3.org/XML/1998/namespace",
    xmlns: "http://www.w3.org/2000/xmlns/"
  },

	path : path,

  debug : debug,

	decimals : decimals,

  theme : theme,

  doc : document.documentElement,

  browser : {
    trident : /Trident/.test(navigator.userAgent) | 0,
    firefox : /Firefox/.test(navigator.userAgent) | 0,
    opera : /Opera/.test(navigator.userAgent) | 0,
    webkit : /WebKit/.test(navigator.userAgent) | 0,
    chrome : /Chrome/.test(navigator.userAgent) | 0,
    safari : /Safari/.test(navigator.userAgent) | 0,
  },

  xmlName : {

		list : [],

		exist : function (string) {
			var list = pergola.xmlName.list;
			for (var i in list) {
        if (string == list[i].substring(list[i].indexOf("XML"))) return true;
      }

      return false;
		},

		add : function (name) {
			pergola.xmlName.list.push(name);
			return name;
		},

		remove : function (name) {
			var list = pergola.xmlName.list;
			for (var i in list) {
				if (name == list[i]) {
					list.splice(i, 1);
					break;
				}
			}
		}

	},

  mutationEvnt : document.implementation.hasFeature('MutationEvents', '2.0') | !!window.MutationEvent,

  mutationObserver : window.MutationObserver || window.WebKitMutationObserver || window.MozMutationObserver,

  remote : ("" + window.location).indexOf("http") != -1,

  subclass : function (subClass, baseClass) {
    if (!subClass.length) subClass = [subClass];
    function inheritance () {}
    inheritance.prototype = baseClass.prototype;
    for (var c in subClass) {
      var clss = subClass[c];
      clss.prototype = new inheritance();
      clss.prototype.constructor = clss;
      clss.baseConstructor = baseClass;
      clss.superClass = baseClass.prototype;
    }
  },

  extend : function (entity, ext) {
    for (var prop in ext) entity[prop] = ext[prop];
    return entity;
  },

  destroy : function (handle, name) {
    if (handle[name]) delete handle[name];
    else if (name in window) {
      if (! delete window[name]) window[name] = null;
    }
  },

  SVG : function (o) {
  	var element,
  			value;

    for (var p in o) {
      value = o[p];
      switch(p) {
        case "element" : element = document.createElementNS(pergola.ns.svg, value);
        break;
        case "textNode" : element.appendChild(document.createTextNode(value));
        break;
        case "appendTo" : value.appendChild(element);
        break;
        default : element.setAttributeNS((p == "xlink:href") ? pergola.ns.xlink : null, p, value);
      }
    }
    return element;
  },

  HTML_FO : function (o) {
  	var element,
  			value;

    for (var p in o) {
      value = o[p];
      switch(p) {
        case "element" : element = document.createElementNS(pergola.ns.xhtml, value);
        break;
        case "textNode" : element.appendChild(document.createTextNode(value));
        break;
        case "appendTo" : value.appendChild(element);
        break;
        default : element.setAttribute(p, value);
      }
    }
    return element;
  },

  HTML : function (o) {
  	var element,
  			value;

    for (var p in o) {
      value = o[p];
      switch(p) {
        case "element" : element = document.createElement(o.element);
        break;
        case "textNode" : element.appendChild(document.createTextNode(value));
        break;
        case "appendTo" : value.appendChild(element);
        break;
        default : element.setAttribute(p, value);
      }
    }
    return element;
  },

  attribute : function (element, list, ns) {
    for (var a in list) {
    	switch(ns) {
    		case undefined : element.setAttributeNS(null, a, list[a]);
    		case false : element.setAttribute(a, list[a]);
    		default : element.setAttributeNS(ns, a, list[a]);
			}
		}
  },

  removeChildren : function (node) {
    while (node.firstChild) node.removeChild(node.firstChild);
  },

  width : function () {
    var container = pergola.container || pergola.doc;

    if (window.getComputedStyle) return parseInt(window.getComputedStyle(container, null).width);
    return window.innerWidth;
  },

  height : function () {
    var container = pergola.container || pergola.doc;

    if (window.getComputedStyle) return parseInt(window.getComputedStyle(container, null).height);
    return window.innerHeight;
  },
   

  mousePoint : function (evt) {
    if (pergola.container) {
      var doc = pergola.doc,
          point = doc.createSVGPoint();

      point.x = evt.clientX;
      point.y = evt.clientY;
      point = point.matrixTransform(doc.getScreenCTM().inverse());
      return {x : point.x, y : point.y};
    }

    return {x : evt.clientX, y : evt.clientY};
  },

  use : function (o) {
    if (! o.shape) {
      alert (pergola.messages.noSuchShape);
      return;
    }

		var element = pergola.extend({}, o.shape)
		element.appendTo = o.parent;
		return $C(pergola.extend(element, o.attributes));
  },

  symbol : function (o) {
    var element,
        bBox,
        x = o.x || 0,
        y = o.y || 0,
        op = o.opacity,
        parent = o.parent || this.container,
        g = $C({element : "g", 'pointer-events' : "none", appendTo : pergola.user});

    if (op) g.setAttributeNS(null, "opacity", op);

    if (typeof o.symbol == "string") {
      $C({element : "image", width : o.width, height : o.height, "xlink:href" : o.symbol, appendTo : g});
    }
    else {
      for (var a in o.symbol) {
        element = $C(o.symbol[a]);
        if (o.fill) element.setAttributeNS(null, "fill", o.fill);
        if (o.stroke) element.setAttributeNS(null, "stroke", o.stroke);
        if (o.scale) element.setAttributeNS(null, "transform", "scale(" + o.scale + ")");
        g.appendChild(element);
      }
    }

    bBox = g.getBBox();
    if (x == "center") x = Math.ceil((this.width - bBox.width) / 2);
    if (y == "center") y = Math.ceil((this.height - bBox.height) / 2);
    if (x || y) g.setAttributeNS(null, "transform", "translate(" + x + " " + y + ")");
    parent.appendChild(g);
    return g;
  },

  shapeToSymbol : function (shape) {
    if (! shape) {
      alert (pergola.messages.noSuchShape);
      return;
    }
    var symbol = [];
    symbol[0] = {element: shape.element};
    for (var p in shape.geometry) symbol[0][p] = shape.geometry[p];
    return symbol;
  },


  message : function (m, pos) {
    var o = pergola.notification,
        parent = o.messageGroup;

    m = m.split("\n");
    pergola.removeChildren(o.messageGroup);
    var y = 16;
    for (var e in m) {
      m[e] = m[e].truncate(o.width - 40, "10pt");
      $C({element : "text", y : e * 16, textNode : m[e], appendTo : parent});
    }
    if (pos) {
      o.x = pos.x;
      o.y = pos.y;
      o.container.setAttributeNS(null, "transform", "translate(" + o.x + " " + o.y + ")");
    }
    o.open();
  },


  resize : function () {
    var w = pergola.width(),
        h = pergola.height(),
        bg = pergola.background,
        d = pergola.dragarea;

    if (bg) {
      bg.rect.setAttributeNS(null, "width", w);
      bg.rect.setAttributeNS(null, "height", h);
      bg.image.setAttributeNS(null, "width", w);
      bg.image.setAttributeNS(null, "height", h);
    }
    d.rect.setAttributeNS(null, "width", w);
    d.rect.setAttributeNS(null, "height", h);
    for (var a in pergola.Window.windows) {
      var win = pergola.Window.windows[a];
      if (win.isFull) {
        win.width = win.fullW();
        win.height = win.fullH();
        win.setXYWH(win.container);
        win.doc.transformUpdate();
      }
    }
    if (pergola.taskbar) {
      pergola.taskbar.mask.setAttributeNS(null, "width", w);
      pergola.taskbar.rect.setAttributeNS(null, "width", w);
    }
  },




  Timer : function () {
    var self,
        handle;

    return {
      initialize : function () {
        pergola.extend(this, arguments[0]);
        var f = "frequence" in this,
            d = "delay" in this;

        self = this;
        handle = this.handle || this;
        if (handle != this) handle.callback = this.callback;

        if (f && d) this.timeout = window.setTimeout(this.setInterval, this.delay);
        else if (f) this.setInterval();
        else this.timeout = window.setTimeout(function () {handle.callback(self);}, this.delay);
        return this;
      },

      setInterval : function () {
        self.interval = window.setInterval(function () {handle.callback(self);}, self.frequence);
      },

      clear : function () {
        if (this.timeout) window.clearTimeout(this.timeout);
        if (this.interval) window.clearInterval(this.interval);
        this.timeout = this.interval = null;
        if (this.destroy) pergola.destroy(handle, this.destroy);
      }
    };
  },





  Key : function () {
    var self,
        handle,
        doc = document.documentElement;

    return {
      register : function () {
        if (pergola.Key.active) pergola.Key.active.release();

        pergola.extend(this, arguments[0]);
        self = pergola.Key.active = this;
        handle = this.handle;

        if (typeof (this.callback = this.callback || {}) == "function") {
          handle.kPressHandler = handle.kDownHandler = handle.kUpHandler = this.callback;   
        }
        else {
          handle.kDownHandler = this.callback.keydown || handle.keydown || pergola.keydown;        
          handle.kPressHandler = this.callback.keypress || handle.keypress || pergola.keypress;
          handle.kUpHandler = this.callback.keyup || handle.keyup || pergola.keyup;
        }

        doc.addEventListener("keydown", this.keydown, false);
	      doc.addEventListener("keypress", this.keypress, false);
	      doc.addEventListener("keyup", this.keyup, false);
        doc.addEventListener("mouseup", this.release, false);

        if (!("cursor" in this) || this.cursor) {
          handle.inputCursor = new pergola.InputCursor()
          .init({
            user : handle
          });
        }

        return this;
      },

      keydown : function (evt) {
        handle.kDownHandler(evt);
      },

      keypress : function (evt) {
        handle.kPressHandler(evt);
      },

      keyup : function (evt) {
        handle.kUpHandler(evt);
      },

      release : function () {
        if ("stop" in self && !self.stop) return;

        doc.removeEventListener("keydown", self.keydown, false);
        doc.removeEventListener("keypress", self.keypress, false);
        doc.removeEventListener("keyup", self.keyup, false);
        doc.removeEventListener("mouseup", self.release, false);                
        pergola.Key.active = null;
        if (!("cursor" in self) || self.cursor) handle.inputCursor.remove();
        if (handle.endInput) handle.endInput(arguments[0]);
        if (self.destroy) pergola.destroy(handle, self.destroy);
      }
 
    };
  },

  keydown : function (evt) {
    switch (evt.keyCode) {
      case 8: {                   // BACKSPACE
        this.value = this.value.substring(0, this.value.length - 1);
        this.setValue();
        evt.preventDefault();
        break;
      }
      case 46: {                  // DELETE
        // Needs cursor position handling
        break;
      }
      case 9: {                   // TAB
        evt.preventDefault();
        break;
      }
      case 13: {                  // ENTER
        pergola.Key.active.release();
        break;
      }
      case 112 : case 113 : case 114 : case 115 : case 116 : case 117 : case 118 : case 119 : case 120 : case 121 : case 122 : case 123 : {		// F1-F12
				evt.preventDefault();
				break;
			}
    }
  },

  keypress : function (evt) {
    var c = evt.charCode;
    if (c > 31 && c < 65536) {
      this.value += String.fromCharCode(c);
      this.setValue();
    }
  },

  keypressNum : function (evt) {
    var v = String.fromCharCode(evt.charCode);
    if (/[0-9]|[.]|[-]/.test(v)) {
      this.value += v;
      this.setValue();
    }
  },

  keypressInt : function (evt) {
    var v = String.fromCharCode(evt.charCode);
    if (/[0-9]||[-]/.test(v)) {
      this.value += v;
      this.setValue();
    }
  },

  keypressPos : function (evt) {
    var v = String.fromCharCode(evt.charCode);
    if (/[0-9]|[.]/.test(v)) {
      this.value += v;
      this.setValue();
    }
  },

  keypressPosInt : function (evt) {
    var v = String.fromCharCode(evt.charCode);
    if (/[0-9]/.test(v)) {
      this.value += v;
      this.setValue();
    }
  },

  keypressHex : function (evt) {
    var v = String.fromCharCode(evt.charCode);
    if (/[ABCDEF]|[0-9]/i.test(v)) {
      this.value += v;
      this.setValue();
    }
  },

  keyup : function (evt) {

  },





	Undo : function () {
		var max = undos;

		this.buffer = [];
		this.index = 0;

		pergola.extend(pergola.Undo.prototype, {

			canUndo : function () {return this.index > 0},

			canRedo : function () {return this.buffer.length > this.index},

			add : function (o) {
				var s = this.buffer.splice(this.index, this.buffer.length - this.index++);
				this.update();
				this.buffer.push(o);
				this.destroy(s);

				return this;
			},

			update : function () {
				if (this.buffer.length == max) {
					var s = this.buffer.shift();
					this.index--;
					this.destroy([s]);
				}
			},

			cancel : function () {
				this.buffer.pop();
				this.index--;
			},

			destroy : function (elements) {
				var e, o,
						buff = [],
            dList = [];

				for (var i in this.buffer) {
					for (var j in this.buffer[i]) {
						buff.push(this.buffer[i][j].obj)
					}
				}
				for (var i in elements) {
					for (var j in elements[i]) {
						e = elements[i][j];
						o = e.obj;
            if (!isSame(o)) dList.push(o);
					}
				}
				for (var i in dList) {
					o = dList[i];
					if (o.pendingDelete && o.destroy) o.destroy();
				}

        function isSame (o) {
					for (var i in buff) {
						if (o == buff[i]) return true;
					}
				}
			},

			flush : function () {
				this.destroy(this.buffer);
				this.buffer = [];
			},

			undo : function () {
				if (!this.canUndo()) return;

				var buffer = this.buffer[--this.index];
				for (var i in buffer)	buffer[i].undo();
			},

			redo : function () {
				if (!this.canRedo()) return;

				var buffer = this.buffer[this.index++];
				for (var i in buffer)	buffer[i].redo();
			},

			handleEvent : function (evt) {
				evt.preventDefault();
				if (!evt.ctrlKey || evt.keyCode != 90) return;
				evt.shiftKey ? this.redo() : this.undo();
			},

			register : function () {
				document.documentElement.addEventListener("keydown", this, false);

				return this;
			},

			disable : function () {
				document.documentElement.removeEventListener("keydown", this, false);
			}

		});
		
		return this;
	},



  xhrGET : function() {
    var args = arguments[0],
        request = new XMLHttpRequest;

    if (args.mime && request.overrideMimeType) request.overrideMimeType(args.mime);
    request.open("GET", args.url, true);
    request.onreadystatechange = function() {
      if (this.readyState == 4) {
        if (this.status < 300) {
          if (args.callback) args.callback(request);
        }
      }
    };
    request.send(null);
  },

  xhrPOST : function() {
    var args = arguments[0],
        request = new XMLHttpRequest;

    if (args.mime && request.overrideMimeType) request.overrideMimeType(args.mime);
    request.open("POST", args.url, true);
    request.onreadystatechange = function() {
      if (request.readyState == 4) {
        if (this.status == 200) {
          if (args.callback) args.callback(args.file);
        }
      }
    };
    request.send("file=" + args.file + "&data=" + args.data);
  },

 	loadSVG : function () {
  	var args = arguments[0];

  	if (!args.url) {
			alert("To load a document you must specify its url.");
			return;
		}
		if (!args.target) {
			alert("To load a document you must specify a target container.");
			return;
		}
  	
	  if (args.progress && pergola.remote) args.progress.start();
	  
	  if (window.XMLHttpRequest) {
	    var LOF,
	        request = new XMLHttpRequest(),
	        parser,
	        doc,
					loaded;
	
	    request.onreadystatechange = function () {
	      if (pergola.remote) {
	        if(request.readyState == 3) {
	          if(!LOF) LOF = request.getResponseHeader("Content-Length");
	          if (args.progress) args.progress.status.setAttribute("width", request.responseText.length * (args.progress.extent / LOF));
	        }
	      }
	      if (request.readyState == 4) {
	        if (request.status == 200) {
	          if (window.DOMParser) {
	            parser = new DOMParser();
	            doc = parser.parseFromString(request.responseText, "text/xml");
	          }
	          else doc = request.responseXML;
	
	          loaded = addDocument(document.importNode(doc.documentElement, true));
	          if (args.progress) args.progress.stop();
	          return loaded;
	        }
	        else if (request.status == 0) {
	          try {
	            if (window.DOMParser) {
	              parser = new DOMParser();
	              doc = parser.parseFromString(request.responseText, "text/xml");
	            }
	            else doc = request.responseXML;
	
	            return addDocument(document.importNode(request.responseXML.documentElement, true)); 
	          }
	          catch(e) {
	            alert (pergola.messages.noLocalLoading);
	          }
	        }
	        else if (request.status == 404) {
	        	if (args.progress) args.progress.stop();
	          alert (pergola.messages.file404);
	        }
	      }
	    };
	
	    try {
	      request.open("GET", args.url, true);
	      request.send(null);
	    }
	    catch(e) {
	    	if (args.progress) args.progress.stop();
	      alert (pergola.messages.loadingFail);
	      return;
	    }
	  }
	  else if (window.getURL) {
	    getURL(args.url, getURLCallback);
	    function getURLCallback(data) {
	      if(data.success) {
	      var node = parseXML(data.content, document);
	      loaded = addDocument(node.firstChild);
	      if (args.progress) args.progress.stop();
	      return loaded;
	      }
	      else {
	      	if (args.progress) args.progress.stop();
					alert (pergola.messages.loadingFail);
				}
	    }
	  }
	
	  function addDocument (node) {
	    var newdoc = node;
	
	    while (newdoc != null && newdoc.nodeName != "svg") newdoc = newdoc.nextSibling;
	    if (newdoc == null) {
	      alert (pergola.messages.svgNotFound + args.url);
	      return null;
	    }
	
	    if (args.target.constructor == pergola.Window) {
				args.newdoc = newdoc;
				args.target.doc.append(args);
			}
	    else args.target.appendChild(newdoc);
	
	    return newdoc;
	
	  }
	},


	geo : {

		distance : function (lat1, lon1, lat2, lon2) {
	    var r = 6378.1, // km
					dLat = (lat2 - lat1).toRad(),
					dLon = (lon2 - lon1).toRad(),
					a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
			    		Math.sin(dLon / 2) * Math.sin(dLon / 2) * Math.cos(lat1.toRad()) * Math.cos(lat2.toRad()), 
					c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a)); 
	
	    return r * c;
	  },

	  kmToMi : function (d) {
			return d / 1.609344;
		},

		kmToNmi : function (d) {
			return d / 1.852;
		},

		formatMiles : function (l) {
    	var yards = l * 1760,
					feet,
					inches;

	    if (l < .25 && l > .1) {
	    	feet = yards % 3;
				inches = (feet - parseInt(feet)) * 12;

				return parseInt(yards) + " yd " + parseInt(feet) + "\u2032 " + parseInt(inches) + "\u2033";
			}
			else if (l <= .1) {
				feet = yards * 3;
				inches = (feet - parseInt(feet)) * 12;

				return parseInt(feet) + "\u2032 " + parseInt(inches) + "\u2033";
			}
			return l.toFixed(3) + " mi";
	  },

    formatKm : function (l) {
      var metres = l * 1000;

      if (l < .5) {
				return metres.toFixed(2) + " m";
			}
			return l.toFixed(3) + " Km";
		}
	},


  hexToDecimal : function (n) {
    return parseInt(n, 16);
  },

  decimalToHex : function (n) {
    return n.toString(16);
  },


  toArray : function (o) {
    var array = [];
    for (var prop in o) array.push(o[prop]);
    return array;
  },


	geom : {},

	color : {}

};



var $C = pergola.SVG,
    $H = pergola.HTML,
    $M = pergola.message,
    $D;



if (container) {
	if (pergola.container = document.getElementById(container)) {
		pergola.doc = $C({element : "svg", width : svgWidth || "100%", height : svgHeight || "100%", appendTo : pergola.container});
	}
	else alert('The HTML element with ID \"' + container + '\" was not found.');
}

pergola.defs = $C({element : "defs", id : "pergola_defs", appendTo : pergola.doc});

window.addEventListener("resize", pergola.resize, false);


