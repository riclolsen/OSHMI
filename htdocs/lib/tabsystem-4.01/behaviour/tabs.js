//==================================================
//=
//=	xhtml degrabable tabs origionally for tsh (thestandardhack.com)
//=	by tim scarfe, developer-x.com / tim@developer-x.com
//=	started - 14:00 22/12/2001 / updated - 2:39 PM 6/21/2002
//=	time spent - 30 hours 
//=
//=	The purpose of this script is the "emphasize" the heading structure of a document if the UA supports enough standards.
//=	Headings will be turned into tabs and nested according to underlying HTML structure.
//=
//=	Many thanks to Aaron Boodman (www.youngpup.net), who presented this challenge to me last year.
//=	This was one of the early examples of DHTML being built as an optional layer of behaviour on top of structured HTML.
//=
//=	Copyright:
//=
//=	You may use this for anything on anything and change it as you will. Leave this and everything alove this intact.
//=	You may remove the updates info and bugs below if you want to save space. Please show me your implementations of this,
//=	as that is what interests me the most. It should be so easy to hook this up to an XML-RPC system and goodness knows what else!
//=
//=	Updates:
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
//=	v3.5 (03:03 20/06/2002)
//=		
//=		*+Loads more tweaking. 
//=		*+Added in TabSystem.addEventHandler,removeEventHandler, fireEventHandler + tabSystem."onload" (generics)
//=
//=	v4 (2:41 PM 6/21/2002)
//=			
//=		*+Active style was not inheriting normal tab style: fixed;
//=		*+Loads of new properties and methods!
//=		*+Decent deconstruct method, fixed bugs with build method, addtab method added, rebuild method
//=		*+Totally shifted the object model! Now there is a "tab" object that is totally seperate
//=		*+Things like tab.style(tab) are now just tab.style()::read updated object model for more nfo
//=		*+Having tab object now makes adding and removing tabs dynamically so easy
//=		*+this.getTabIndexByTab added, tab.remove() added
//=		*+Now tabs have their very own signal/slot event level!!! ie. tab.addEventHandler
//=
//=	v4.01 (04:32 24/08/2002)
//=
//=		*+Gecko Added, Just a one-liner fix :)
//=
//=
//=	*To-Do*
//=
//=		*Cookie Persistance
//=		*Better OOP design, ie. TabSystem, Tab, TabPage, TabFooter etc. Proper objects then will be passed to methods, not tab indexes and shit like that
//=
//=	*Bugs*
//=
//=	+	When we style change in gecko, we presently need to remove all current styling, do something about this..
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
	tabSystem.removeHeadings=true;
	tabSystem.styleSheetUri="style/luna/winxp.css";
	tabSystem.tabContainerClass="tabcontainer";
	tabSystem.tabSelectedKeyword="tdefault";
	tabSystem.invertOrder=0;
	tabSystem.firstHeadingClass="firstHeading";
	tabSystem.firstHeadingKeepTrigger="keepheading";

	// GLOBAL ONLY (Custom Event Listeners)

	tabSystem.style_changeEvent="onstylechange";
	tabSystem.buildEvent="onbuild";
	tabSystem.unbuildEvent="onunbuild";
	tabSystem.tab_mouseoverEvent="onmouseover";
	tabSystem.tab_mouseoutEvent="onmouseout";
	tabSystem.tab_clickEvent="onclick";
	tabSystem.onloadEvent="onload";

	tabSystem.Dependancies = ( document
					&& document.getElementById
					&& document.getElementsByTagName
					&& document.createElement
					&& document.getElementsByClassName // this is an extension component
					&& window )?1:0;

	// global sig/slot event handling

	tabSystem.addEventHandler=function _glo_tabSystem_addEventHandler_( evt, fn, obj, once ) { Listener.add( ((obj)?obj:tabSystem), evt, fn, this, once ) };
	tabSystem.removeEventHandler=function _glo_tabSystem_removeEventHandler_( evt, fn, obj ) { Listener.remove( ((obj)?obj:tabSystem), evt, fn, this ) };
	tabSystem.fireEventHandler=function _glo_tabSystem_fireEventHandler_( evt, obj, args ) { if( ((obj)?obj:this)[evt] )((obj)?obj:tabSystem)[evt](args) };

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

	tabSystem.fireEventHandler( tabSystem.onloadEvent );
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

