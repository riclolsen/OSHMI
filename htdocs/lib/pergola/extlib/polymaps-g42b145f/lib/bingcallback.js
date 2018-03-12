
var bingData,
		bingCopyright,
		script;


function renderBing() {
	var win = pergola.Window.active(),
	resourceSets = bingData.resourceSets;

	for (var i = 0; i < resourceSets.length; i++) {
		var resources = resourceSets[i].resources;
		for (var j = 0; j < resources.length; j++) {
			var resource = resources[j];
			win.map.add(polymaps.image(win.currentView)
				.url(template(resource.imageUrl, resource.imageUrlSubdomains)))
				.tileSize({x: resource.imageWidth, y: resource.imageHeight});
		}
	}

	$C({element: "image", id: "bingLogo", x: 7, y: -18, "xlink:href": bingData.brandLogoUri, width: "64", height: "64", appendTo: win.topBar.container});

	var a = bingData.copyright.split(" ");
	var lines = [];
	for (var i = 4; i > 1; i--) lines.push(a.splice(0, parseInt(a.length / i + (2 - (i == 4)))).join(" "));
	lines.push(a.splice(0).join(" "));

	var doc = win.doc;
	if (!doc.copyright) {
		doc.copyright = $C({element: "g", transform: "translate(13 " + (doc.visibleH - 54) + ")", "pointer-events": "none", appendTo: doc.container});
		$C({element: "rect", x: 2.5, y: .5, rx: 4, width: 254, height: 50, fill: "black", opacity: .4, appendTo: doc.copyright});
		var text = $C({element: "text", "font-size": "6.5pt", fill: "#EFEFEF", appendTo: doc.copyright});
		for (var i = 0; i < lines.length; i++) {
			$C({element: "tspan", x: 5, dy: "8.5pt", textNode: lines[i], appendTo: text});
		}
	}

// Viewport dimensions for window of type "map" don't change.
// Updating viewport at every content change is obnoxious.
// Disable after map and layers are appended.
	doc.unregisterEvents(doc.transformable, ["DOMNodeInserted", "DOMNodeRemoved"]);

};


function tileSource() {
	var win = pergola.Window.active(),
			container = win.map.container();

	if (win.currentView == win.views[this.view]) return;

	container.removeChild(container.firstChild);
	win.currentView = win.views[this.view];
	pergola.defs.removeChild(script);
	queryBing(this.view);
}



function bingCallback(data) {
	bingData = data;
  renderBing();
}


// Returns a Bing URL template given a string and a list of subdomains.
function template(url, subdomains) {
	var n = subdomains.length,
	salt = ~~(Math.random() * n); // per-session salt
// Returns the given coordinate formatted as a 'quadkey'.
	function quad(column, row, zoom) {
		var key = "";
		for (var i = 1; i <= zoom; i++) {
			key += (((row >> zoom - i) & 1) << 1) | ((column >> zoom - i) & 1);
		}
		return key;
	}
	return function(c) {
		var quadKey = quad(c.column, c.row, c.zoom),
		server = Math.abs(salt + c.column + c.row + c.zoom) % n;
		return url
		.replace("{quadkey}", quadKey)
		.replace("{subdomain}", subdomains[server]);
	};
}


function queryBing(view) {
	view = view || "aerialWithLabels";
	script = $C({
		element : "script",
		type : "text/javascript", 
		"xlink:href" : "http://dev.virtualearth.net"
			+ "/REST/V1/Imagery/Metadata/" + view
			+ "?key=AoQhfBJhkyflrvYyCnfMkNWqHws6AAgZKYXPLK4ME1oRQqdygndkTDDrLjAQoM32"
			+ "&jsonp=bingCallback",
		appendTo: pergola.defs
	});
}

