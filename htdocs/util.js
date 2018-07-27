// Rotinas utilitárias diversas
//
// OSHMI/Open Substation HMI - Copyright 2008-2018 - Ricardo L. Olsen

// http://paulirish.com/2011/requestanimationframe-for-smart-animating/
// http://my.opera.com/emoller/blog/2011/12/20/requestanimationframe-for-smart-er-animating
// requestAnimationFrame polyfill by Erik Möller
// fixes from Paul Irish and Tino Zijdel
(function() {
    var lastTime = 0;
    var vendors = ['ms', 'moz', 'webkit', 'o'];
    for(var x = 0; x < vendors.length && !window.requestAnimationFrame; ++x) {
        window.requestAnimationFrame = window[vendors[x]+'RequestAnimationFrame'];
        window.cancelAnimationFrame = window[vendors[x]+'CancelAnimationFrame']
                                   || window[vendors[x]+'CancelRequestAnimationFrame'];
    }
 
    if (!window.requestAnimationFrame)
        window.requestAnimationFrame = function(callback, element) {
            var currTime = new Date().getTime();
            var timeToCall = Math.max(0, 16 - (currTime - lastTime));
            var id = window.setTimeout(function() { callback(currTime + timeToCall); },
              timeToCall);
            lastTime = currTime + timeToCall;
            return id;
        };
 
    if (!window.cancelAnimationFrame)
        window.cancelAnimationFrame = function(id) {
            clearTimeout(id);
        };
}());

// ----------------------------------------------------------------------------------------
// Testa se storage API está disponível
function storageAvailable(type) {
	try {
		var storage = window[type],
			x = '__storage_test__';
		storage.setItem(x, x);
		storage.removeItem(x);
		return true;
	}
	catch(e) {
		return false;
	}
}

// ----------------------------------------------------------------------------------------
// extrai parâmetros da URL
function gup( name )
{  
name = name.replace(/[\[]/,"\\\[").replace(/[\]]/,"\\\]");  
var regexS = "[\\?&]" + name + "=([^&#]*)";  
var regex = new RegExp( regexS );  
var results = regex.exec( window.location.href );  
if( results === null )    
  {
  return "";
  }    
return results[1];
}

// ----------------------------------------------------------------------------------------
// retorna data na forma "YYYY-MM-DD"
function dateToYMDString( dt )
{
  return printf( "%04d-%02d-%02d", dt.getFullYear(), dt.getMonth() + 1, dt.getDate() );
}

// ----------------------------------------------------------------------------------------
function LoadFavicon( href )
{
  // Define o favicon da aplicação
  var link = document.createElement('link');
  link.type = 'image/x-icon';
  link.rel = 'shortcut icon';
  link.href = href;
  document.getElementsByTagName('head')[0].appendChild(link);
}

// ----------------------------------------------------------------------------------------
function MostraEsconde( objeto )
{
if ( objeto.style.display === "none" )
  {
  objeto.style.display = "";
  }
else
  {
  objeto.style.display = "none";
  }
}

// ----------------------------------------------------------------------------------------
function selectElementContents(el) {
  var body = document.body, range, sel;

  if (document.createRange && window.getSelection) {
      range = document.createRange();
      sel = window.getSelection();
      sel.removeAllRanges();
      try {
          range.selectNodeContents(el);
          sel.addRange(range);
      } catch (e) {
          range.selectNode(el);
          sel.addRange(range);
      }
  } else if (body.createTextRange) {
      range = body.createTextRange();
      range.moveToElementText(el);
      range.select();
  }
}

// ----------------------------------------------------------------------------------------
// copy to clipboard
function CopyClipboard( theField )
{
  var obj, notshown=false, nouserselect=false;
  if (typeof theField === 'string')
    obj = document.getElementById(theField);
  else
    obj = theField;  

  if ( typeof document.execCommand === 'function' )
    {
    if ( document.body.style.hasOwnProperty("userSelect") )
    if ( document.body.style.userSelect === "none" )
      {
        nouserselect=true;
        document.body.style.userSelect = "";
      }
    
        if (obj.style.display==='none') 
      {
        obj.style.display='';
        notshown=true;
      }
      
    if (obj.tagName === "TABLE")
      {
        selectElementContents(obj);
      }
    else  
      {
      obj.focus(); 
      obj.select(); 
      } 
    document.execCommand('copy');
    window.getSelection().removeAllRanges();
    if ( nouserselect )
      document.body.style.userSelect = "none";
    if (notshown)
      obj.style.display='none';
    }
}

