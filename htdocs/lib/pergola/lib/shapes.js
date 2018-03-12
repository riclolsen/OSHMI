
/*
 * PERGOLA - JAVASCRIPT SVG FRAMEWORK AND LIBRARIES
 *  
 * SHAPES LIBRARY 
 * 
 * Copyright (C) 2009-2013  Dotuscomus
 * info@dotuscomus.com
 * http://www.dotuscomus.com/pergola/licenses
*/



pergola.shapes = {
/*
  triangle : {
  	element : "path",
		geometry : {
			d : "M0,32h32l-16-27.7128z"
		}
	},
*/
  triangle : {
  	element : "path",
		d : "M0,32h32l-16-27.7128z"
	},

	pentagon : {
		element : "polygon",
		points : "5.812,28.944 0,11.056 15.217,0 30.434,11.056 24.622,28.944"
	},
	hexagon : {
		element : "polygon",
		points : "8,27.713 0,13.856 8,0 24,0 32,13.856 24,27.713"
	},
	heptagon : {
		element : "polygon",
		points : "8.657,30.416 0,19.561 3.089,6.024 15.599,0 28.108,6.024 31.197,19.561 22.541,30.416"
	},
	octagon : {
		element : "polygon",
		points : "8.659,29.564 0,20.905 0,8.659 8.659,0 20.905,0 29.564,8.659 29.564,20.905 20.905,29.564"
	},
	nonagon : {
		element : "polygon",
		points : "10.285,31.035 1.9,24 0,13.222 5.472,3.743 15.757,0 26.042,3.743 31.514,13.222 29.613,24 21.229,31.035"
	},
	decagon : {
		element : "polygon",
		points : "11.056,30.434 3.056,24.621 0,15.217 3.056,5.813 11.056,0 20.944,0 28.944,5.813 32,15.217 28.944,24.621 20.944,30.434"
	},
	dodecagon : {
		element : "polygon",
		points : "11.313,30.909 4.141,26.769 0,19.596 0,11.313 4.141,4.141 11.313,0 19.596,0 26.768,4.141 30.909,11.313 30.909,19.596 26.768,26.769 19.596,30.909"
	},
	rhombus : {
  	element : "polygon",
		points : "12,32 0,16 12,0 24,16"
	},
	semicircle : {
  	element : "path",
		d : "M16,32C7.164,32,0,24.836,0,16S7.164,0,16,0V32z"
	},
	star : {
		element : "polygon",
		points : "24,0 29.848,18.265 48,18.265 33.479,29.848 38.722,48 24,37.168 9.278,48 14.521,29.848 0,18.265 18.265,18.265"
	},
	heart : {
  	element : "path",
		d : "M23.5,42.5c7.7-11.49,24-20.91,24-30s-6.48-12-12-12s-10.01,2.71-12,7c-1.91-3.98-5.77-7-12-7s-12,2.98-12,12S16.65,30.87,23.5,42z"
	},
	pied_de_poule : {
  	element : "path",
		d : "M2,0h2v2h6v2h-2v6h-2v-2h-6v-2h2z"
	}

};




