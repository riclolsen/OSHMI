
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/



pergola.Class = function (name) {
  this.name = name || "".xmlNameGenerator();
  this.id = this.name.toXML();
};



pergola.extend (pergola.Class.prototype, {

  x : 0,
  y : 0,
  'font-family' : pergola.presentationAttributes.all['font-family'],
  'font-size' : pergola.presentationAttributes.all['font-size'],
  stroke : pergola.presentationAttributes.all.stroke,

  registerEvents : function (tgt, e, handler) {
    if (typeof e == "string") e = [e];
    for (var a in e) tgt.addEventListener(e[a], handler || this, false);
  },

  unregisterEvents : function (tgt, e, handler) {
    if (typeof e == "string") e = [e];
    for (var a in e) tgt.removeEventListener(e[a], handler || this, false);
  },

  eventBehavior : function (evt) {
    evt.preventDefault();
    evt.stopPropagation();
  },

  setPointerEvents : function () {
    var args = arguments[0];
    for (var a in args) args[a].setAttributeNS(null, "pointer-events", "all");
  },

  removePointerEvents : function () {
    var args = arguments[0];
    for (var a in args) args[a].setAttributeNS(null, "pointer-events", "none");
  },

	memento : function () {
  	var o = {obj : this};
  	for (var a in arguments[0]) o[a] = arguments[0][a];		  
    return o;
  },

  absoluteX : function (node) {
    return node.getCTM().e;
  },

  absoluteY : function (node) {
    return node.getCTM().f;
  },

  ff_absoluteX : function () {
    var owner = this.owner,
				sub = 0;

		while (owner != undefined) {
			sub += owner.x;
			owner = owner.owner;
		}
		return sub;
  },

  ff_absoluteY : function () {
    var owner = this.owner,
				sub = 0;

		while (owner != undefined) {
			sub += owner.y;
			owner = owner.owner;
		}
		return sub;
  },

  setXYWH : function (element) {
    element.setAttributeNS(null, "x", this.x);
    element.setAttributeNS(null, "y", this.y);
    element.setAttributeNS(null, "width", this.width);
    element.setAttributeNS(null, "height", this.height);
  },

  moveXY : function (evt) {
    var t = this.target;

    t.setAttributeNS(null, "x", evt.clientX - this.offset.x);
    t.setAttributeNS(null, "y", evt.clientY - this.offset.y);
  },

  moveByTranslation : function (evt) {
    this.target.setAttributeNS(null, "transform", "translate(" + (evt.clientX - this.offset.x) + " " + (evt.clientY - this.offset.y) + ")");
  },

  checkUserFunction : function (evt) {
    if (typeof this.fn == "string") this[this.fn](evt);
    else if (typeof this.fn == "function") this.fn(evt);
  },

  setPositionToStrokeWidth : function (sw) {
    return !sw.isEven() / 2;
  },

  sweep : function (props) {
    for (var p in props) delete this[p];
  },


  autoScroll : function () {
    var owner = this.owner,
        v = owner.autoscrollValue;

    if (this.left) this.owner.ctm.e += v * (this.owner.ctm.e < -v);
    else if (this.right) this.owner.ctm.e -= v * (this.owner.ctm.e > this.range.minX);
    if (this.up) this.owner.ctm.f += v * (this.owner.ctm.f < -v);
    else if (this.down) this.owner.ctm.f -= v * (this.owner.ctm.f > this.range.minY);
    this.owner.matrixTransformApply();
    this.owner.horScrollSliderSync();
    this.owner.vertScrollSliderSync();
    this.offset = owner.getOffset();
    if (this.timer.fn) this[this.timer.fn]();
  },


  resizeN : function (evt) {
    var point = pergola.mousePoint(evt),
        t = this.target;

    t.height += t.y - (point.y - this.offset.y);
    if (t.height < t.minHeight) {
      t.height = t.minHeight;
      return;
    }
    t.y = point.y - this.offset.y;
    t.container.setAttributeNS(null, "height", t.height);
    t.container.setAttributeNS(null, "y", t.y);
    t.doc.transformUpdate();
  },

  resizeS : function (evt) {
    var point = pergola.mousePoint(evt),
				t = this.target;

    t.height = (point.y + this.offset.y) - t.y;
    if (t.height < t.minHeight) {
      t.height = t.minHeight;
      return;
    }
    t.container.setAttributeNS(null, "height", t.height);
    t.doc.transformUpdate();
  },

  resizeE : function (evt) {
    var point = pergola.mousePoint(evt),
				t = this.target;

    t.width = (point.x + this.offset.x) - t.x;
    if (t.width < t.minWidth) {
      t.width = t.minWidth;
      return;
    }
    t.container.setAttributeNS(null, "width", t.width);
    t.doc.transformUpdate();
  },

  resizeW : function (evt) {
    var point = pergola.mousePoint(evt),
        t = this.target;

    t.width += t.x - (point.x - this.offset.x);
    if (t.width < t.minWidth) {
      t.width = t.minWidth;
      return;
    }
    t.x = point.x - this.offset.x;
    t.container.setAttributeNS(null, "width", t.width);
    t.container.setAttributeNS(null, "x", t.x);
    t.doc.transformUpdate();
  },

  resizeNE : function (evt) {
    this.resizeN(evt);
    this.resizeE(evt);
  },

  resizeNW : function (evt) {
    this.resizeN(evt);
    this.resizeW(evt);
  },

  resizeSE : function (evt) {
    this.resizeS(evt);
    this.resizeE(evt);
  },

  resizeSW : function (evt) {
    this.resizeS(evt);
    this.resizeW(evt);
  },



  toolInit : function (evt, tool) {
  	if (evt.detail == 2) {
	    switch(tool) {
	      case this.lensTool : this.lensToolDoubleClick(evt);
	      break;
	      case this.handTool : this.handToolDoubleClick(evt);
	      break;
	      case this.itineraryTool : this.itineraryToolDoubleClick(evt);
	      break;
	    }
    }
    tool.selected? this.releaseTool(tool) : this.activateTool(tool);
  },

  activateTool : function (tool) {

  	if (pergola.Key.active) pergola.Key.active.release();

    for (var a in this.exclusiveTools) {
    	var t = this.exclusiveTools[a];
			if (t.selected) this.releaseTool(t);
		}

    tool.selected = true;
    tool.suspended = false;
    this.doc.currentTool = tool;
    this.buttonSelect(tool);

    if (this.type == "editor") {
      if (tool.type == "selection") this.doc.updateSelection();
      if (tool.coordinates) this.registerEvents(this.doc.transformable, "mousemove", this.status.coordinates);
    }

		if (this.map) {
			this.map.remove(this.interact);
      if (tool.location) this.registerEvents(this.doc.transformable, "mousemove", this.getLocation);
    }

    this.doc.container.setAttributeNS(null, "cursor", tool.cursor);
    if (tool.altCursor) {
			this.doc.key = pergola.Key()
	    .register({
	      handle : this.doc,
	      callback: {
	        keydown : function (evt) {
						evt.preventDefault();
						if (evt.altKey) this.container.setAttributeNS(null, "cursor", this.currentTool.altCursor);
					},
	        keyup : function (evt) {
            evt.preventDefault();
						this.container.setAttributeNS(null, "cursor", this.currentTool.cursor);
					}
	      },
	      cursor : false,
        destroy : "key"          
	    });
		}	
			
  },

  releaseTool : function (tool) {
    tool.selected = false;
    this.buttonDeselect(tool);

    if (this.doc.key) this.doc.key.release();
    this.doc.container.setAttributeNS(null, "cursor", "default");

    if (this.type == "editor") {
      if (tool.coordinates) this.unregisterEvents(this.doc.transformable, "mousemove", this.status.coordinates);
    }

		if (this.map) {
      if (tool.location) this.unregisterEvents(this.doc.transformable, "mousemove", this.getLocation);
      this.map.add(this.interact);
    }
		this.doc.currentTool = null;
  },

  toolHandler : function (evt) {
  	var tool;
		for (var i in this.tools) {
			tool = this.tools[i];
			if (tool.selected && tool.actOnCanvas) {
				this.doc[tool.fn](evt);
				break;
			}
		}
  },

  lensToolDoubleClick : function (evt) {
  	var doc = this.doc,
				editor = (this.type == "editor");

  	if (this.map) {
			this.mapZoom(2);
			return;
		}

  	if (doc.scaleFactor == 1) return;

		doc.scaleFactor = 1;
		if (editor) doc.hideSelectionOnScale();
		doc.scale();
		if (editor) doc.redrawOnScale();
  },

  handToolDoubleClick : function (evt) {
		if (this.map) {
			this.centerMap({lon : 0, lat : 0});
			this.mapZoom(1);
		}
		else {
			var doc = this.doc;
			doc.ctm.e = doc.ctm.f = 0;
			doc.matrixTransformApply();
			doc.horScrollSliderSync();
			doc.vertScrollSliderSync();
		}
  },

  itineraryToolDoubleClick : function (evt) {
		if (this.doc.itinerary) {
			this.doc.itinerary.remove();
			delete this.doc.itinerary;
		}
  },

  zoomBylensToolInit : function (evt) {
  	var fn = this.owner.map ? "mapZoomBylensTool" : "zoomBylensTool";

    if (this.owner.type == "editor") this.hideSelectionOnScale();

    if(evt.altKey) this.scaleDown = true;
		pergola.rubberBox.set({
      user : this,
      fn : fn,
      point : pergola.mousePoint(evt)
    });

    pergola.dragarea.resize(this.absoluteX(this.container), this.absoluteY(this.container), this.visibleW, this.visibleH)
    .activate({
      callback : pergola.rubberBox.update,
      handle : pergola.rubberBox,
      offset : this.getOffset(),
      cursor : this.currentTool.cursor,
      altCursor : this.currentTool.altCursor,
      updateCoordinates : false
    });
  },


  zoomBylensTool : function (evt) {
    var width = pergola.rubberBox.width;

    if (this.scaleDown) {
      this.scaleDown = false;
      if (this.scaleFactor == this.minScale) return;
      this.scaleFactor = (this.scaleFactor / 2).trim(4);
      if (this.scaleFactor < this.minScale) this.scaleFactor = this.minScale;
    }
    else {
      if (this.scaleFactor == this.maxScale) return;
      this.scaleFactor = (width < 4) ? (this.scaleFactor * 2).trim(4) : (this.scaleFactor * (this.visibleW / width)).trim(3);
      if (this.scaleFactor > this.maxScale) this.scaleFactor = this.maxScale;
    }
    this.scale(pergola.rubberBox);
    if (this.owner.type == "editor") this.redrawOnScale();
  },

  mapZoomBylensTool : function (evt) {
    var owner = this.owner,
        map = owner.map,
        ctm = this.ctm,
        rubberBox = pergola.rubberBox,
        z = map.zoom(),
        origin = polymaps.origin,
        offset = {
					x : ctm.e + origin.x,
					y : ctm.f + origin.y
				},
        point = pergola.mousePoint(evt),
        m = {x : point.x - offset.x, y : point.y - offset.y};
        x = rubberBox.x - offset.x,
        y = rubberBox.y - offset.y;

    if (rubberBox.width && rubberBox.height) {
      map.extent([
        map.pointLocation({x : x, y : y}),
        map.pointLocation({x : x + rubberBox.width, y : y + rubberBox.height})
      ]);
      map.zoom(parseInt(map.zoom()));
    }
    else {
      if (this.scaleDown) {
        z = Math.ceil(z) - z - 1;
        this.scaleDown = false;
      }
      else z = 1 - z + Math.floor(z);
      owner.centerMap(map.pointLocation(m));
      map.zoomBy(z);
    }
    z = map.zoom();
    owner.mapZoomLevelMark(z);
    if (owner.status) owner.status.zoom(z);
  },

  scrollByhandToolInit : function (evt) {
    var offset = pergola.mousePoint(evt);
    if (this.owner.map) {
      var m = {x : offset.x - (this.ctm.e + polymaps.origin.x), y : offset.y - (this.ctm.f + polymaps.origin.y)};
      if (this.owner.status) this.owner.status.location(this.owner.map.pointLocation(m));
    }
    pergola.dragarea.activate({
      callback : this.scrollByHandTool,
      handle : this,
      offset : offset,
      cursor : this.currentTool.cursor,
      updateCoordinates : false
    });
  },

  scrollByHandTool : function (evt) {
    var handle = this.handle,
        map = handle.owner.map,
        point = pergola.mousePoint(evt);

    if (map) {
      map.panBy({x : point.x - this.offset.x, y : point.y - this.offset.y});
      if (handle.owner.status) handle.owner.status.center(map.center());
      this.offset = point;
      return;
    }

    var ctm = handle.ctm,
        e = ctm.e + (point.x - this.offset.x),
        f = ctm.f + (point.y - this.offset.y);

    var minX =- (handle.scaledW - handle.visibleW) * (handle.scaledW > handle.visibleW) | 0;
    var minY =- (handle.scaledH - handle.visibleH) * (handle.scaledH > handle.visibleH) | 0;
    if (e > 0) e = 0;
    if (f > 0) f = 0;
    if (e < minX) e = minX;
    if (f < minY) f = minY;
    ctm.e = e;
    ctm.f = f;
    handle.matrixTransformApply();
    handle.horScrollSliderSync();
    handle.vertScrollSliderSync();
    this.offset = point;
  },


  measureByRulerTool : function (evt) {
    var map = this.owner.map,
        point = pergola.mousePoint(evt),
				lonInRange;

    if (evt.type == "mousemove") {
    	var c,
					geoPoint2 = map.pointLocation({x : point.x - this.offset.x, y : point.y - this.offset.y}),
					p1Lat = this.geoPoint1.lat,
		      p1Lon = this.geoPoint1.lon,
		      p2Lat = geoPoint2.lat,
		      p2Lon = geoPoint2.lon,
		      diff = p1Lon - p2Lon,
		      dist = pergola.geo.distance(p1Lat, p1Lon, p2Lat, p2Lon),
					warn = (diff < -180 || diff > 180) ? " (!longitude range > 180\u00B0)" : "",
					w;

      this.line.setAttributeNS(null, "x2", point.x);
      this.line.setAttributeNS(null, "y2", point.y);
      this.valueTipGroup.setAttributeNS(null, "transform", "translate(" + (point.x + 12) + " " + (point.y - 8) + ")");

      if (map.unit == "mi") dist = pergola.geo.kmToMi(dist);
      else if (map.unit == "nmi") dist = pergola.geo.kmToNmi(dist);

			if (map.unit == "Km") {
				c = pergola.geo.formatKm(dist);
			}
			else if (map.unit == "nmi") {
				c = dist.toFixed(3) + " " + map.unit;
			}
			else {
				c = pergola.geo.formatMiles(dist);
			}

      this.text.firstChild.data = c + warn;
      lonInRange = this.owner.lonToRange(p2Lon);
      this.lon.firstChild.data = "Lon: " + lonInRange.toFixed(3);
      this.lat.firstChild.data = "Lat: " + geoPoint2.lat.toFixed(3);
      w = Math.max.apply(null, [this.text.getComputedTextLength(), this.lon.getComputedTextLength(), this.lat.getComputedTextLength()]);
      this.box.setAttributeNS(null, "width", w + 10);
      this.text.setAttributeNS(null, "x", w + 6);
    }

    else if (evt.type == "mouseup" || evt.type == "mouseout") {
      this.unregisterEvents(this.owner.background.rect, ["mousemove", "mouseout", "mouseup"], this.owner);
      pergola.systemComponents.removeChild(this.line);
      pergola.systemComponents.removeChild(this.valueTipGroup);
      this.setPointerEvents([this.container]);
    }

    else if (evt.type == "mousedown") {
    	var origin = polymaps.origin,
					cross = pergola.marker.cross(),
					bbox;

			this.removePointerEvents([this.container]);
      this.registerEvents(this.owner.background.rect, ["mousemove", "mouseout", "mouseup"], this.owner);
      this.offset = {
				x : this.ctm.e + origin.x,
				y : this.ctm.f + origin.y
			};
      this.geoPoint1 = map.pointLocation({x : point.x - this.offset.x, y : point.y - this.offset.y});
      if (this.owner.status) this.owner.status.location(this.geoPoint1);      
      this.line = $C({
        element : "line",
        x1 : point.x,
        y1 : point.y,
        x2 : point.x,
        y2 : point.y,
        stroke : "#00FFFF",
        "stroke-width" : 1,
        "marker-start" : cross,
        "marker-end" : cross,
        "pointer-events" : "none",
        appendTo : pergola.systemComponents
      });
      this.valueTipGroup = $C({element : "g", transform : "translate(" + (point.x + 12) + " " + (point.y - 8) + ")", "pointer-events" : "none", appendTo : pergola.systemComponents});
      this.box = $C({element : "rect", x : .5, y : -7.5, rx : 2, width : 1, height : 1, fill : "url(#quickTipGrad)", stroke : "#808080", appendTo : this.valueTipGroup});
      this.text = $C({element : "text", x : 46, y : 5, 'font-size' : "11px", 'text-anchor' : 'end', textNode : "0.00 " + map.unit, appendTo : this.valueTipGroup});
      lonInRange = this.owner.lonToRange(this.geoPoint1.lon);
      this.lon = $C({element : "text", x : 6, y : 16, 'font-size' : "11px", textNode : "Lon: " + lonInRange.toFixed(3), appendTo : this.valueTipGroup});
      this.lat = $C({element : "text", x : 6, y : 27, 'font-size' : "11px", textNode : "Lat: " + this.geoPoint1.lat.toFixed(3), appendTo : this.valueTipGroup});
      bbox = this.valueTipGroup.getBBox();
      this.box.setAttributeNS(null, "width", bbox.width);
      this.box.setAttributeNS(null, "height", bbox.height + 2);
    }
  },

  measureItinerary : function (evt) {
    var map = this.owner.map,
				origin = polymaps.origin,
        point = pergola.mousePoint(evt),
				lonInRange,
				w;

    this.offset = {
			x : this.ctm.e + origin.x,
			y : this.ctm.f + origin.y
		};

    if (this.itinerary) {
    	var it = this.itinerary,
					geoPoint = map.pointLocation({x : point.x - this.offset.x, y : point.y - this.offset.y}),
					p1Lat = it.geoPoint.lat,
		      p1Lon = it.geoPoint.lon,
		      p2Lat = geoPoint.lat,
		      p2Lon = geoPoint.lon,
		      diff = p1Lon - p2Lon,
		      dist = pergola.geo.distance(p1Lat, p1Lon, p2Lat, p2Lon);

			it.remove();
			it.totalDistance += dist;
			it.warn = (diff < -180 || diff > 180) ? " (!longitude range > 180\u00B0)" : "";
			it.coordinates.push([geoPoint.lon, geoPoint.lat]);
			it.width = width([
				(convert(it.totalDistance) + it.warn).width("11px", 0, "bold"),
				("Lon: " + it.geoPoint.lon.toFixed(4)).width("11px", 0, "bold"),
				("Lat: " + it.geoPoint.lat.toFixed(4)).width("11px", 0, "bold")
			]);
			it.geoPoint = geoPoint;
      it.render(map);
		}
		else {
			var dot = pergola.marker.dot({radius : 24, fill : "#0080FF", stroke : "#FF6000"});
			this.itinerary = {
				feature : true,
				totalDistance : 0,
				geoPoint : map.pointLocation({x : point.x - this.offset.x, y : point.y - this.offset.y}),
				warn : "",
				updateUnit : function (map) {
					this.remove();
					this.render(map);
				},
				remove : function () {
					map.remove(this.layer);
					delete this.layer;		
				},
				render : function (map) {
					lonInRange = pergola.Window.active().lonToRange(this.geoPoint.lon);
					this.layer = polymaps.geoJson(this)
			    .features([
			      {
			        "geometry" : {
			        	"type" : "LineString",
			          "coordinates" : this.coordinates,
			          "style" : "fill: none; stroke: #808080; stroke-width: 1px; opacity: .35; marker-start:"+dot+"; marker-mid: "+dot+"; marker-end:"+dot+";"
			        }
			      },
			      {
			        "geometry" : {
			        	"type" : "LineString",
			          "coordinates" : this.coordinates,
			          "style" : "fill: none; stroke: #C00000; stroke-width: 2px; stroke-dasharray: 5,2;"
			        }
			      },
			      {
			        "geometry" : {
			          "type" : "Point",
			          "coordinates" : [this.geoPoint.lon, this.geoPoint.lat],
			          "elements" : [
					        {element : "rect", x : 6.5, y : -33.5, rx : 2.5, width : this.width + 8, height : 37, fill : "#F0F0FF", stroke : "#808080", opacity : .5},
					        {element : "text", x : 10, y : -22, "font-size" : "11px", "font-weight" : "bold", "pointer-events" : "none", textNode : convert(this.totalDistance) + this.warn},
						      {element : "text", x : 10, y : -11, 'font-size' : "11px", "font-weight" : "bold", textNode : "Lon: " + lonInRange.toFixed(4)},
						      {element : "text", x : 10, y : 0, 'font-size' : "11px", "font-weight" : "bold", textNode : "Lat: " + this.geoPoint.lat.toFixed(4)}
					      ]
			        }
			      }
			    ]);
			    map.add(this.layer);
				}
			};
			var it = this.itinerary;
			it.coordinates = [[it.geoPoint.lon, it.geoPoint.lat]];
			it.width = width([
				("Lon: " + it.geoPoint.lon.toFixed(4)).width("11px", 0, "bold"),
				("Lat: " + it.geoPoint.lat.toFixed(4)).width("11px", 0, "bold")
			]);

	    it.render(map);
		}

    function width (v) {
			return Math.max.apply(null, v);
		};

		function convert (d) {
			if (map.unit == "mi") d /= 1.609344;
      else if (map.unit == "nmi") d /= 1.852;

			if (map.unit == "Km") {
				d = pergola.geo.formatKm(d);
			}
			else if (map.unit == "nmi") {
				d = d.toFixed(3) + " " + map.unit;
			}
			else {
				d = pergola.geo.formatMiles(d);
			}

			return d;
		};
  },


  zoomDownByButtonInit : function (evt) {
    var callback;
    if (this.map) {
      callback = this.fn = this.mapZoomDownByButton;
    }
    else {
      this.doc.zoomStep = 2;
      callback = this.doc.fn = this.zoomDownByButton;
    }
    this.zoomTimerInit(evt, this.zoomDown, callback);
  },

  zoomUpByButtonInit : function (evt) {
    var callback;
    if (this.map) {
      callback = this.mapZoomUpByButton;
      this.fn = this.mapZoomUpByButton;
    }
    else {
      this.doc.zoomStep = 2;
      callback = this.doc.fn = this.zoomUpByButton;
    }
    this.zoomTimerInit(evt, this.zoomUp, callback);
  },

  zoomTimerInit : function (evt, obj, callback) {
    var timerOwner,
        f;

    if (evt.type == "mousedown") {
      this.registerEvents(evt.target, ["mouseout", "mouseup"]);
      if (this.type == "editor") this.doc.hideSelectionOnScale();
      if (this.map) {
        timerOwner =  this;
        f = 30;
        this.zoomStep = .05;
      }
      else {
        timerOwner = this.doc;
        f = 20;
        this.doc.zoomStep = 1.05;
      }
      if (obj.zoomTimer) obj.zoomTimer.initialize();
      else {
        obj.zoomTimer = pergola.Timer()
        .initialize({
          handle : timerOwner,
          callback : callback,
          delay : 250,
          frequence : f
        });
      }
    }

    else if (evt.type == "mouseup" || evt.type == "mouseout") {
      var zoomTimer = obj.zoomTimer;

      if (this.map) {
        var map = this.map,
            range = map.zoomRange(),
            z = map.zoom();

        if (this.callback == this.mapZoomUpByButton) {
          obj.z = Math.floor(z) + 1;
          if (obj.z > range[1]) obj.z = range[1];
        }
        else if (this.callback == this.mapZoomDownByButton) {
          obj.z = Math.ceil(z) - 1;
          if (obj.z < range[0]) obj.z = range[0];
        }

        if (zoomTimer.interval == null && evt.type == "mouseup") {
          zoomTimer.clear();
          map.zoom(obj.z);
        }
// fadeTimer lets zoomTimer continue until next integer zoom level
        else {
          if (obj.fadeTimer) obj.fadeTimer.initialize();
          else {
            obj.fadeTimer = pergola.Timer()
            .initialize({
              handle : obj,
              callback : this.mapZoomFadeout,
              frequence : 29
            });
          }
        }
        this.mapZoomLevelMark();
        if (this.status) this.status.zoom(obj.z);
      }
      else {
        if (zoomTimer.interval == null && evt.type == "mouseup") this.doc.fn();
        zoomTimer.clear();
      }
      this.unregisterEvents(evt.target, ["mouseout", "mouseup"]);

      if (this.type == "editor") this.doc.redrawOnScale();
    }
  },

  mapZoomFadeout : function () {
    var z = this.owner.map.zoom();
    if(parseFloat(z) != parseInt(z)) return;
    this.zoomTimer.clear();
    this.fadeTimer.clear();
    this.z = null;
    this.owner.mapZoomLevelMark(z);
  },

  mapZoomDownByButton : function () {
    this.map.zoomBy(-this.zoomStep);
  },

  mapZoomUpByButton : function () {
    this.map.zoomBy(this.zoomStep);
  },

  zoomDownByButton : function () {
    if (this.scaleFactor == this.minScale) return;
    this.scaleFactor = (this.scaleFactor / this.zoomStep).trim(4);
    if (this.scaleFactor < this.minScale) this.scaleFactor = this.minScale;
    this.scale();
  },

  zoomUpByButton : function () {
    if (this.scaleFactor == this.maxScale) return;
    this.scaleFactor = (this.scaleFactor * this.zoomStep).trim(4);
    if (this.scaleFactor > this.maxScale) this.scaleFactor = this.maxScale;
    this.scale();
  },


  mapHelpInit : function (evt) {
    this.mapHelpOn? this.releaseMapHelp(evt) : this.activateMapHelp(evt);
  },

  activateMapHelp : function (evt) {
    if (this.mapHelpOn) this.releaseMapHelp(evt);
    this.navHelpPanel.container.setAttributeNS(null, "display", "block");
    this.mapHelpOn = true;
    this.mapHelp.selected = true;
    this.mapHelpSuspended = false;
    this.buttonSelect(this.mapHelp);
  },

  releaseMapHelp : function (evt) {
    this.navHelpPanel.container.setAttributeNS(null, "display", "none");
    this.mapHelpOn = false;
    this.mapHelp.selected = false;
    this.buttonDeselect(this.mapHelp);
  },

  quickTipFormat : function () {
    var qTip = this.quickTip;
    if (typeof qTip == "string") {
      if (pergola.quicktips[qTip]) this.quickTip = pergola.quicktips[qTip];
      else alert(qTip + pergola.messages.wrongQuickTip);
    }
    else {
      this.quickTip = new pergola.QuickTip().build(qTip);
    }
  },

  quickTipInit : function () {
    var qTip = this.quickTip;
    this.registerEvents(this.button, "mousemove");
    qTip.owner = this;
    if (qTip.timer) qTip.timer.initialize();
    else {
      qTip.timer = pergola.Timer()
      .initialize({
        handle : qTip,
        callback : this.quickTip.show,
        delay : this.quickTip.delay
      });
    }
  },


  rollover : function (evt) {
    this.button.setAttributeNS(null, "fill-opacity", evt.type == "mouseover" | 0);
    if (this.image) this.image.setAttributeNS(null, "opacity", evt.type == "mouseover" | 0);
    if (this.text && this.hasTextEffect) this.videoInverse(evt);
  },

  videoInverse : function (evt) {
    var fill = (evt.type == "mouseover") ? this.textFillInverse : this.textFill;

    this.text.setAttributeNS(null, "fill", fill);
    if (this.checkmark) this.checkmark.setAttributeNS(null, "fill", fill);
    if (this.arrow) this.arrow.setAttributeNS(null, "fill", fill);
  },

  buttonSelect : function (o) {
    o.button.setAttributeNS(null, "fill-opacity", 1);
  },

  buttonDeselect : function (o) {
    o.button.setAttributeNS(null, "fill-opacity", 0);
  },

  textShow : function () {
    this.text.setAttributeNS(null, "display", "block");
  },

  textHide : function () {
    this.text.setAttributeNS(null, "display", "none");
  },

  textDataUpdate : function (v) {
    this.text.firstChild.data = v;
  }

});






