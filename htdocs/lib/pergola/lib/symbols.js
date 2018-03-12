
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *  
 * SYMBOLS LIBRARY 
 * 
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/



pergola.symbols = function () {
  var shade = pergola.color.shade,
  		dark = (pergola.themeLuminosity < 128),
			cross = pergola.marker.cross(),
			dot = pergola.marker.dot({radius : 8, fill : "#60A0FF", stroke : "#FF6000"});;

	return {

    lens : [
      {element : "path", d : "M4.5 3.4l6.5,5", stroke : "#323232", "stroke-width" : 1.25, "stroke-linecap" : "round"},
      {element : "circle", r : 5.75, fill : "#FFFFFF", stroke : "#727272", "stroke-width" : 1.5, "fill-opacity" : .6}
    ],

    zoomMinus : [
      {element : "path", d : "M0 .5h13", stroke : dark? "#F0F0F0" : "#202020", 'stroke-linecap' : "round"}
    ],

    zoomPlus : [
      {element : "path", d : "M0 .5h13M6.5 -6v13", fill : "none", stroke : dark? "#F0F0F0" : "#202020", 'stroke-linecap' : "round"}
    ],

    hand : [
      {element : "path", d : "M0,11l-3-4l1.5-2l3,2v-8h3v5v-7h3v7v-6h3v6v-4h3v10", fill : "url(#handGrad)", stroke : "black", "stroke-width" : .45, "stroke-linejoin" :  "round", "stroke-linecap" : "round"}
    ],

    ruler : [
      {element : "path", d : "M0,10.56L3.27,15.59L19.54,5.03L16.27,0z", fill : "#FFFFFF", stroke : "#000000", "stroke-width" : .5},
      {element : "path", d : "M1.98,13.15L3.34,15.25L19.2,4.96L17.83,2.86z", fill : "#C8C8C8"},
      {element : "path", d : "M5.78,10.66l-1.5-2.31 M2.19,9.72l1.23,1.88 M7.61,8.88L6.38,6.99M10.58,4.27l1.23,1.89 M16,3.43l-1.23-1.89 M8.48,5.63l1.5,2.31 M12.67,2.91l1.5,2.31", fill : "none", stroke : "#000000", "stroke-width" : .5}
    ],

    measure : [
      {element : "path", d : "M-6,22h32", stroke : "#00D0FF", "stroke-width" : 2, "marker-start" : cross, "marker-end" : cross, transform : "rotate(-30) scale(.5)"}
    ],

    itinerary : [
      {element : "path", d : "M2,8L7,0L14,14L18,3", fill : "none", stroke : "#FF0000", "stroke-width" : .5, "stroke-dasharray" : "3,1", "marker-start" : dot, "marker-mid" : dot, "marker-end" : dot}
    ],

    winMinimize : [
      {element : "path", d : "M1,1h6", fill : "none", stroke : dark? "#F0F0F0"  : "#707070", 'stroke-width' : 2, 'stroke-linecap' : "round"}
    ],

    winFull : [
      {element : "rect", x : .5, y : .5, width : 7, height : 7, fill : "none", stroke : dark? "#F0F0F0" : "#707070"},
      {element : "path", d : "M1 1h6", fill : "none", stroke : dark? "#F0F0F0"  : "#707070", 'stroke-width' : 2, 'stroke-linecap' : "square"}
    ],

    winClose : [
      {element : "path", d : "M0,0l6,6M0,6l6-6", fill : "none", stroke : dark? "#FFFFFF" : "#585858", 'stroke-width' : 1.5, 'stroke-linecap' : "round"}
    ],

    resize : [
      {element : "path", d : "M0,0l6,-6 a8,8 0 0,1 -12,12z", fill : "url(#resizeGrad)"}
    ],

    checkmark : [
      {element : "path", d : "M0,4l3,4l6-8h-1l-5.5,6l-1.5-2z", fill : "#000000"}
    ],

    arrow : {

    	tiny : {

				up : [
		      {element : "path", d : "M0,3h5l-2.5,-3z", fill : dark ? "#FFFFFF" : "#282828"}
		    ],

		    down : [
		      {element : "path", d : "M0,0h5l-2.5,3z", fill : dark ? "#FFFFFF" : "#282828"}
		    ],

		    left : [
		      {element : "path", d : "M0,3.5l6-3.5v7z", fill : dark ? "#FFFFFF" : "#282828"}
		    ],

		    right : [
		      {element : "path", d : "M0,0v7l6-3.5z", fill : dark ? "#FFFFFF" : "#282828"}
		    ]
			},

			small : {

		    up : [
		      {element : "path", d : "M0,6h7l-3.5,-6z", fill : shade([100, 100, 100], -24)}
		    ],

		    down : [
		      {element : "path", d : "M0,1h7l-3.5,6z", fill : shade([100, 100, 100], -24)}
		    ],

		    left : [
		      {element : "path", d : "M0,3.5l6-3.5v7z", fill : shade([100, 100, 100], -24)}
		    ],

		    right : [
		      {element : "path", d : "M0,0v7l6-3.5z", fill : shade([100, 100, 100], -24)}
		    ]
	  	},

		  large : {

		    up : [
		      {element : "path", d : "M0,8h10l-5,-8.8z", fill : shade([100, 100, 100], -28)}
		    ],

		    down : [
		      {element : "path", d : "M0,0h10l-5,8.8z", fill : shade([100, 100, 100], -28)}
		    ],

		    left : [
		      {element : "path", d : "M-1,5l8.8-5v10z", fill : shade([100, 100, 100], -28)}
		    ],

		    right : [
		      {element : "path", d : "M0,0v10l8.8-5z", fill : shade([100, 100, 100], -28)}
		    ]
		  }
		},

    dot : {

			large : [
	      {element : "circle", cx : 4.5, cy : 4.5, r : 4.5, fill : shade([100, 100, 100], -28)}
	    ],

	    small : [
	      {element : "circle", cx : 2.5, cy : 2.5, r : 2.2, fill : shade([100, 100, 100], -28)}
	    ]
	  },

    menuArrow : [
			{element : "path", d : "M0,0l3.5,3.5l-3.5,3.5z", fill : "black"}
		],

		selectorArrow : {

			up : [
        {element : "path", d : "M0 0l4 4h-8z", fill : "#6B6E6B"}
      ],

      down : [
        {element : "path", d : "M0 0l4 -4h-8z", fill : "#6B6E6B"}
      ]
		},

    question : [
			{element : "text", "font-size" : "13pt", "font-family" : "Verdana", "font-weight" : "bold", fill : "#606060", stroke : "white", "stroke-width" : .75, textNode : "?"}
		],

		leg : [
      {element : "polyline", fill : "#FFFFFF", stroke : "#404040", points : "-1.5,-14.5 -1.5,-.5 1.5,-.5 1.5,-14.5"},
      {element : "circle", fill : "white", "fill-opacity" : .65, stroke : "url(#leg_diskGrad)", "stroke-width" : 5, cx : 0, cy : -24, r : 7}
    ],

		signalPaddle : [
      {element : "polyline", fill : "url(#chiaroscuro)", stroke : "#A8A8A8", points : "-2.5,-22 -2.5,.5 2.5,0.5 2.5,-22 "},
      {element : "circle", fill : "none", stroke : "url(#paddleGrad)", "stroke-width" : 6, cx : 0, cy : -29, r : 8},
      {element : "circle", fill : "none", stroke : "url(#paddleReflection)", "stroke-width" : 1.25, cx : -.25, cy : -29.25, r : 9}
    ],

    pushpin_red : [
      {element : "polyline", fill : "url(#chrome)", points : "-2,-30 -2,-8.5 -1.2,.5 1.2,.5 2,-8.5 2,-30 "},
      {element : "circle", fill : "none", stroke : "url(#pushpinGrad_red)", "stroke-width" : 6, cx : 0, cy : -37, r : 8},
      {element : "circle", fill : "none", stroke : "url(#pinReflection_red)", "stroke-width" : 1.25, cx : -.25, cy : -37.25, r : 9}
    ],

    pushpin_green : [
      {element : "polyline", fill : "url(#chrome)", points : "-2,-30 -2,-8.5 -1.2,.5 1.2,.5 2,-8.5 2,-30 "},
      {element : "circle", fill : "none", stroke : "url(#pushpinGrad_green)", "stroke-width" : 6, cx : 0, cy : -37, r : 8},
      {element : "circle", fill : "none", stroke : "url(#pinReflection_green)", "stroke-width" : 1.25, cx : -.25, cy : -37.25, r : 9}
    ],

    pushpin_blue : [
      {element : "polyline", fill : "url(#chrome)", points : "-2,-30 -2,-8.5 -1.2,.5 1.2,.5 2,-8.5 2,-30 "},
      {element : "circle", fill : "none", stroke : "url(#pushpinGrad_blue)", "stroke-width" : 6, cx : 0, cy : -37, r : 8},
      {element : "circle", fill : "none", stroke : "url(#pinReflection_blue)", "stroke-width" : 1.25, cx : -.25, cy : -37.25, r : 9}
    ],

		banana : [
			{element : "path", fill : "#FADC74", d : "M3.8,12.7L4.6,15l4.2,5.1l9,4.4l10.5,0.7l8.6-1.3l7-4.6l3.9-5l1.3-5.7l3.1-8.4c0,0-0.5-0.3-1.1-0.2c-0.9,3.5-1.7,5-3.5,8C41.7,11.4,35.8,18.4,27,18.5C17.3,18.7,8.7,14.6,4.6,10C3.3,9.3,2.8,9.9,3.8,12.7z"},
			{element : "path", fill : "#E8B84D", d : "M27.6,24.2c5.8-0.1,10.4-1.8,14.9-5c4.4-3.4,5.8-7.5,5.8-10.9c2-3.3,3.9-8.1,3.9-8.1c0.9-0.2,1.5,0.2,1.5,0.2s1.4,0.6,2,0.5c-1.8,2.5-4.3,6.4-4.3,9.2s1.5,4.4-0.4,8.3C48,24,40,31.2,26,31.2c-8.8,0-13.5-2-17.6-5.2C4.7,23.1,1.8,18.9,1,14.9c0-4,1-4.6,2.8-2.2c1,1.9,3,4.4,5.8,6.7C14.2,22.6,19.9,24.3,27.6,24.2z"},
			{element : "path", fill : "#C89943", d : "M55.8,0.8c-1.6,2.3-4.4,6.5-4.4,9.3s1.5,4.4-0.4,8.3C48,24,42.1,28,36.2,29.8c2.1-2.1,7.6-4.3,7.6-7.1c0,0-2.1,1.5-3.8,1.1c6.5-3.7,9.4-8.3,9.7-13.6c0.2-3.1,1.6-6.4,4.9-8.9c-0.2-0.4-0.8-0.3-0.9-0.9C53.8,0.3,54.6,0.2,55.8,0.8z"},
			{element : "path", fill : "#B2BC4B", d : "M4.6,10C4,9,3.1,8,2.8,8c0,0-1.1,1.1-2.8,2c0,0.6,0.6,2,1,4.9c0-3.9,1-4.6,2.8-2.2C2.9,9.9,3.3,9.4,4.6,10z"},
			{element : "path", fill : "#537F37", d : "M4.3,9.6C3.7,8.7,3,8,2.8,8c0,0-1.1,1.1-2.8,2c0,0.4,0.6,2.4,0.8,3.5c-0.2-3.5,0.8-4,2.4-1.9C1.9,8.8,2.9,8.6,4.3,9.6z"},
			{element : "path", fill : "#4C4822", d : "M2.8,8C2.1,7.9,1.9,8.8,1.4,9S-0.1,9.6,0,10c0.3,0.2,1.1-0.1,1.7-0.7C2.4,8.7,3,8.2,2.8,8z"}
    ],

    transition : [
			{element : "path", d : "M0,.5h10", fill : "none", stroke : "#808090", "stroke-width" : 2, "stroke-linecap" : "round", "marker-end" : pergola.marker.chevron()}
		]

  };
};