//==================================================
//=	Tab Object Constructor
//==================================================

tabSystem.tab = function _tabSystem_tab_( member ) {

	this.ts=member;
	this.index=null;
	this.tabPage=null;
	this.hClone=null;
	this.event={};
	this.heading=null;

	// tab level sig/slot event handling

	this.addEventHandler=function _tab_addEventHandler_( evt, fn, obj, once ) { Listener.add( ((obj)?obj:this), evt, fn, this, once ) };

	this.removeEventHandler=function _tab_removeEventHandler_( evt, fn, obj ) { Listener.remove( ((obj)?obj:this), evt, fn, this ) };

	this.fireEventHandler=function _tab_fireEventHandler_( evt, obj, args ) { if( ((obj)?obj:this)[evt] )((obj)?obj:this)[evt](args) };

	this.notReady="Like the Grolsh, we only let you drink it... when it's ready. This tab is not (ready that is)."

	this.readyStateCheck = function _tabSystem_tab_readyStateCheck_( e ) {

		return (this.hClone&&this.tabPage&&this.ts) ?1:0;
	}

	this.addTabPage=function _tabSystem_tab_addTabPage_( e ) { this.tabPage=e; }

	this.createListItem = function _tabSystem_tab_createListItem_( e ) {

		if(this.hClone) {

			this.li = document.createElement("li");
			this.li.appendChild( this.hClone );

		} //else throw new Error(this.notReady);
	}
	
	this.addHeading = function _tabSystem_tab_addHeading_( dom_node ) { 

			this.hClone=dom_node;

			if(this.hClone.style.display=="none") this.hClone.style.display="block";
	}

	this.styletab = function _tabSystem_tab_styletab_( e ) {

		if( this.readyStateCheck( ) ) {

			this.hClone.style.cursor=document.all?"hand":"pointer";
			this.hClone.className=this.ts.normalTabClass+" "+this.ts.backupClassName||this.ts.className;

		}  //else throw new Error( this.notReady );
	}

	this.removeEvents = function _tabSystem_tab_removeEvents_( tab ) {

		if( this.readyStateCheck( ) ) {

			this.removeEventHandler( "onmouseout", this.event._mou_, this.hClone );
			this.removeEventHandler( "onclick", this.event._click_, this.hClone );
			this.removeEventHandler( "onmouseover", this.event._mov_, this.hClone );
	
		} //else throw new Error(this.notReady);
	}

	this.addEvents = function _tabSystem_addevents_( tab ) {

		if( this.readyStateCheck( ) ) {

			this.backupClassName=this.hClone.className;

			var index=this.index, obj=this.ts;

			this.event._mov_ = function ( e ) {

				obj.tabs[ index ].hClone.className+=" "+obj.hoverTabClass;

				obj.fireEventHandler( tabSystem.tab_mouseoverEvent, obj.tabs[ index ] );
			}

			this.event._mou_ = function ( e ) {

				obj.tabs[ index ].hClone.className=obj.tabs[ index ].backupClassName;

				obj.fireEventHandler( tabSystem.tab_mouseoutEvent, obj.tabs[ index ] );
			}

			this.event._click_ = function( e ) {

				obj.switchtab( index );

				obj.fireEventHandler( tabSystem.tab_clickEvent, obj.tabs[ index ] );
			}

			this.addEventHandler( "onmouseout", this.event._mou_, this.hClone );
			this.addEventHandler( "onclick", this.event._click_, this.hClone);
			this.addEventHandler( "onmouseover", this.event._mov_, this.hClone );
		}

		//else throw new Error(this.notReady);
	}

	this.remove = function _tabSystem_removePage_(  ) {

		if( this.readyStateCheck( ) ) {

			this.tabPage.parentNode.removeChild( this.tabPage );
			this.ts.rebuild( );

		}// else throw new Error(this.notReady);
	}
}

