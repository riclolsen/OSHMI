
/*
 *                                ATTENTION !
 * 
 * THIS IS A MODIFIED VERSION OF THE POLYMAPS LIBRARY AND IS DESIGNED TO WORK IN A STANDALONE SVG CONTEXT.
 *  
 * IT IS STRICTLY FORBIDDEN TO DISTRIBUTE. 
 * THE OFFICIAL POLYMAPS LICENSE FOR DISTRIBUTION DOES NOT APPLY FOR THIS EXPERIMENTAL WORK.
 *  
 * Copyright (c) 2010, SimpleGeo and Stamen Design All rights reserved. 
 * 
 * Latest Polymaps official release at: http://polymaps.org/
*/


var polymaps = {
	version : "2.4.0-standaloneSVG",
	xmlName: [0],
	origin : {x: 0, y: 0},
	y2lat : function(y) {
		return 360 / Math.PI * Math.atan(Math.exp(y * Math.PI / 180)) - 90;
	},
	lat2y : function(lat) {
		return 180 / Math.PI * Math.log(Math.tan(Math.PI / 4 + lat * Math.PI / 360));
	},
// scan-line conversion
	edge : function(a, b) {
		if (a.row > b.row) { var t = a; a = b; b = t; }
		return {
			x0: a.column,
			y0: a.row,
			x1: b.column,
			y1: b.row,
			dx: b.column - a.column,
			dy: b.row - a.row
		};
	},
	scanSpans : function(e0, e1, ymin, ymax, scanLine) {
		var y0 = Math.max(ymin, Math.floor(e1.y0)),
				y1 = Math.min(ymax, Math.ceil(e1.y1));
// sort edges by x-coordinate
		if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ? (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) : (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0)) {
			var t = e0; e0 = e1; e1 = t;
		}
// scan lines!
		var m0 = e0.dx / e0.dy,
				m1 = e1.dx / e1.dy,
				d0 = e0.dx > 0, // use y + 1 to compute x0
				d1 = e1.dx < 0; // use y + 1 to compute x1
		for (var y = y0; y < y1; y++) {
			var x0 = m0 * Math.max(0, Math.min(e0.dy, y + d0 - e0.y0)) + e0.x0,
					x1 = m1 * Math.max(0, Math.min(e1.dy, y + d1 - e1.y0)) + e1.x0;
			scanLine(Math.floor(x1), Math.ceil(x0), y);
		}
	},
// scan-line conversion
	scanTriangle : function(a, b, c, ymin, ymax, scanLine) {
		var edge = polymaps.edge,
				scanSpans = polymaps.scanSpans;
		var ab = edge(a, b),
				bc = edge(b, c),
				ca = edge(c, a);
// sort edges by y-length
		if (ab.dy > bc.dy) { var t = ab; ab = bc; bc = t; }
		if (ab.dy > ca.dy) { var t = ab; ab = ca; ca = t; }
		if (bc.dy > ca.dy) { var t = bc; bc = ca; ca = t; }
// scan span! scan span!
		if (ab.dy) scanSpans(ca, ab, ymin, ymax, scanLine);
		if (bc.dy) scanSpans(ca, bc, ymin, ymax, scanLine);
	},
	resizer : function(e) {
		var c = arguments.callee;
		for (var i = 0; i < c.maps.length; i++) {
			c.maps[i].resize();
		}
	},
	transform : function(a, b, c, d, e, f) {
		var zoomDelta,
				zoomFraction,
				k;
		if (!arguments.length) {
			a = 1; c = 0; e = 0;
			b = 0; d = 1; f = 0;
		}
		var transform = {
			zoomFraction : function(x) {
				if (!arguments.length) return zoomFraction;
				zoomFraction = x;
				zoomDelta = Math.floor(zoomFraction + Math.log(Math.sqrt(a * a + b * b + c * c + d * d)) / Math.log(2));
				k = Math.pow(2, -zoomDelta);
				return transform;
			},
			apply : function(x) {
				var k0 = Math.pow(2, -x.zoom),
						k1 = Math.pow(2, x.zoom - zoomDelta);
				return {
					column: (a * x.column * k0 + c * x.row * k0 + e) * k1,
					row: (b * x.column * k0 + d * x.row * k0 + f) * k1,
					zoom: x.zoom - zoomDelta
				};
			},
			unapply : function(x) {
				var k0 = Math.pow(2, -x.zoom),
						k1 = Math.pow(2, x.zoom + zoomDelta);
				return {
					column: (x.column * k0 * d - x.row * k0 * c - e * d + f * c) / (a * d - b * c) * k1,
					row: (x.column * k0 * b - x.row * k0 * a - e * b + f * a) / (c * b - d * a) * k1,
					zoom: x.zoom + zoomDelta
				};
			},
			toString : function() {
				return "matrix(" + [a * k, b * k, c * k, d * k].join(" ") + " 0 0)";
			}
		};
		return transform.zoomFraction(0);
	},
//==========================================  CACHE  ==============================================
	cache : function(load, unload) {
		var	locks = {},
				map = {},
				head = null,
				tail = null,
				size = 64,
				n = 0;

		function remove(tile) {
			n--;
			if (unload) unload(tile);
			delete map[tile.key];
			if (tile.next) tile.next.prev = tile.prev;
			else if (tail = tile.prev) tail.next = null;
			if (tile.prev) tile.prev.next = tile.next;
			else if (head = tile.next) head.prev = null;
		}

		var cache = {
			peek : function(c) {
				return map[[c.zoom, c.column, c.row].join("/")];
			},
			load : function(c, projection) {
				var key = [c.zoom, c.column, c.row].join("/"),
				tile = map[key];
				if (tile) {
					if (tile.prev) {
						tile.prev.next = tile.next;
						if (tile.next) tile.next.prev = tile.prev;
						else tail = tile.prev;
						tile.prev = null;
						tile.next = head;
						head.prev = tile;
						head = tile;
					}
					tile.lock = 1;
					locks[key] = tile;
					return tile;
				}
				tile = {
					key: key,
					column: c.column,
					row: c.row,
					zoom: c.zoom,
					next: head,
					prev: null,
					lock: 1
				};
				load.call(null, tile, projection);
				locks[key] = map[key] = tile;
				if (head) head.prev = tile;
				else tail = tile;
				head = tile;
				n++;
				return tile;
			},
			unload : function(key) {
				if (!(key in locks)) return false;
				var tile = locks[key];
				tile.lock = 0;
				delete locks[key];
				if (tile.request && tile.request.abort(false)) remove(tile);
				return tile;
			},
			locks : function() {
				return locks;
			},
			size : function(x) {
				if (!arguments.length) return size;
				size = x;
				return cache;
			},
			clear : function() {
				for (var key in map) {
					var tile = map[key];
					if (tile.request) tile.request.abort(false);
					if (unload) unload(map[key]);
					if (tile.lock) {
						tile.lock = 0;
						tile.element.parentNode.removeChild(tile.element);
					}
				}
				locks = {};
				map = {};
				head = tail = null;
				n = 0;
				return cache;
			}
		};
		return cache;
	},