// ----------------------------------------------------------------------------------------
// copy to clipboard (only works in IE)
function CpyClip( theField )
{
  //  var tempval=eval("document."+theField);
  var tempval = eval( theField );
  tempval.style.display = "";
  tempval.focus();
  tempval.select();
  if ( document.all )
    {
    var therange = tempval.createTextRange();
    therange.execCommand("Copy");
    window.status = "Dados copiados para a área de transferência!";
    setTimeout( "window.status=''", 2400 );
    }
  tempval.style.display = "none";
}

// ----------------------------------------------------------------------------------------
function SelHoje( objeto )
{
var dt = new Date();
var ano = dt.getYear();
if ( ano < 500 )
  {
    ano = ano - 100 + 2000;
  }
objeto.value = dt.getDate() + "/" + ( dt.getMonth() + 1 ) + "/" + ano;
}

// ----------------------------------------------------------------------------------------
function ObtemPonto( objeto )
{
 var url = "../comum/obtem_nponto.php";
 var janela = "_blank";
 window.objeto = objeto;
 window.open(url,janela,"toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,top=20,left=20,screenY=500,screenX=680,width=600,height=350");
}

// ----------------------------------------------------------------------------------------
function ObtemPontoAnaParaMVA( objeto )
{
 var url = "../comum/obtem_nponto.php?tpPto=A&Tipos=6,7";
 var janela = "_blank";
 window.objeto = objeto;
 window.open(url,janela,"toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,top=20,left=20,screenY=500,screenX=680,width=600,height=350");
}

// ----------------------------------------------------------------------------------------
function ObtemPontoAna( objeto )
{
 var url = "../comum/obtem_nponto.php?tpPto=A";
 var janela = "_blank";
 window.objeto = objeto;
 window.open(url,janela,"toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,top=20,left=20,screenY=500,screenX=680,width=600,height=350");
}

// ----------------------------------------------------------------------------------------
function ObtemPontoDig( objeto )
{
 var tipo = '';	
 var url = "../comum/obtem_nponto.php?tpPto=D";
 var janela = "_blank";
 window.objeto = objeto;
 window.open(url,janela,"toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,top=20,left=20,screenY=500,screenX=680,width=600,height=350");
}

// ----------------------------------------------------------------------------------------
function ToggleTreeNode( node )
{
	// Unfold the branch if it isn't visible
	if ( node.nextSibling.style.display === 'none' )
	{
		// Change the image (if there is an image)
		if ( node.childNodes.length > 0 )
		{
			if (node.childNodes.item(0).nodeName === "IMG")
			{
				node.childNodes.item(0).src = "../comum/minus.gif";
			}
		}

		node.nextSibling.style.display = 'block';
	}
	// Collapse the branch if it IS visible
	else
	{
		// Change the image (if there is an image)
		if ( node.childNodes.length > 0 )
		{
			if ( node.childNodes.item(0).nodeName === "IMG" )
			{
				node.childNodes.item(0).src = "../comum/plus.gif";
			}
		}

		node.nextSibling.style.display = 'none';
	}
}

// ----------------------------------------------------------------------------------------
function leftTrim( sString ) 
{
if ( sString == undefined )
  {
  return "";
  }
while ( sString.charAt(0) == ' ' || sString.charCodeAt(0) == 10 )
  { 
    sString = sString.substring(1, sString.length);
  }
return sString;
}

// from http://www.somacon.com/p355.php

String.prototype.trim = function() {
	return this.replace(/^\s+|\s+$/g,"");
};
String.prototype.ltrim = function() {
	return this.replace(/^\s+/,"");
};
String.prototype.rtrim = function() {
	return this.replace(/\s+$/,"");
};

