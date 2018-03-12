
if(document.all || document.getElementsByTagName) {

	document.getElementsByClassName = function (el) {

		var searchObj, nodeList;

		// initialize
		searchObj = n = new Object();
		for (var i = 1; i < arguments.length; i++) {
			searchObj[arguments[i]] = new Array();
		}

		nodeList = el.all || el.getElementsByTagName("*");

		// loop over all descendant elements (c)
		for (var i = 0, c = null, cn; (el.all ? c = nodeList(i) : c = nodeList.item(i)); i++) {
			if (c.nodeType == 1) {
				cn = c.className.split(" ");
				for (j = 0; j < cn.length; j++) {

					// if the current cn element is defined in searchObj, 
					// then it must be one we're searching for. add it.
					if (searchObj[cn[j]]) {
						searchObj[cn[j]][searchObj[cn[j]].length] = c;
					}
				}
			}
		}

		return ( searchObj );
	}
}

if(!Array.prototype.push) {

	Array.prototype.push =  function( e ) {
		var i;
		for(i=0; j=arguments[i]; i++) this[this.length] = j;
		return ( this.length );
	}
}