//==================================================
//=	TabSystem Object Constructor
//==================================================

function tabSystem( el, genericCapture ) {

	if(!genericCapture)tabSystem.instances.push(this);

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
	this.firstHeadingClass=tabSystem.firstHeadingClass;
	this.firstHeadingKeepTrigger=tabSystem.firstHeadingKeepTrigger;

	this.el = el;
	this.tabs=[];
	this.id=this.el.getAttribute("id");

	this.getTabIndexByTab=function _tabSystem_getTabIndexByTab_( e ) {

		for(var x=0;x<this.tabs.length;x++)if (this.tabs[x]==e) return(x)
	}

	// instance sig/slot event handling

	this.addEventHandler=function _tabSystem_addEventHandler_( evt, fn, obj, once ) { Listener.add( ((obj)?obj:this), evt, fn, this, once ) };
	this.removeEventHandler=function _tabSystem_removeEventHandler_( evt, fn, obj ) { Listener.remove( ((obj)?obj:this), evt, fn, this ) };
	this.fireEventHandler=function _tabSystem_fireEventHandler_( evt, obj, args ) { if( ((obj)?obj:this)[evt] )((obj)?obj:this)[evt](args) };

	this.rebuild = function _tabSystem_rebuild_( e ) {

		// this basically just collapses and re-constructs the tab system
		// doing this would obviously pick up on any new tabs that have been added.
		// need to add a more efficient way of doing this at some point (hence this tier level)

		var si=this.selected.index;
		this.deconstruct( );
		this.build( );
		this.switchtab(si);
	}

	// add a tab dynamically

	this.addTab = function _tabSystem_addTab_( div, insertBefore ) {

		// please supply a division dom node with a first child of a heading element;
		// i'm not going to wipe your arse for you, if you fuck it up, it's your problem :)

		div.className=this.tabPageClass;

		this.el.insertBefore( div, (insertBefore)?insertBefore:this.tabs[0].tabPage );

		this.rebuild();
	}

	// ie6 memory

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

		this.fireEventHandler( tabSystem.style_changeEvent );
	}

	this.backup=function _tabSystem_backup_( e ) {

		// this will backup any nested tab sets too, you have been warned

		this.bDocFrag=this.el.cloneNode( 1 );
	}

	this.deconstruct=function _tabSystem_restore_( e ) {

		if(this.rendered==0) throw new Error("This tab set is not built.");

		// hide the tab container

		this.tabContainer.style.display="none";

		// we have a very nasty sitation here: nested tabsets will still take style down from parents
		// because ie only recognises the "any decendants of.." css space thingy. not direct children; ">".
		// the way to get around it is to have another class name for the tab pages on the nested tabset, but 
		// this wont be caught by generics. What a pain the arse!

		for(var x=0;x<this.tabs.length;x++) {

			// we make all tab page divisions visible

			this.tabs[x].tabPage.style.display="block";

			// if first headings had been hidden, we make them viewable ;)

			if( this.removeHeadings ) this.tabs[x].heading.style.display="block";
		}

		// we do misc stuff and fire events off etc

		this.rendered=0;

		this.fireEventHandler( tabSystem.unbuildEvent );

		var s=this.styleSheet||tabSystem.styleSheet;
		if(s)s.disabled=1;
	}
	
	this.build = function _tabSystem_build_( e ) { // division the structure..

		if(this.rendered==1) throw new Error("This tab set is already built.");

		// memory cleanup

		if(this.tabs.lengh>0) for(var x=0;x<this.tabs.length;x++) {

			for(x in this.tabs[x]) if(this.tabs[x]!=this) this.tabs[x]=null;
		}

		// reset tabs copllection

		this.tabs=[];

		//this.backup( );

		this.addStyleSheet( this.styleSheetUri );

		if( !this.hasBeenBuiltOnce && 
			this.el.className.search( 
				new RegExp( this.firstHeadingKeepTrigger ) )!=-1) this.removeHeadings=0;

		// insert a division at the beginnning of the main div to hold tabs.

		this.tabContainer = document.createElement("div");
		this.el.insertBefore( this.tabContainer, this.el.firstChild );
		this.tabContainer.style.display="block";
		this.tabContainer.className=this.tabContainerClass;

		// create a structured list in the container for the tabs to sit in, (good for no css support).

		this.ul=document.createElement( "ul" );
		this.tabContainer.appendChild( this.ul ); 

		// itterate through the tab-pages in the tab division..

		var d=this.el.getElementsByTagName("div");var z=0;
		
		for(var x=0;x<d.length;x++) {
			
			if(d[x].className.search(this.tabPageClass)!=-1 && d[x].parentNode==this.el) {

				var t = new tabSystem.tab( this );

				t.index=z;

				// check to see if user has requested a default selection on tab

				if( !this.hasBeenBuiltOnce
					 && d[x].className .search( this.tabSelectedKeyword ) != -1 ) 
						this.startIndex=z;

				// first we take out the first child of the tab page
				// we clone the node for later use.

				var h=d.item( x ).firstChild.style?d.item( x ).firstChild:d.item( x ).childNodes.item( 1 );

				if( this.removeHeadings ) {

					// now we take out the heading from the tab-page.

					h.style.display="none";

				} else h.style.display="block";

				// add heading/page to tab

				t.addTabPage( d[x] );
				t.addHeading( h.cloneNode( 1 ) );

				// style the heading 

				h.className=this.firstHeadingClass +=" "+h.className;
				
				// re-style the old heading

				t.styletab( );
				
				// insert the tab into the structured list in the container.
					
				t.createListItem( );

				// by default tabs will look the wrong way around

				if( this.invertOrder && this.ul.firstChild )  this.ul.insertBefore( t.li, this.ul.firstChild );

					else this.ul.appendChild( t.li );
			
				// reference to the origional heading

				t.heading=h;

				// add events to the tab	

				t.addEvents( );

				// add tab to main obj array;

				this.tabs[ z ]=t;

				z++ // this is the counter to keep track of tabs.
			}
		}

		// all tabs rendered, now switch to the default first index

		this.switchtab( this.startIndex, 1 );

		this.rendered=1;

		this.hasBeenBuiltOnce=1;

		this.fireEventHandler( tabSystem.buildEvent );
	}

	this.switchTabToId = function _tabSystem_switchtab_( id ) { // maybe test this? ;)

		for(var x=0;x<this.tabs.length;x++) {

			if( this.tabs[x].hClone.id && this.tabs[x].hClone.id==id ) {

				this.switchtab( this.tabs[x].index );
			}
		}
	}

	this.switchtab = function _tabSystem_switchtab_( e, firstTime ) { // switches on tab INDEX not tab

		if(!this.tabs[e]) throw new Error("tab does not exist..");

		var t=this.tabs[e]; // NOT hClone!

		if( this.selected ) {

			this.selected.styletab( );
			this.selected.addEvents( );
		
			this.selected.tabPage.style.display="none";
		}

		if( firstTime ) { // hide all tab-pages first time

			for(var x=0;x<this.tabs.length;x++) {

				this.tabs[x].tabPage.style.display="none";
			}
		}

		t.hClone.className +=" "+ this.activeTabClass;

		t.removeEvents( );

		t.tabPage.style.display="block";

		this.selected=null; this.selected=t;
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

if(!window.Error) { // some browsers dont have this, and i dont want to throw strings in ie5..

	function Error( message ) {
		this.message=message;
		this.name=new String;
	}
}

//==================================================
//=	Global Code Starts
//==================================================

tabSystem.instances = [ ];

tabSystem.makeGlobalProps( );

if( window && tabSystem.Dependancies && Listener) Listener.add( window, "onload", tabSystem.construct, tabSystem, 1 )

	else window.status="Standards-Check failed for tab system";