//==========================================  URL  ==============================================
	url : function(template) {
		var hosts = [];

		function format(c) {
			var max = c.zoom < 0 ? 1 : 1 << c.zoom,
			column = c.column % max;
			if (column < 0) column += max;
			return template.replace(/{(.)}/g, function(s, v) {
				switch (v) {
				case "S": return hosts[(Math.abs(c.zoom) + c.row + column) % hosts.length];
				case "Z": return c.zoom;
				case "X": return column;
				case "Y": return c.row;
				case "B": {
					var nw = polymaps.map.coordinateLocation({row: c.row, column: column, zoom: c.zoom}),
					se = polymaps.map.coordinateLocation({row: c.row + 1, column: column + 1, zoom: c.zoom}),
					pn = Math.ceil(Math.log(c.zoom) / Math.LN2);
					return se.lat.toFixed(pn) + "," + nw.lon.toFixed(pn) + "," + nw.lat.toFixed(pn) + "," + se.lon.toFixed(pn);
				}
			}
			return v;
			});
		}
	
		format.template = function(x) {
			if (!arguments.length) return template;
			template = x;
			return format;
		};
	
		format.hosts = function(x) {
			if (!arguments.length) return hosts;
			hosts = x;
			return format;
		};
		return format;
	},

//==========================================  DISPATCH  ==============================================
	dispatch : function(that) {
		var types = {};
		
		that.on = function(type, handler) {
			var listeners = types[type] || (types[type] = []);
			for (var i = 0; i < listeners.length; i++) {
				if (listeners[i].handler == handler) return that; // already registered
			}
			listeners.push({handler: handler, on: true});
			return that;
		};

		that.off = function(type, handler) {
			var listeners = types[type];
			if (listeners) for (var i = 0; i < listeners.length; i++) {
				var l = listeners[i];
				if (l.handler == handler) {
					l.on = false;
					listeners.splice(i, 1);
					break;
				}
			}
			return that;
		};
		
		return function(event) {
			var listeners = types[event.type];
			if (!listeners) return;
			listeners = listeners.slice(); // defensive copy
			for (var i = 0; i < listeners.length; i++) {
				var l = listeners[i];
				if (l.on) l.handler.call(that, event);
			}
		};
	},

//==========================================  QUEUE  ==============================================
	queue : (function() {
		var queued = [],
				active = 0,
				size = 6;

		function process() {
			if ((active >= size) || !queued.length) return;
			active++;
			queued.pop()();
		}

		function dequeue(send) {
			for (var i = 0; i < queued.length; i++) {
				if (queued[i] == send) {
					queued.splice(i, 1);
					return true;
				}
			}
			return false;
		}

		function request(url, callback, mimeType) {
			var req;

			function send() {
				req = new XMLHttpRequest();
/*
 * We override the MIME type here so that you can load local files; some
 * browsers don't assign a proper MIME type for local files.
*/
			if (mimeType) {
				req.overrideMimeType(mimeType);
			}
				req.open("GET", url, true);
				req.onreadystatechange = function(e) {
					if (req.readyState == 4) {
						active--;
						if (req.status < 300) {
							callback(req);
						}
						process();
					}
				};
				req.send(null);
			}

			function abort(hard) {
				if (dequeue(send)) return true;
				if (hard && req) {
					req.abort();
					return true;
				}
				return false;
			}

			queued.push(send);
			process();
			return {abort: abort};
		}

		function text(url, callback, mimeType) {
			return request(url, function(req) {
				if (req.responseText) callback(req.responseText);
			}, mimeType);
		}

		function json(url, callback) {
			return request(url, function(req) {
			if (req.responseText) callback(JSON.parse(req.responseText));
			}, "application/json");
		}

		function xml(url, callback) {
			return request(url, function(req) {
			if (req.responseXML) callback(req.responseXML);
			}, "application/xml");
		}

		function image(image, src, callback) {
			var img;

			function send() {
				img = $C({element : "image", "xlink:href" : src});
				active--;
				callback(img);
				process();
				image.setAttributeNS(pergola.ns.xlink, "xlink:href", src);
			}

			function abort(hard) {
				if (dequeue(send)) return true;
				if (hard && img) {
					img.setAttributeNS(pergola.ns.xlink, "xlink:href", "");
					return true;
				} // cancels request
				return false;
			}

			queued.push(send);
			process();
			return {abort: abort};
		}

		return {text: text, xml: xml, json: json, image: image};
	})(),
