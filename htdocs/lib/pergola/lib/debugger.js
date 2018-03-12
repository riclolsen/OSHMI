
//=====================================   MINI DEBUGGER   ================================


// USE
/*

$D({
	"myVar" : myVar, 
	"this" : this, 
	"evt.target" : evt.target
});

*/



pergola.debugger = function () {
	var d = pergola.debugger,
			thumb,
			box,
			g,
      lines = (typeof debug == "boolean") * 20 || debug;

	d.toggleOn = function (evt) {
		thumb.setAttributeNS(null, "display", "none");
		box.setAttributeNS(null, "display", "block");
	};

	d.toggleOff = function (evt) {
		box.setAttributeNS(null, "display", "none");
		thumb.setAttributeNS(null, "display", "block");
	};

  d.vars = function (input) {
		var p = [],
				i = 0;
	  for (var prop in input) p.push(prop);
	  if (p.length > lines) {
			alert(pergola.messages.debuggerLines[0] + lines + pergola.messages.debuggerLines[1]);
			return;
		}
		for (var a in input) {
	    var output = (input[a] == undefined) ? a + " is undefined" : a + " = " + input[a];
	    d["var" + i++].firstChild.data = output;
	  }
	};


  g = pergola.debug = $C({
		element : "g",
		id : "debugger",
		transform : "translate(0 54)",
		appendTo : pergola.systemComponents
	});

  thumb = $C({
		element : "g",
		appendTo : g
	});
  thumb.addEventListener("click", d.toggleOn, false);

	$C({
		element : "circle",
		cx : 4,
		cy : -6,
		r : 12,
		fill : "#FFFFFF",
		stroke : "#505090",
		'stroke-width' : 4,
		appendTo : thumb
	});

	$C({
		element : "text",
		'font-size' : 16,
		'font-weight' : "bold",
		fill : "firebrick",
		'pointer-events' : "none",
		textNode : "D",
		appendTo : thumb
	});

  box = $C({
		element : "g",
		fill : "firebrick",
		display : "none",
		appendTo : g
	});
	box.addEventListener("click", d.toggleOff, false);

  $C({
		element : "rect",
		x : 2,
		y : -20,
		rx : 4,
		width : 360,
		height : lines * 20 + 40,
		fill : "#F3F3F0",
		stroke : "gray",
		'stroke-width' : 4,
		opacity : .4,
		'pointer-events' : "none",
		appendTo : box
	});

  $C({
		element : "circle",
		cx : 16,
		cy : -6.5,
		r : 7,
		fill : "#505090",
		appendTo : box
	});

	$C({
		element : "path",
		d : "M11.5 -6.5h9",
		stroke : "#FFFFFF",
		'pointer-events' : "none",
		appendTo : box
	});

	$C({
		element : "text",
		x : 180,
		'font-family' : "Arial",
		'font-size' : 14,
		'text-anchor' : "middle",
		'pointer-events' : "none",
		textNode : "MINI DEBUGGER",
		appendTo : box
	});

	g = $C({
		element : "g",
		transform : "translate(10 20)",
		'font-family' : "'Courier New'",
		'font-size' : "10pt",
		'font-weight' : "bold",
		'pointer-events' : "none",
		appendTo : box
	});

  for (var i = 0; i < lines; i ++) {
		d["var" + i] = $C({
			element : "text",
			y : (i * 20),
			textNode : "\u00A0",
			appendTo : g
		});
	}

	$D = d.vars;
};



