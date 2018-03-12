//==================================================
//=
//=	xhtml degrabable tabs origionally for tsh (thestandardhack.com)
//=	by tim scarfe, developer-x.com / tim@developer-x.com
//=	started - 14:00 22/12/2001 / updated - 4:42 PM 6/19/2002
//=	time spent - 20 hours 
//=
//=	The purpose of this script is the "emphasize" the heading structure of a document if the UA supports enough standards.
//=	Headings will be turned into tabs and nested according to underlying HTML structure.
//=
//=	Many thanks to Aaron Boodman (www.youngpup.net), who presented this challenge to me last year.
//=	This was one of the early examples of DHTML being built as an optional layer of behaviour on top of structured HTML.
//=
//=	Updates
//=
//=	V 1.0 (29/12/2001)
//=
//=		First version of the xHTML tabs.	
//=		
//=		*+Added ID# url linking if you gave a tab an id, but soon removed this feature.
//=
//=	v 2.0 (09:30 AM 15/06/2002):
//=
//=		*+Complete re-writing using DOM2 Range/NodeItterator APIs.
//=		*+Totally scrapped this as it only worked in some versions of Gecko.
//=
//=	v 3.1 (23:11 16/06/2002):
//=
//=		*+ Complete Re-Hash
//=		*+ All Old global props are now per instance, ie. you can have different hover classes for instances.
//=		*+ No more literal functions, All higher-level
//=		*+ As origionally intended, now can be uses hierachially to break down any level of HTML structure, Not limited to one heading level.
//=		*+ Tab systems can now be nested, Can't beleve it took me so long to notice the system had that bug before.. :)
//=		*+ Added a nice Windows-XP theme just to show what is possible (Thanks to Erik Ardvisson for making it!)
//=
//=	v3.2 (10:20 AM 19/6/2002):
//=
//=		*+Fixed style sheet issue where styles were cascadig and not dissapearing.
//=		*+Fixed small rendering issue
//=		*+Added .switchTabToId( id ) - this one is for you Jerry!
//=		*+Added .invertOrder to invert ordering of tabs
//=		*+Added .tabSelectedKeyword so that you can set selection of tab from html in the class
//=		*+Addee tabSystem.takeOutOfMemory	 to clean up some memory issues.
//=		*+Tabsystems can now be restored and re-rendered dynamically.
//=		*+Added events to the tabs, these can be used inline (21:52 19/06/2002)--?
//=		*+Fairly un-intentional, but blow-me Multi-row tabs look pretty damn sweet on this puppy ;)
//=
//=	v3.3 (4:40 PM 19/06/2002)
//=
//=		*+Fixed issue where tab systems were collapsed, but the stylesheet did not go with them.
//=		*+What do we go about collapsed generics?? They will still take the style of the other generics when collapsed.
//=
//=	v3.4 (21:35 19/06/2002)
//=
//=		*+Now uses a Signals/Slots event interface for intrinsic events and DOM events. Special Thanks to Aaron Boodman/www.youngpup.net.
//=		*+Coded in an instance.addEventHandler and instance.removeEventHandler (Any amount of events are cool, or add your own!)
//=		*+I'm so happy, everything is really well scoped now!:)
//=		*+20 mins later.. Fixed some sig/slot bugs
//=	
//=	v3.5
//=		*+Yet more tweaks. i.e. : tabSystem.firstRealHeadingClass
//=
//=	*Bugs*
//=
//=	+	When you collapse a tabset, it also origionally clones the html for any nested tabsets, thus de-rendering them too.
//=		Rather than saving state before, we need to make the system dynamically de-construct a rendered system.
//=	+	What the hell is up with Gecko? I can't even get it to give me constructive error messages (or any to that matter).
//=
//=	*To-Do*
//=
//=	+	Cookie persistance.
//=	+	Animated transitions when changing tabs, by using IE filters and/or clip routines. (**today please).
//=	+	Ability to dynamically add/remove tabs, we need to do a renderStateChange( ) method or something.
//=
//==================================================

//==================================================
//=	Non-Global Code Starts
//==================================================

tabSystem.makeGlobalProps = function( e ) {

	// These are the generic "Catch-All" properties for tabsystems. You may also create custom tab sets.

	tabSystem.classTrigger = "tabsystem";
	tabSystem.tabPageClass = "tabpage";
	tabSystem.activeTabClass = "tab-active";
	tabSystem.hoverTabClass = "tab-hover";
	tabSystem.normalTabClass = "tab";
	tabSystem.startIndex = 0;
	tabSystem.removeHeadings=0;
	tabSystem.styleSheetUri="style/luna/winxp.css";
	tabSystem.tabContainerClass="tabcontainer";
	tabSystem.tabSelectedKeyword="tdefault";
	tabSystem.invertOrder=0;
	tabSystem.firstRealHeadingClass="firstHeading";

	// GLOBAL ONLY (Custom Event Listeners)

	tabSystem.style_changeEvent="onstylechange";
	tabSystem.buildEvent="onbuild";
	tabSystem.unbuildEvent="onunbuild";
	tabSystem.tab_mouseoverEvent="onmouseover";
	tabSystem.tab_mouseoutEvent="onmouseout";
	tabSystem.tab_clickEvent="onclick";

	tabSystem.Dependancies = ( document
					&& document.getElementById
					&& document.getElementsByTagName
					&& document.createElement
					&& document.getElementsByClassName // this is an extension component
					&& window )?1:0;
}