//==========================================  MAP  ==============================================
	map : function(o) {
		var	container,
				size,
				sizeActual = polymaps.origin,
				sizeRadius = polymaps.origin, // sizeActual / 2
				tileSize = {x: 256, y: 256},
				center = {lat: 0.0, lon: 0.0},
				zoom = 2,
				zoomFraction = 0,
				zoomFactor = 1, // Math.pow(2, zoomFraction)
				zoomRange = [1, 21],
				angle = 0,
				angleCos = 1, // Math.cos(angle)
				angleSin = 0, // Math.sin(angle)
				angleCosi = 1, // Math.cos(-angle)
				angleSini = 0, // Math.sin(-angle)
				ymin = -180, // lat2y(centerRange[0].lat)
				ymax = 180, // lat2y(centerRange[1].lat)
				unit = "Km";

		var centerRange = [
			{lat: polymaps.y2lat(ymin), lon: -Infinity},
			{lat: polymaps.y2lat(ymax), lon: Infinity}
		];

		var map = {
			owner: o,
			unit: unit,
			locationCoordinate : function(l) {
				var c = polymaps.map.locationCoordinate(l),
					k = Math.pow(2, zoom);
				c.column *= k;
				c.row *= k;
				c.zoom += zoom;
				return c;
			},
			coordinateLocation : polymaps.map.coordinateLocation,
			coordinatePoint : function(tileCenter, c) {
				var kc = Math.pow(2, zoom - c.zoom),
					kt = Math.pow(2, zoom - tileCenter.zoom),
					dx = (c.column * kc - tileCenter.column * kt) * tileSize.x * zoomFactor,
					dy = (c.row * kc - tileCenter.row * kt) * tileSize.y * zoomFactor;
				return {
					x: sizeRadius.x + angleCos * dx - angleSin * dy,
					y: sizeRadius.y + angleSin * dx + angleCos * dy
				};
			},
			pointCoordinate : function(tileCenter, p) {
				var kt = Math.pow(2, zoom - tileCenter.zoom),
					dx = (p.x - sizeRadius.x) / zoomFactor,
					dy = (p.y - sizeRadius.y) / zoomFactor;
				return {
					column: tileCenter.column * kt + (angleCosi * dx - angleSini * dy) / tileSize.x,
					row: tileCenter.row * kt + (angleSini * dx + angleCosi * dy) / tileSize.y,
					zoom: zoom
				};
			},
			locationPoint : function(l) {
				var k = Math.pow(2, zoom + zoomFraction - 3) / 45,
						dx = (l.lon - center.lon) * k * tileSize.x,
						dy = (polymaps.lat2y(center.lat) - polymaps.lat2y(l.lat)) * k * tileSize.y;
				return {
					x: sizeRadius.x + angleCos * dx - angleSin * dy,
					y: sizeRadius.y + angleSin * dx + angleCos * dy
				};
			},
			pointLocation : function(p) {
				var k = 45 / Math.pow(2, zoom + zoomFraction - 3),
					dx = (p.x - sizeRadius.x) * k,
					dy = (p.y - sizeRadius.y) * k;
				return {
					lon: center.lon + (angleCosi * dx - angleSini * dy) / tileSize.x,
					lat: polymaps.y2lat(polymaps.lat2y(center.lat) - (angleSini * dx + angleCosi * dy) / tileSize.y)
				};
			},
			container : function(x) {
				if (!arguments.length) return container;
				container = x;
				map.featuresContainer = $C({element: "g", id: container.id + "_features", appendTo: x});
				return map.resize(); // infer size
			},
			focusableParent : function() {
				for (var p = container; p; p = p.parentNode) {
					if (p.tabIndex >= 0) return p;
				}
				return window;
			},
			mouse : function(e) {
				return {x: e.clientX, y: e.clientY}
			},
			size : function(x) {
				if (!arguments.length) return sizeActual;
				size = x;
				return map.resize(); // size tiles
			},
			resize : function() {
				if (!size) {
					var win = pergola.Window.active();
					sizeActual = {x: win.canvas.width, y: win.canvas.height};
					polymaps.resizer.add(map);
				}
				else {
					sizeActual = size;
					polymaps.resizer.remove(map);
				}
				sizeRadius = {x: sizeActual.x / 2, y: sizeActual.y / 2};
				recenter();
				map.dispatch({type: "resize"});
				return map;
			},
			tileSize : function(x) {
				if (!arguments.length) return tileSize;
				tileSize = x;
				map.dispatch({type: "move"});
				return map;
			},
			center : function(x) {
				if (!arguments.length) return center;
				center = x;
				recenter();
				map.dispatch({type: "move"});
				return map;
			},
			panBy : function(x) {
				var k = 45 / Math.pow(2, zoom + zoomFraction - 3),
					dx = x.x * k,
					dy = x.y * k;
				return map.center({
					lon: center.lon + (angleSini * dy - angleCosi * dx) / tileSize.x,
					lat: polymaps.y2lat(polymaps.lat2y(center.lat) + (angleSini * dx + angleCosi * dy) / tileSize.y)
				});
			},
			centerRange : function(x) {
				if (!arguments.length) return centerRange;
				centerRange = x;
				if (centerRange) {
					ymin = centerRange[0].lat > -90 ? polymaps.lat2y(centerRange[0].lat) : -Infinity;
					ymax = centerRange[0].lat < 90 ? polymaps.lat2y(centerRange[1].lat) : Infinity;
				}
				else {
					ymin = -Infinity;
					ymax = Infinity;
				}
				recenter();
				map.dispatch({type: "move"});
				return map;
			},
			zoom : function(x) {
				if (!arguments.length) return zoom + zoomFraction;
				zoom = x;
				rezoom();
				return map.center(center);
			},
			zoomBy : function(z, x0, l) {
				zoom = parseFloat((zoom + zoomFraction + z).toFixed(2));	
				if (arguments.length < 2) return map.zoom(zoom);
// compute the location of x0
				if (arguments.length < 3) l = map.pointLocation(x0);
// update the zoom level
				
				rezoom();
// compute the new point of the location
				var x1 = map.locationPoint(l);
				return map.panBy({x: x0.x - x1.x, y: x0.y - x1.y});
			},
			zoomRange : function(x) {
				if (!arguments.length) return zoomRange;
				zoomRange = x;
				return map.zoom(zoom + zoomFraction);
			},
			extent : function(x) {
				if (!arguments.length) return [
					map.pointLocation({x: 0, y: sizeActual.y}),
					map.pointLocation({x: sizeActual.x, y: 0})
				];
// compute the extent in points, scale factor, and center
				var bl = map.locationPoint(x[0]),
				tr = map.locationPoint(x[1]),
				k = Math.max((tr.x - bl.x) / sizeActual.x, (bl.y - tr.y) / sizeActual.y),
				l = map.pointLocation({x: (bl.x + tr.x) / 2, y: (bl.y + tr.y) / 2});
// update the zoom level
				zoom = zoom + zoomFraction - Math.log(k) / Math.log(2);
				rezoom();
// set the new center
				return map.center(l);
			},
			angle : function(x) {
				if (!arguments.length) return angle;
				angle = x;
				angleCos = Math.cos(angle);
				angleSin = Math.sin(angle);
				angleCosi = Math.cos(-angle);
				angleSini = Math.sin(-angle);
				recenter();
				map.dispatch({type: "move"});
				return map;
			},
			add : function(x) {
				x.map(map);
				return map;
			},
			remove : function(x) {
				x.map(null);
				return map;
			}
		};

		function rezoom() {
			if (zoomRange) {
				if (zoom < zoomRange[0]) zoom = zoomRange[0];
				else if (zoom > zoomRange[1]) zoom = zoomRange[1];
			}
			zoomFraction = zoom - (zoom = Math.round(zoom));
			zoomFactor = Math.pow(2, zoomFraction);
		}
	
		function recenter() {
			if (!centerRange) return;
			var k = 45 / Math.pow(2, zoom + zoomFraction - 3);

// constrain latitude
			var y = Math.max(Math.abs(angleSin * sizeRadius.x + angleCos * sizeRadius.y), Math.abs(angleSini * sizeRadius.x + angleCosi * sizeRadius.y)),
					lat0 = polymaps.y2lat(ymin - y * k / tileSize.y),
					lat1 = polymaps.y2lat(ymax + y * k / tileSize.y);
			center.lat = Math.max(lat0, Math.min(lat1, center.lat));

// constrain longitude
			var x = Math.max(Math.abs(angleSin * sizeRadius.y + angleCos * sizeRadius.x), Math.abs(angleSini * sizeRadius.y + angleCosi * sizeRadius.x)),
					lon0 = centerRange[0].lon - x * k / tileSize.x,
					lon1 = centerRange[1].lon + x * k / tileSize.x;
			center.lon = Math.max(lon0, Math.min(lon1, center.lon));
		}

		map.dispatch = polymaps.dispatch(map);
		return map;
		},