// ----------------------------------------------------------------------------------------
function isInt( str )
{
	var i = parseInt (str);

	if ( isNaN (i) )
    {
      return false;
    }

	i = i.toString();
	if ( i != str )
	{
      return false;
    }

	return true;
}

// ----------------------------------------------------------------------------------------

/* Function printf(format_string,arguments...)
 * Javascript emulation of the C printf function (modifiers and argument types 
 *    "p" and "n" are not supported due to language restrictions)
 *
 * Copyright 2003 K&L Productions. All rights reserved
 * http://www.klproductions.com 
 *
 * Terms of use: This function can be used free of charge IF this header is not
 *               modified and remains with the function code.
 * 
 * Legal: Use this code at your own risk. K&L Productions assumes NO resposibility
 *        for anything.
 ********************************************************************************/
function printf(fstring)
  { var pad = function(str,ch,len)
      { var ps='';
        for(var i=0; i<Math.abs(len); i++) ps+=ch;
        return len>0?str+ps:ps+str;
      };
    var processFlags = function(flags,width,rs,arg)
      { var pn = function(flags,arg,rs)
          { if(arg>=0)
              { if(flags.indexOf(' ')>=0) rs = ' ' + rs;
                else if(flags.indexOf('+')>=0) rs = '+' + rs;
              }
            else
                rs = '-' + rs;
            return rs;
          };
        var iWidth = parseInt(width,10);
        if(width.charAt(0) == '0')
          { var ec=0;
            if(flags.indexOf(' ')>=0 || flags.indexOf('+')>=0) ec++;
            if(rs.length<(iWidth-ec)) rs = pad(rs,'0',rs.length-(iWidth-ec));
            return pn(flags,arg,rs);
          }
        rs = pn(flags,arg,rs);
        if(rs.length<iWidth)
          { if(flags.indexOf('-')<0) rs = pad(rs,' ',rs.length-iWidth);
            else rs = pad(rs,' ',iWidth - rs.length);
          }    
        return rs;
      };
    var converters = new Array();
    converters['c'] = function(flags,width,precision,arg)
      { if(typeof(arg) == 'number') return String.fromCharCode(arg);
        if(typeof(arg) == 'string') return arg.charAt(0);
        return '';
      };
    converters['d'] = function(flags,width,precision,arg)
      { return converters['i'](flags,width,precision,arg); 
      };
    converters['u'] = function(flags,width,precision,arg)
      { return converters['i'](flags,width,precision,Math.abs(arg)); 
      };
    converters['i'] =  function(flags,width,precision,arg)
      { var iPrecision=parseInt(precision);
        var rs = ((Math.abs(arg)).toString().split('.'))[0];
        if(rs.length<iPrecision) rs=pad(rs,' ',iPrecision - rs.length);
        return processFlags(flags,width,rs,arg); 
      };
    converters['E'] = function(flags,width,precision,arg) 
      { return (converters['e'](flags,width,precision,arg)).toUpperCase();
      };
    converters['e'] =  function(flags,width,precision,arg)
      { iPrecision = parseInt(precision);
        if(isNaN(iPrecision)) iPrecision = 6;
        rs = (Math.abs(arg)).toExponential(iPrecision);
        if(rs.indexOf('.')<0 && flags.indexOf('#')>=0) rs = rs.replace(/^(.*)(e.*)$/,'$1.$2');
        return processFlags(flags,width,rs,arg);        
      };
    converters['f'] = function(flags,width,precision,arg)
      { iPrecision = parseInt(precision);
        if(isNaN(iPrecision)) iPrecision = 6;
        rs = (Math.abs(arg)).toFixed(iPrecision);
        if(rs.indexOf('.')<0 && flags.indexOf('#')>=0) rs = rs + '.';
        return processFlags(flags,width,rs,arg);
      };
    converters['G'] = function(flags,width,precision,arg)
      { return (converters['g'](flags,width,precision,arg)).toUpperCase();
      };
    converters['g'] = function(flags,width,precision,arg)
      { iPrecision = parseInt(precision);
        absArg = Math.abs(arg);
        rse = absArg.toExponential();
        rsf = absArg.toFixed(6);
        if(!isNaN(iPrecision))
          { rsep = absArg.toExponential(iPrecision);
            rse = rsep.length < rse.length ? rsep : rse;
            rsfp = absArg.toFixed(iPrecision);
            rsf = rsfp.length < rsf.length ? rsfp : rsf;
          }
        if(rse.indexOf('.')<0 && flags.indexOf('#')>=0) rse = rse.replace(/^(.*)(e.*)$/,'$1.$2');
        if(rsf.indexOf('.')<0 && flags.indexOf('#')>=0) rsf = rsf + '.';
        rs = rse.length<rsf.length ? rse : rsf;
        return processFlags(flags,width,rs,arg);        
      } ; 
    converters['o'] = function(flags,width,precision,arg)
      { var iPrecision=parseInt(precision);
        var rs = Math.round(Math.abs(arg)).toString(8);
        if(rs.length<iPrecision) rs=pad(rs,' ',iPrecision - rs.length);
        if(flags.indexOf('#')>=0) rs='0'+rs;
        return processFlags(flags,width,rs,arg); 
      };
    converters['X'] = function(flags,width,precision,arg)
      { return (converters['x'](flags,width,precision,arg)).toUpperCase();
      };
    converters['x'] = function(flags,width,precision,arg)
      { var iPrecision=parseInt(precision);
        arg = Math.abs(arg);
        var rs = Math.round(arg).toString(16);
        if(rs.length<iPrecision) rs=pad(rs,' ',iPrecision - rs.length);
        if(flags.indexOf('#')>=0) rs='0x'+rs;
        return processFlags(flags,width,rs,arg); 
      };
    converters['s'] = function(flags,width,precision,arg)
      { var iPrecision=parseInt(precision);
        var rs = arg;
        if(rs.length > iPrecision) rs = rs.substring(0,iPrecision);
        return processFlags(flags,width,rs,0);
      };
    farr = fstring.split('%');
    retstr = farr[0];
    fpRE = /^([-+ #]*)(\d*)\.?(\d*)([cdieEfFgGosuxX])(.*)$/;
    for(var i=1; i<farr.length; i++)
      { fps=fpRE.exec(farr[i]);
        if(!fps) continue;
        if(arguments[i]!=null) retstr+=converters[fps[4]](fps[1],fps[2],fps[3],arguments[i]);
        retstr += fps[5];
      }
    return retstr;
  }
// Function printf() END 

// ----------------------------------------------------------------------------------------
// Funções auxiliares para gerenciar cookies
// http://www.quirksmode.org/js/cookies.html
function createCookie(name,value,days) {
	if (days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires=" + date.toGMTString();
	}
	else var expires = "";
	document.cookie = name + "=" + value + expires + "; path=/";
}

function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for( var i = 0; i < ca.length; i++ ) {
		var c = ca[i];
		while ( c.charAt(0) == ' ' ) 
          c = c.substring(1,c.length);
		if ( c.indexOf(nameEQ) == 0 ) 
          return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name) {
	createCookie( name,"",-1 );
}

// ----------------------------------------------------------------------------------------
// http://www.quirksmode.org/js/detect.html
var BrowserDetect = {
	init: function () {
		this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
		this.version = this.searchVersion(navigator.userAgent)
			|| this.searchVersion(navigator.appVersion)
			|| "an unknown version";
		this.OS = this.searchString(this.dataOS) || "an unknown OS";
	},
	searchString: function (data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
	},
	dataBrowser: [
		{
			string: navigator.userAgent,
			subString: "Chrome",
			identity: "Chrome"
		},
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari",
			versionSearch: "Version"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			string: navigator.userAgent,
			subString: "iPhone",
			identity: "iPhone/iPod"
	    },
		{
			string: navigator.userAgent,
			subString: "iPad",
			identity: "iPad"
	    },
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]

};
BrowserDetect.init();