tabSystem.takeOutOfMemory = function _tabSystem_takeOutOfMemory_( e ) {

	for(var x=0;x<tabSystem.instances.length;x++) {

		tabSystem.instances[x].takeOutOfMemory( );
	}
}

tabSystem.construct = function tabSystem_construct_( e ) { // render generic "catch-all" tabsets

	var ts = document.getElementsByClassName( 
					document, tabSystem.classTrigger 
					)
	
	for(var x=0;x<ts[ tabSystem.classTrigger ].length;x++) {

		var el=ts[ tabSystem.classTrigger ][ x ];

		tabSystem.instances.push( new tabSystem( el, true ) );
	}
}

tabSystem.genericStyleSheet = function tabSystem_genericStyleSheet_( e ) { // "catch-all" tabset style sheets

	if(!tabSystem.styleSheet) {

		var head=document.getElementsByTagName("head").item( 0 );
		var s=document.createElement("link");
		s.rel="stylesheet";
		s.type="text/css";
		s.media="screen";
		head.appendChild( s );
		tabSystem.styleSheet=s;
	}

	tabSystem.styleSheet.href=((e)?e:tabSystem.styleSheetUri);
}

tabSystem.getTabsetByDivision = function _tabSystem_getTabsetByDivision_( el ) {

	for(var x=0;x<tabSystem.instances.length;x++) {

		if(tabSystem.instances[x].el==el) return( tabSystem.instances[x] )
	}
}