//==========================================  LAYER  ==============================================
	layer : function(load, unload, obj) {
		var cache = polymaps.cache(load, unload).size(512),
				tile = true,
				visible = (obj && obj.feature) ? obj.feature : true,
				zoom,
				id,
				map,
				container = $C({element : "g"}),
				transform,
				levelZoom;

		if (obj) obj.container = container;

		function move() {
			var map = layer.map(), // in case the layer is removed
					mapZoom = map.zoom(),
					mapZoomFraction = mapZoom - (mapZoom = Math.round(mapZoom)),
					mapSize = map.size(),
					mapAngle = map.angle(),
					tileSize = map.tileSize(),
					tileCenter = map.locationCoordinate(map.center());

// set the layer transform
			container.setAttributeNS(null, "transform",
			"translate(" + (mapSize.x / 2) + "," + (mapSize.y / 2) + ")"
			+ (mapAngle ? "rotate(" + mapAngle / Math.PI * 180 + ")" : "")
			+ (mapZoomFraction ? "scale(" + Math.pow(2, mapZoomFraction) + ")" : "")
			+ (transform ? transform.zoomFraction(mapZoomFraction) : ""));

// get the coordinates of the four corners
			var c0 = map.pointCoordinate(tileCenter, polymaps.origin),
					c1 = map.pointCoordinate(tileCenter, {x: mapSize.x, y: 0}),
					c2 = map.pointCoordinate(tileCenter, mapSize),
					c3 = map.pointCoordinate(tileCenter, {x: 0, y: mapSize.y});

// round to pixel boundary to avoid anti-aliasing artifacts
			if (!mapZoomFraction && !mapAngle && !transform) {
				tileCenter.column = (Math.round(tileSize.x * tileCenter.column) + (mapSize.x & 1) / 2) / tileSize.x;
				tileCenter.row = (Math.round(tileSize.y * tileCenter.row) + (mapSize.y & 1) / 2) / tileSize.y;
			}

// layer-specific coordinate transform
			if (transform) {
				c0 = transform.unapply(c0);
				c1 = transform.unapply(c1);
				c2 = transform.unapply(c2);
				c3 = transform.unapply(c3);
				tileCenter = transform.unapply(tileCenter);
			}

// layer-specific zoom transform
			var tileLevel = zoom ? zoom(c0.zoom) - c0.zoom : 0;
			if (tileLevel) {
				var k = Math.pow(2, tileLevel);
				c0.column *= k; c0.row *= k;
				c1.column *= k; c1.row *= k;
				c2.column *= k; c2.row *= k;
				c3.column *= k; c3.row *= k;
				c0.zoom = c1.zoom = c2.zoom = c3.zoom += tileLevel;
			}

// tile-specific projection
			function projection(c) {
				var zoom = c.zoom,
				max = zoom < 0 ? 1 : 1 << zoom,
				column = c.column % max,
				row = c.row;
				if (column < 0) column += max;
				return {
					locationPoint: function(l) {
						var c = polymaps.map.locationCoordinate(l),
						k = Math.pow(2, zoom - c.zoom);
						return {
						x: tileSize.x * (k * c.column - column),
						y: tileSize.y * (k * c.row - row)
						};
					}
				};
			}
// record which tiles are visible
			var oldLocks = cache.locks(), newLocks = {};
// reset the proxy counts
			for (var key in oldLocks) oldLocks[key].proxyCount = 0;
// load the tiles!
			if (visible && tileLevel > -5 && tileLevel < 3) {
				var ymax = c0.zoom < 0 ? 1 : 1 << c0.zoom;
				if (tile) {
					polymaps.scanTriangle(c0, c1, c2, 0, ymax, scanLine);
					polymaps.scanTriangle(c2, c3, c0, 0, ymax, scanLine);
				}
				else {
					var x = Math.floor((c0.column + c2.column) / 2),
					y = Math.max(0, Math.min(ymax - 1, Math.floor((c1.row + c3.row) / 2))),
					z = Math.min(4, c0.zoom);
					x = x >> z << z;
					y = y >> z << z;
					scanLine(x, x + 1, y);
				}
			}

// scan-line conversion
			function scanLine(x0, x1, y) {
				var z = c0.zoom,
					z0 = 2 - tileLevel,
					z1 = 4 + tileLevel;

				for (var x = x0; x < x1; x++) {
					var t = cache.load({column: x, row: y, zoom: z}, projection);
					if (!t.ready && !(t.key in newLocks)) {
						t.proxyRefs = {};
						var c, full, proxy;

// downsample high-resolution tiles
						for (var dz = 1; dz <= z0; dz++) {
							full = true;
							for (var dy = 0, k = 1 << dz; dy <= k; dy++) {
								for (var dx = 0; dx <= k; dx++) {
									proxy = cache.peek(c = {
									column: (x << dz) + dx,
									row: (y << dz) + dy,
									zoom: z + dz
									});
									if (proxy && proxy.ready) {
										newLocks[proxy.key] = cache.load(c);
										proxy.proxyCount++;
										t.proxyRefs[proxy.key] = proxy;
									}
									else full = false;
								}
							}
							if (full) break;
						}

// upsample low-resolution tiles
						if (!full) {
							for (var dz = 1; dz <= z1; dz++) {
								proxy = cache.peek(c = {
									column: x >> dz,
									row: y >> dz,
									zoom: z - dz
								});
								if (proxy && proxy.ready) {
									newLocks[proxy.key] = cache.load(c);
									proxy.proxyCount++;
									t.proxyRefs[proxy.key] = proxy;
									break;
								}
							}
						}
					}
					newLocks[t.key] = t;
				}
			}

// position tiles
			for (var key in newLocks) {
				var t = newLocks[key],
				k = Math.pow(2, t.level = t.zoom - tileCenter.zoom);
				t.element.setAttributeNS(null, "transform", "translate("
				+ (t.x = tileSize.x * (t.column - tileCenter.column * k)) + ","
				+ (t.y = tileSize.y * (t.row - tileCenter.row * k)) + ")");
			}

// remove tiles that are no longer visible
			for (var key in oldLocks) {
				if (!(key in newLocks)) {
					var t = cache.unload(key);
					t.element.parentNode.removeChild(t.element);
					delete t.proxyRefs;
				}
			}

// append tiles that are now visible
			for (var key in newLocks) {
				var t = newLocks[key];
				if (t.element.parentNode != container) {
					container.appendChild(t.element);
					if (layer.show) layer.show(t);
				}
			}

// dispatch the move event
			layer.dispatch({type: "move"});
		}

// remove proxy tiles when tiles load
		function cleanup(e) {
			if (e.tile.proxyRefs) {
				for (var proxyKey in e.tile.proxyRefs) {
					var proxyTile = e.tile.proxyRefs[proxyKey];
					if ((--proxyTile.proxyCount <= 0) && cache.unload(proxyKey)) {
						proxyTile.element.parentNode.removeChild(proxyTile.element);
					}
				}
			delete e.tile.proxyRefs;
			}
		}

		var layer = {
			cache : cache,
			map : function(x) {
				if (!arguments.length) return map;
				if (map) {
					if (map == x) {
						container.parentNode.insertBefore(container, map.featuresContainer);
						return layer;
					}
					map.off("move", move).off("resize", move);
					container.parentNode.removeChild(container);
				}
				map = x;
				if (map) {
					if (obj && obj.feature == true) {
						map.featuresContainer.appendChild(container);
					}
					else {
						map.container().insertBefore(container, map.featuresContainer);
					}
					if (layer.init) layer.init(container);
					map.on("move", move).on("resize", move);
					move();
				}
				return layer;
			},
			container : function() {
				return container;
			},
			id : function(x) {
				if (!arguments.length) return id;
				id = x;
				container.id = x;
				return layer;
			},
			visible : function(x) {
				if (!arguments.length) return visible;
				if (visible = x) container.removeAttributeNS(null, "visibility");
				else container.setAttributeNS(null, "visibility", "hidden");
				if (map) move();
				return layer;
			},
			transform : function(x) {
				if (!arguments.length) return transform;
				transform = x;
				if (map) move();
				return layer;
			},
			zoom : function(x) {
				if (!arguments.length) return zoom;
				zoom = typeof x == "function" || x == null ? x : function() { return x; };
				if (map) move();
				return layer;
			},
			tile : function(x) {
				if (!arguments.length) return tile;
				tile = x;
				if (map) move();
				return layer;
			},
			reload : function(s) {
				cache.clear();
				if (map) move();
				return layer;
			}
		};

		layer.dispatch = polymaps.dispatch(layer);
		layer.on("load", cleanup);
		return layer;
	},

//==========================================  IMAGE  ==============================================
	image : function(obj) {
		var image = polymaps.layer(load, unload, obj),
				url;

		function load(tile) {
			var size = image.map().tileSize();
			var element = tile.element = $C({
				element : "image",
				width : size.x,
				height : size.y,
				preserveAspectRatio : "none"
			});
			if (typeof url == "function") {
				tile.request = polymaps.queue.image(element, url(tile), function(img) {
						delete tile.request;
						tile.ready = true;
						tile.img = img;
						image.dispatch({type: "load", tile: tile});
				});
			}
			else {
				tile.ready = true;
				if (url) element.setAttributeNS(pergola.ns.xlink, "xlink:href", url);
				image.dispatch({type: "load", tile: tile});
			}
		}

		function unload(tile) {
			if (tile.request) tile.request.abort(true);
		}

		image.url = function(x) {
			if (!arguments.length) return url;
			url = typeof x == "string" && /{.}/.test(x) ? polymaps.url(x) : x;
			return image.reload();
		};

		return image;
	},

//==========================================  GEOJSON  ==============================================
	geoJson : function(obj, fetch) {
		var geoJson = polymaps.layer(load, unload, obj),
			container = geoJson.container(),
			url,
			clip = true,
			clipId = "".xmlNameGenerator(),
			clipHref = "url(#" + clipId + ")",
			clipPath = container.insertBefore($C({element : "clipPath", id : clipId}), container.firstChild),
			clipRect = $C({element : "rect", appendTo : clipPath}),
			scale = "auto",
			zoom = null,
			features;
		container.setAttributeNS(null, "fill-rule", "evenodd");
		if (!fetch) fetch = polymaps.queue.json;

		function projection(proj) {
			var l = {lat: 0, lon: 0};
			return function(coordinates) {
				l.lat = coordinates[1];
				l.lon = coordinates[0];
				var p = proj(l);
				coordinates.x = p.x;
				coordinates.y = p.y;
				return p;
			};
		}

		function geometry(o, proj) {
			return o && o.type in types && types[o.type](o, proj);
		}

		var types = {
			Point: function(o, proj) {
				var p = proj(o.coordinates),
						props = o.properties,
						elements = o.elements,
						transform = "translate(" + parseInt(p.x) + "," + parseInt(p.y) + ")" + (o.scale ? (" scale" + o.scale) : ""),
						g;

				if (elements) {
					if (elements.length > 1) {
						g = {element: "g", transform: transform};
						if (props && props.attributes) {
							for (var p in props.attributes) g[p] = props.attributes[p];
						}
						g = $C(g);
						for (var a in elements) {
							g.appendChild($C(elements[a]));
						}
						return g;
					}
					else {
						elements[0].transform = transform;
						return $C(elements[0])
					};
				}
				return $C({element : "circle", r : 5, transform : "translate(" + p.x + "," + p.y + ")", style: o.style});
			},
			MultiPoint: function(o, proj) {
				var g = $C({element : "g"}),
						c = o.coordinates,
						p, // proj(c[i])
						x, // svg:circle
						i = -1,
						n = c.length;
				while (++i < n) $C({element : "circle", r : 4.5, transform : "translate(" + (p = proj(c[i])).x + "," + p.y + ")", style: o.style, appendTo : g});
				return g;
			},
			LineString: function(o, proj) {
				var d = ["M"],
						c = o.coordinates,
						p, // proj(c[i])
						i = -1,
						n = c.length;
				while (++i < n) d.push((p = proj(c[i])).x, ",", p.y, "L");
				d.pop();
				if (!d.length) return;
				return $C({element : "path", d : d.join(""), style: o.style});
			},
			MultiLineString: function(o, proj) {
				var d = [],
						ci = o.coordinates,
						cj, // ci[i]
						i = -1,
						j,
						n = ci.length,
						m;
				while (++i < n) {
					cj = ci[i];
					j = -1;
					m = cj.length;
					d.push("M");
					while (++j < m) d.push((p = proj(cj[j])).x, ",", p.y, "L");
					d.pop();
				}
				if (!d.length) return;
				return $C({element : "path", d : d.join(""), style: o.style});
			},
			Polygon: function(o, proj) {
				var d = [],
						ci = o.coordinates,
						cj, // ci[i]
						i = -1,
						j,
						n = ci.length,
						m;
				while (++i < n) {
					cj = ci[i];
					j = -1;
					m = cj.length - 1;
					d.push("M");
					while (++j < m) d.push((p = proj(cj[j])).x, ",", p.y, "L");
					d[d.length - 1] = "Z";
				}
				if (!d.length) return;
				return $C({element : "path", d : d.join(""), style: o.style});
			},
			MultiPolygon: function(o, proj) {
				var d = [],
						ci = o.coordinates,
						cj, // ci[i]
						ck, // cj[j]
						i = -1,
						j,
						k,
						n = ci.length,
						m,
						l;
				while (++i < n) {
					cj = ci[i];
					j = -1;
					m = cj.length;
					while (++j < m) {
						ck = cj[j];
						k = -1;
						l = ck.length - 1;
						d.push("M");
						while (++k < l) d.push((p = proj(ck[k])).x, ",", p.y, "L");
						d[d.length - 1] = "Z";
					}
				}
				if (!d.length) return;
				return $C({element: "path", "d": d.join(""), style: o.style});
			},
			GeometryCollection: function(o, proj) {
				var g = $C({element : "g"}),
						i = -1,
						c = o.geometries,
						n = c.length,
						x;
				while (++i < n) {
					x = geometry(c[i], proj);
					if (x) g.appendChild(x);
				}
				return g;
				}
		};
		
		function rescale(o, e, k) {
			return o.type in rescales && rescales[o.type](o, e, k);
		}
		
		var rescales = {
			Point: function (o, e, k) {
				var p = o.coordinates;
				e.setAttributeNS(null, "transform", "translate(" + p.x + "," + p.y + ")" + k);
			},
			MultiPoint: function (o, e, k) {
				var c = o.coordinates,
					i = -1,
					n = p.length,
					x = e.firstChild,
					p;
				while (++i < n) {
					p = c[i];
					x.setAttributeNS(null, "transform", "translate(" + p.x + "," + p.y + ")" + k);
					x = x.nextSibling;
				}
			}
		};

		function load(tile, proj) {
			var g = tile.element = $C({element : "g"});
			tile.features = [];
			proj = projection(proj(tile).locationPoint);
	
			function update(data) {
				var updated = [];
/* Fetch the next batch of features, if so directed. */
				if (data.next) tile.request = fetch(data.next.href, update);
/* Convert the GeoJSON to SVG. */
				switch (data.type) {
					case "FeatureCollection": {
						for (var i = 0; i < data.features.length; i++) {
							var feature = data.features[i],
							element = geometry(feature.geometry, proj);
							if (element) updated.push({element: g.appendChild(element), data: feature});
						}
						break;
					}
					case "Feature": {
						var element = geometry(data.geometry, proj);
						if (element) updated.push({element: g.appendChild(element), data: data});
						break;
					}
					default: {
						var element = geometry(data, proj);
						if (element) updated.push({element: g.appendChild(element), data: {type: "Feature", geometry: data}});
						break;
					}
				}

				tile.ready = true;
				updated.push.apply(tile.features, updated);
				geoJson.dispatch({type: "load", tile: tile, features: updated});
			}
			
			if (url != null) {
				tile.request = fetch(typeof url == "function" ? url(tile) : url, update);
			}
			else {
				update({type: "FeatureCollection", features: features || []});
			}
		}

		function unload(tile) {
			if (tile.request) tile.request.abort(true);
		}

		function move() {
			var zoom = geoJson.map().zoom(),
					tiles = geoJson.cache.locks(), // visible tiles
					key, // key in locks
					tile, // locks[key]
					features, // tile.features
					i, // current feature index
					n, // current feature count, features.length
					feature, // features[i]
					k; // scale transform
			if (scale == "fixed") {
				for (key in tiles) {
					if ((tile = tiles[key]).scale != zoom) {
						k = "scale(" + Math.pow(2, tile.zoom - zoom) + ")";
						i = -1;
						n = (features = tile.features).length;
						while (++i < n) rescale((feature = features[i]).data.geometry, feature.element, k);
						tile.scale = zoom;
					}
				}
			}
			else {
				for (key in tiles) {
				i = -1;
				n = (features = (tile = tiles[key]).features).length;
				while (++i < n) rescale((feature = features[i]).data.geometry, feature.element, "");
				delete tile.scale;
				}
			}
		}

		geoJson.url = function(x) {
			if (!arguments.length) return url;
			url = typeof x == "string" && /{.}/.test(x) ? polymaps.url(x) : x;
			if (url != null) features = null;
			if (typeof url == "string") geoJson.tile(false);
			return geoJson.reload();
		};
		
		geoJson.features = function(x) {
			if (!arguments.length) return features;
			if (features = x) {
				url = null;
				geoJson.tile(false);
			}
			return geoJson.reload();
		};
		
		geoJson.clip = function(x) {
			if (!arguments.length) return clip;
			if (clip) container.removeChild(clipPath);
			if (clip = x) container.insertBefore(clipPath, container.firstChild);
			var locks = geoJson.cache.locks();
			for (var key in locks) {
				if (clip) locks[key].element.setAttributeNS(null, "clip-path", clipHref);
				else locks[key].element.removeAttributeNS(null, "clip-path");
			}
			return geoJson;
		};

		var __tile__ = geoJson.tile;
		geoJson.tile = function(x) {
			if (arguments.length && !x) geoJson.clip(x);
			return __tile__.apply(geoJson, arguments);
		};

		var __map__ = geoJson.map;
		geoJson.map = function(x) {
			if (x && clipRect) {
				var size = x.tileSize();
				clipRect.setAttributeNS(null, "width", size.x);
				clipRect.setAttributeNS(null, "height", size.y);
			}
			return __map__.apply(geoJson, arguments);
		};

		geoJson.scale = function(x) {
			if (!arguments.length) return scale;
			if (scale = x) geoJson.on("move", move);
			else geoJson.off("move", move);
			if (geoJson.map()) move();
			return geoJson;
		};

		geoJson.show = function(tile) {
			if (clip) tile.element.setAttributeNS(null, "clip-path", clipHref);
			else tile.element.removeAttributeNS(null, "clip-path");
			geoJson.dispatch({type: "show", tile: tile, features: tile.features});
			return geoJson;
		};

		geoJson.reshow = function() {
			var locks = geoJson.cache.locks();
			for (var key in locks) geoJson.show(locks[key]);
			return geoJson;
		};

		return geoJson;
	},