function tabSystem( el, genericCapture ) {

	this.classTrigger=tabSystem.classTrigger;
	this.tabPageClass=tabSystem.tabPageClass;
	this.activeTabClass=tabSystem.activeTabClass;
	this.tabContainerClass=tabSystem.tabContainerClass;
	this.hoverTabClass=tabSystem.hoverTabClass;
	this.normalTabClass=tabSystem.normalTabClass;
	this.startIndex=tabSystem.startIndex;
	this.genericCapture=genericCapture;
	this.removeHeadings=tabSystem.removeHeadings;
	this.styleSheetUri=tabSystem.styleSheetUri;
	this.tabSelectedKeyword=tabSystem.tabSelectedKeyword;
	this.invertOrder=tabSystem.invertOrder;

	this.el = el;
	this.tabs=[];
	this.id=this.el.getAttribute("id");

	// instance sig/slot event handling

	this.addEventHandler=function _tabSystem_addEventHandler_( evt, fn, obj, once ) { Listener.add( ((obj)?obj:this), evt, fn, this, once ) };
	this.removeEventHandler=function _tabSystem_removeEventHandler_( evt, fn, obj ) { Listener.remove( ((obj)?obj:this), evt, fn, this ) };
	this.fireEventHandler=function _tabSystem_fireEventHandler_( evt, obj, args ) { if( ((obj)?obj:this)[evt] )((obj)?obj:this)[evt](args) };

	this.takeOutOfMemory = function _tabSystem_takeOutOfMemory_( e ) { // ie6..s

		for(x in this) {try{this[x]=null}catch(ex){;;;}}
	}

	this.addStyleSheet=function _tabSystem_addStyleSheet_( stURI ) {
		
		if(!this.genericCapture) { // if called on a generic level, bubbles up to global level

			if(!this.styleSheet) {

				var head=document.getElementsByTagName("head").item( 0 );

				this.styleSheet=document.createElement("link");
				this.styleSheet.rel="stylesheet";
				this.styleSheet.type="text/css";
				this.styleSheet.media="screen";
				this.styleSheet.href=stURI;
				head.appendChild( this.styleSheet );
			}

			else {
				this.styleSheet.disabled=0;
				this.styleSheet.href=stURI;
			}
		}

		else tabSystem.genericStyleSheet( stURI );

		this.fireEventHandler( tabSystem.style_changeEvent )
	}

	this.backup=function _tabSystem_backup_( e ) {

		this.bDocFrag=this.el.cloneNode( 1 );
	}

	this.restore=function _tabSystem_restore_( e ) {

		// insert backup

		var tmp=this.el.parentNode.insertBefore( this.bDocFrag, this.el );

		// remove modified version..

		this.el.parentNode.removeChild( this.el );

		this.el=null; this.el=tmp;

		this.rendered=0;

		this.fireEventHandler( tabSystem.unbuildEvent );

		var s=this.styleSheet||tabSystem.styleSheet;
		if(s)s.disabled=1;
	}

	this.build = function _tabSystem_build_( e ) { // division the structure..

		this.backup( );

		this.addStyleSheet( this.styleSheetUri );

		// insert a division at the beginnning of the main div to hold tabs.

		this.tabContainer = document.createElement("div");
		this.tabContainer.className=this.tabContainerClass;
		this.el.insertBefore( this.tabContainer, this.el.firstChild );

		// create a structured list in the container for the tabs to sit in, (good for no css support).

		this.ul=document.createElement( "ul" );
		this.tabContainer.appendChild( this.ul ); 

		// itterate through the tab-pages in the tab division..

		var d=this.el.getElementsByTagName("div"), z=0;
		
		for(var x=0;x<d.length;x++) {
			
			if(d[x].className.search(this.tabPageClass)!=-1 && d[x].parentNode==this.el) {

				var t={}; t.index=z;

				// check to see if user has requested a default selection on tab

				if( d[x].className .search( this.tabSelectedKeyword ) != -1 ) 
					
					this.startIndex=z;

				// first we take out the first child of the tab page
				// we clone the node for later use.

				var h=d[x].firstChild;
		
				t.tabPage=d[x];

				t.hClone=h.cloneNode( 1 );

				if( this.removeHeadings ) {

					// now we take out the heading from the tab-page.

					d[ x ].removeChild( h );
 
				} else d[x].className=tabSystem.firstRealHeadingClass;
				
				// re-style the old heading

				this.styletab( t );
				
				// insert the tab into the structured list in the container.

				t.li = document.createElement("li");
				t.li.appendChild( t.hClone );

				// by default tabs will look the wrong way around

				if( this.invertOrder && this.ul.firstChild )  this.ul.insertBefore( t.li, this.ul.firstChild );

					else this.ul.appendChild( t.li );
			
				// add tab to main obj array;

				this.tabs[ z ]=t;

				// add events to the tab			

				this.addEvents( this.tabs[ z ] );

				z++ // this is the counter to keep track of tabs.
			}
		}

		// all tabs rendered, now switch to the default first index

		this.switchtab( this.startIndex, 1 );

		this.rendered=1;

		this.fireEventHandler( tabSystem.buildEvent );
	}

	this.styletab = function _tabSystem_styletab_( tab ) {

		var t=tab.hClone;

		t.style.cursor=document.all?"hand":"pointer";
		t.className=tab.backupClassName||t.className+" "+this.normalTabClass;
	}

	this.switchTabToId = function _tabSystem_switchtab_( id ) { // maybe test this? ;)

		for(var x=0;x<this.tabs.length;x++) {

			if( this.tabs[x].hClone.id && this.tabs[x].hClone.id==id ) {

				this.switchtab( this.tabs[x].index );
			}
		}
	}

	this.switchtab = function _tabSystem_switchtab_( e, firstTime ) {

		var t=this.tabs[e]; // NOT hClone!

		if( this.selected ) {

			this.styletab( this.selected );
			this.addEvents( this.selected );
		
			this.selected.tabPage.style.display="none";
		}

		if( firstTime ) { // hide all tab-pages first time

			for(var x=0;x<this.tabs.length;x++) {

				this.tabs[x].tabPage.style.display="none";
			}
		}

		t.hClone.className +=" "+ this.activeTabClass;
		
		this.removeEvents( t );

		t.tabPage.style.display="block";

		this.selected=null; this.selected=t;
	}

	this.removeEvents = function( tab ) {

		var t=tab;

		this.removeEventHandler( "onmouseout", t.evt._mou_, t.hClone );
		this.removeEventHandler( "onclick", t.evt._click_, t.hClone );
		this.removeEventHandler( "onmouseover", t.evt._mov_, t.hClone );
	}

	this.addEvents = function _tabSystem_addevents_( tab ) {

		tab.backupClassName=tab.hClone.className;

		var index=tab.index, obj=this;

		tab.evt={};

		tab.evt._mov_ = function ( e ) {

			obj.tabs[ index ].hClone.className+=" "+obj.hoverTabClass;

			obj.fireEventHandler( tabSystem.tab_mouseoverEvent, obj.tabs[ index ] );
		}

		tab.evt._mou_ = function ( e ) {

			obj.tabs[ index ].hClone.className=obj.tabs[ index ].backupClassName;

			obj.fireEventHandler( tabSystem.tab_mouseoutEvent, obj.tabs[ index ] );
		}

		tab.evt._click_ = function( e ) {

			obj.switchtab( index );

			obj.fireEventHandler( tabSystem.tab_clickEvent, obj.tabs[ index ] );
		}

		this.addEventHandler( "onmouseout", tab.evt._mou_, tab.hClone );
		this.addEventHandler( "onclick", tab.evt._click_, tab.hClone);
		this.addEventHandler( "onmouseover", tab.evt._mov_, tab.hClone );
	}

	if ( this.genericCapture ) this.build( );

	this.addEventHandler( "onunload", this.takeOutOfMemory, 0, 1 );
}

//==================================================
//=	Patch in a couple of things we need
//==================================================

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

//==================================================
//=	Global Code Starts
//==================================================

tabSystem.instances = [ ];

tabSystem.makeGlobalProps( );

if( tabSystem.Dependancies && Listener) Listener.add( window, "onload", tabSystem.construct, tabSystem, 1 )

	else window.status="Standards-Check failed for tab system";