//==========================================  DRAG  ==============================================
	drag : function() {
		var drag = {},
				map,
				container,
				dragging,
				owner,
				statusBar,
				r;

		function mousedown(e) {
			if (e.shiftKey) return;
			var porg = polymaps.origin,
          offsetX = owner.ctm.e + porg.x,
          offsetY = owner.ctm.f + porg.y,
          point = pergola.mousePoint(e),
          m = {x : point.x - offsetX, y : point.y - offsetY};

			dragging = {x: e.clientX, y: e.clientY};
			owner.removePointerEvents([owner.container]); 
			r.setAttributeNS(null, "cursor", "move");
			r.addEventListener("mousemove", mousemove, false);
			r.addEventListener("mouseup", mouseup, false);
			r.addEventListener("mouseout", mouseup, false);
			if (statusBar) statusBar.location(map.pointLocation(m));
		}
		function mousemove(e) {
			map.panBy({x: e.clientX - dragging.x, y: e.clientY - dragging.y});
			if (statusBar) statusBar.center(map.center());
			dragging.x = e.clientX;
			dragging.y = e.clientY;
		}
		function mouseup(e) {
			if (!dragging) return;
			dragging = null;
			owner.setPointerEvents([owner.container]);
			r.removeEventListener("mousemove", mousemove, false);
			r.removeEventListener("mouseup", mouseup, false);
			r.removeEventListener("mouseout", mouseup, false);
			r.setAttributeNS(null, "cursor", "default");
		}
		drag.map = function(x) {
			if (!arguments.length) return map;
			if (map) {
				container.removeEventListener("mousedown", mousedown, false);
				container = null;
			}
			if (map = x) {
				container = map.container();
				owner = map.owner,
				statusBar = owner.owner.status,
				r = owner.owner.background.rect;
				container.addEventListener("mousedown", mousedown, false);
			}
			return drag;
		};

		return drag;
	},

//==========================================  WHEEL  ==============================================
	wheel : function() {
		var timePrev = 0,
				last = 0,
				smooth = true,
				zoom = "mouse",
				location,
				map,
				container,
        win = pergola.Window.active(),
        statusBar = win.status;

		function move(e) {
			location = null;
		}

		function mousewheel(e) {
			var delta = (e.wheelDelta / 120 || -e.detail) * .0333,
					point;
			
/* If smooth zooming is disabled, batch events into unit steps. */
			if (!smooth && delta) {
				var timeNow = Date.now();
				if (timeNow - timePrev > 200) {
					delta = delta > 0 ? +1 : -1;
					timePrev = timeNow;
				} else {
				delta = 0;
				}
			}

			if (delta) {
				switch (zoom) {
					case "mouse": {
						point = map.mouse(e);
						if (!location) location = map.pointLocation(point);
						map.off("move", move).zoomBy(delta, point, location).on("move", move);
						break;
					}
					case "location": {
						map.zoomBy(delta, map.locationPoint(location), location);
						break;
					}
					default: { // center
						map.zoomBy(delta);
						break;
					}
				}
        if (statusBar) statusBar.zoom(map.zoom());
			}
			e.preventDefault();
			return false; // for Firefox
		}

		var wheel = {
			smooth : function(x) {
			if (!arguments.length) return smooth;
				smooth = x;
				return wheel;
			},
			zoom : function(x, l) {
				if (!arguments.length) return zoom;
				zoom = x;
				location = l;
				if (map) {
					if (zoom == "mouse") map.on("move", move);
					else map.off("move", move);
				}
				return wheel;
			},
			map : function(x) {
				if (!arguments.length) return map;
				if (map) {
					container.removeEventListener("mousemove", move, false);
					container.removeEventListener("mousewheel", mousewheel, false);
					container.removeEventListener("DOMMouseScroll", mousewheel, false);
					container = null;
					map.off("move", move);
				}
				if (map = x) {
					if (zoom == "mouse") map.on("move", move);
					container = map.container();
					container.addEventListener("mousemove", move, false);
					container.addEventListener("mousewheel", mousewheel, false);
					container.addEventListener("DOMMouseScroll", mousewheel, false);
				}
				return wheel;
			}
		};

		return wheel;
	},

//==========================================  ARROW  ==============================================
	arrow : function() {
		var key = {left: 0, right: 0, up: 0, down: 0},
				last = 0,
				repeatTimer,
				repeatDelay = 250,
				repeatInterval = 50,
				speed = 16,
				map,
				parent,
        win = pergola.Window.active(),
        statusBar = win.status;

		function keydown(e) {
			if (e.shiftKey) {
				if (e.altKey) speed = 512;
				else speed = 128;
			}
			else speed = 16;
			var now = Date.now(), dx = 0, dy = 0;
			switch (e.keyCode) {
				case 37: {
					if (!key.left) {
						last = now;
						key.left = 1;
						if (!key.right) dx = speed;
					}
					break;
				}
				case 39: {
					if (!key.right) {
						last = now;
						key.right = 1;
						if (!key.left) dx = -speed;
					}
					break;
				}
				case 38: {
					if (!key.up) {
						last = now;
						key.up = 1;
						if (!key.down) dy = speed;
					}
					break;
				}
				case 40: {
					if (!key.down) {
						last = now;
						key.down = 1;
						if (!key.up) dy = -speed;
					}
					break;
				}
				default: return;
			}
			if (dx || dy) map.panBy({x: dx, y: dy});
			if (!repeatTimer && (key.left | key.right | key.up | key.down)) {
				repeatTimer = setInterval(repeat, repeatInterval);
			}
			e.preventDefault();
		}

		function keyup(e) {
			last = Date.now();
			switch (e.keyCode) {
				case 37: key.left = 0; break;
				case 39: key.right = 0; break;
				case 38: key.up = 0; break;
				case 40: key.down = 0; break;
				default: return;
			}
			if (repeatTimer && !(key.left | key.right | key.up | key.down)) {
				repeatTimer = clearInterval(repeatTimer);
				if (statusBar) statusBar.center(map.center());
			}
			e.preventDefault();
		}

		function keypress(e) {
			var z = map.zoom();
			switch (e.charCode) {
				case 45: case 95: map.zoom(Math.ceil(z) - 1); break;
				case 43: case 61: map.zoom(Math.floor(z) + 1); break;
				default: return;
			}
      if (statusBar) statusBar.zoom(map.zoom());
      win.mapZoomLevelMark();
			e.preventDefault();
		}

		function repeat() {
			if (!map) return;
			if (Date.now() < last + repeatDelay) return;
			var dx = (key.left - key.right) * speed,
			dy = (key.up - key.down) * speed;
			if (dx || dy) {
        map.panBy({x: dx, y: dy});
        if (statusBar) statusBar.center(map.center());
      }
		}

		var arrow = {
			map : function(x) {
				if (!arguments.length) return map;
				if (map) {
					parent.removeEventListener("keypress", keypress, false);
					parent.removeEventListener("keydown", keydown, false);
					parent.removeEventListener("keyup", keyup, false);
					parent = null;
				}
				if (map = x) {
					parent = map.focusableParent();
					parent.addEventListener("keypress", keypress, false);
					parent.addEventListener("keydown", keydown, false);
					parent.addEventListener("keyup", keyup, false);
				}
				return arrow;
			},
			speed : function(x) {
				if (!arguments.length) return speed;
				speed = x;
				return arrow;
			}
		};

		return arrow;
	},

//==========================================  HASH  ==============================================
	hash : function() {
		var s0, // cached location.hash
				lat = 90 - 1e-8, // allowable latitude range
				map;

		var parser = function(s) {
			var args = s.split("/").map(Number);
			if (args.length < 3 || args.some(isNaN)) move(); // replace bogus hash
			else {
				var size = map.size();
				map.zoomBy(args[0] - map.zoom(),
					{x: size.x / 2, y: size.y / 2},
					{lat: Math.min(lat, Math.max(-lat, args[1])), lon: args[2]});
			}
		};

		var formatter = function(map) {
			var center = map.center(),
					zoom = map.zoom(),
					precision = Math.max(0, Math.ceil(Math.log(zoom) / Math.LN2));
			return "#" + zoom.toFixed(2)
			+ "/" + center.lat.toFixed(precision)
			+ "/" + center.lon.toFixed(precision);
		};

		function move() {
			var s1 = formatter(map);
			if (s0 !== s1) location.replace(s0 = s1); // don't recenter the map!
		}

		function hashchange() {
			if (location.hash === s0) return; // ignore spurious hashchange events
			parser((s0 = location.hash).substring(1));
		}

		var hash = {
			map : function(x) {
				if (!arguments.length) return map;
				if (map) {
					map.off("move", move);
					window.removeEventListener("hashchange", hashchange, false);
				}
				if (map = x) {
					map.on("move", move);
					window.addEventListener("hashchange", hashchange, false);
					location.hash ? hashchange() : move();
				}
				return hash;
			},
			parser : function(x) {
				if (!arguments.length) return parser;
				parser = x;
				return hash;
			},
			formatter : function(x) {
				if (!arguments.length) return formatter;
				formatter = x;
				return hash;
			}
		};

		return hash;
	},
//==========================================  INTERACT  ==============================================
	interact : function() {
		var drag = polymaps.drag(),
				wheel = polymaps.wheel(),
				arrow = polymaps.arrow();

		var interact = {
			map : function(x) {
				drag.map(x);
				wheel.map(x);
				arrow.map(x);
				return interact;
			}
		};
		return interact;
	},

//==========================================  GRID  ==============================================
	grid : function() {
		var grid = {},
				map,
				win = pergola.Window.active(),
				size = win.map.size(),
				ctm = win.doc.ctm,
				g = $C({element: "g", fill: "#FFFF90", "font-style": "italic", "font-family": "'Times New Roman'", "font-size": "7pt", display: "none"}),
				ticksG = $C({element: "g", stroke: "#A4A4A4", appendTo: g}),
				lonG = $C({element: "g", transform: "translate(0 " + (-ctm.f) + ")", appendTo: g}),
				latG = $C({element: "g", transform: "translate(" + (-ctm.e) + " 0)", appendTo: g}),
				lonText = $C({element: "g", "text-anchor": "middle", appendTo: lonG}),
				latText = $C({element: "g", appendTo: latG});

				$C({element: "rect", fill: "url(#LonGutter)", opacity: .75, width: size.x, height: 19, appendTo: lonG});
				$C({element: "rect", fill: "url(#LatGutter)", opacity: .75, width: 19, height: size.y, appendTo: latG});

		function move(e) {
			var nw = map.pointLocation(polymaps.origin),
					zoom = map.zoom();

			nw.lon = parseInt(nw.lon); nw.lat = parseInt(nw.lat);
			ticksG.setAttributeNS(null, "stroke-opacity", (1 - (1 - (3.3 * Math.log(zoom + 1)) / 10) * (zoom < 20)).toFixed(2));
			while (ticksG.childNodes.length) ticksG.removeChild(ticksG.firstChild);
			while (lonText.childNodes.length) lonText.removeChild(lonText.firstChild);
			while (latText.childNodes.length) latText.removeChild(latText.firstChild);
			for (var x; (x = map.locationPoint(nw).x) <= size.x; nw.lon++) {
				if (nw.lon <= 180 && nw.lon >= -180) {
					var draw = !(zoom < 6 && nw.lon % parseInt(20 / zoom) != 0) | 0;
					if (draw) {
						$C({element: "path", d: ("M" + (Math.ceil(x) - .5) + ",0v" + size.y), appendTo: ticksG});
						$C({element: "text", x: x, y: 11, textNode: nw.lon, appendTo: lonText});
					}
				}
			}
			for (var y; (y = map.locationPoint(nw).y) <= size.y; nw.lat--) {
				draw = !(zoom < 6 && nw.lat % parseInt(20 / zoom) != 0) | 0;
				if (draw) {
					$C({element: "path", d: ("M0 " + (Math.ceil(y) - .5) + "h" + size.x), appendTo: ticksG});
					$C({element: "text", x: 1, y: y + 3, textNode: nw.lat, appendTo: latText});
				}
			}
		}

		grid.map = function(x) {
			if (!arguments.length) return map;
			if (map) {
				g.parentNode.removeChild(g);
				map.off("move", move).off("resize", move);
			}
			if (map = x) {
				map.grid = g, map.lonG = lonG, map.latG = latG;	
				map.on("move", move).on("resize", move);
				map.container().appendChild(g);
				map.dispatch({type: "move"});
			}
			return grid;
		};

		return grid;
	},
};


pergola.extend(polymaps.map, {
	locationCoordinate : function(l) {
		var k = 1 / 360;
		return {
			column: (l.lon + 180) * k,
			row: (180 - polymaps.lat2y(l.lat)) * k,
			zoom: 0
		};
	},
	coordinateLocation : function(c) {
		var k = 45 / Math.pow(2, c.zoom - 3);
		return {
			lon: k * c.column - 180,
			lat: polymaps.y2lat(180 - k * c.row)
		};
	}
});

pergola.extend(polymaps.resizer, {
	maps : [],
	add : function(map) {
		for (var i = 0; i < this.maps.length; i++) {
			if (this.maps[i] == map) return;
		}
		this.maps.push(map);
	},
	remove : function(map) {
		for (var i = 0; i < this.maps.length; i++) {
			if (this.maps[i] == map) {
			this.maps.splice(i, 1);
			return;
			}
		}
	}
});


// https://bugs.webkit.org/show_bug.cgi?id=44083
var bug44083 = /WebKit/.test(navigator.userAgent) ? -1 : 0;
// https://bugs.webkit.org/show_bug.cgi?id=40441
var bug40441 = /WebKit\/533/.test(navigator.userAgent) ? -1 : 0;


