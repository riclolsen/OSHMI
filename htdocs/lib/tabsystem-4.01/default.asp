
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN"
	"http://www.w3.org/TR/REC-html40/strict.dtd">

<html lang="en">

	<head>

		<title>EnCana UK</title>

			<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> 

		<link rel="stylesheet" href="style/basic.css" type="text/css" title="Basic style">

		<!-- add in behaviour w/dependancies first -->

		<script type="text/javascript" src="behaviour/listener.js"></script>
		<script type="text/javascript" src="behaviour/tabs.js"></script>

	</head>

<body>

<!-- Many thanks to Erik Ardvisson, as I chiefed his win-xp theme images being the lazy person I am ;) -->

<div id="main">

	<h1>Degradable, OOP DHTML Tabs.</h1>

	<p><em>By Tim Scarfe, 2002.</em></p>

	<div class="tabsystem keepheading">

		<div class="tabpage scrollit">

			<h2>What's New?</h2>

			<h3>24th August 2002 - Version 4.02</h3>

			<p>Fixed a couple of lines in the listener.js that affected Mozilla 1.0 and NS6.</p>

			<h3>24th August 2002 - Version 4.01</h3>

			<p><em>Gecko Support Added!</em></p>

			<p>Been getting quite a few e-mails about Gecko support. I never did quite see what was stopping it as all the code is pretty standards-compliant. Anyway, after some investigation I found that it was an issue selecting the first node on a tab page. IE thinks it is the first child, and Gecko thinks it is the second. Ohh well. All fixed :)</p>

			<h3>21th June 2002 - Version 4 Now Released!</h3>

			<p>Put a lot of work into this again today, and now we have some more fantastic features such as:</p>

			<ul class="square">
			<li>Cookie Persistance</li>
			<li>Dynamic removal/Adding of tabs</li>
			<li>Signal and Slot Custom event handlers on the Class, Instance and Tab levels!</li>
			<li>Loads of new methods.</li>
			<li>New deconstruct(), and rebuild() methods.</li>
			<li>Bug fixes.</li>
			</ul>

			<p>Take a look at the <a href="encana.asp">new example page</a>.</p>
		</div>

		<div class="tabpage tdefault scrollit">

			<h2 style="font-weight: bold;" title="the introduction, and all that stuff :)">Intro</h2>

			<p>Ever since the <a href="http://www.developer-x.com/old/webroot/?page=projects/tabs/default.asp">Early Days</a>, I have been fascinated by Tab Panes in DHTML.</p>

			<p>Last December (2001), A certain <a href="http://www.youngpup.net">Mr. Aaron Boodman</a> put the challenge to me to make a tab pane built on top of a structured HTML foundation. </p>

			<h3>Goals of the system</h3>

			<ul class="square">
			<li><p>To be written in such a way, that if the browser had no knowledge of JavaScript or CSS, it would still see the structured content. However, if CSS and/or Javascript were supported, The content and/or behaviour would be added accordingly.</p></li>
			<li><p>Needs to be very flexible, Any inline style, class, events, titles or anything in the html, needs to be brought accross to the tab pane.</p></li>
			<li><p>Multi-row tabs.</p></li>
			<li><p>Headers and footers on tab systems.</p></li>
			<li><p>To be written using Object-Oriented Javascript so that the system can be easily extended and prototyped, even if they have limited knowledge.</p></li>
			<li><p>To allow the user to dynamically style the system.</p></li>
			<li><p>To target web standards rather than web browsers, Should work on web browsers in the future that we have not heard of.</p></li>
			<li><p>Needs to be quick and easy for developers to implement. ie. Documented object model.</p></li>
			<li><p>Developers need to be free to use it where ever they want or need without having to worry about stupid licensing issues.</p></li>
			<li><p>System needs to have a "catch-all" system for rendering tabs, and also the power to create them dynamically.</p></li>
			<li><p>Needs to fit in with other DHTML systems through use of events.</p></li>
			<li><p>System is built on a structured HTML foundation, NO need for stupid <a href="http://www.developer-x.com/content/innerhtml">innerHTML</a></p></li>
			<li><p>Built using signal/slot events for the ultimate in extensibility.</p></li>
			</ul>

		</div>

		<div class="tabpage scrollit">

			<h2>Usage</h2>

			<h3>Basic Usage</h3>

			<p>There is nothing to it!</p>

			<p>The code looks like this;</p>

<pre>
&lt;div class="tabsystem"&gt;

	&lt;div class="tabpage tdefault"&gt;

		&lt;h2&gt;this is a tab&lt;/h2&gt;

		&lt;p&gt;this appears inside the first tab..&lt;/p&gt;

		&lt;p&gt;this tab is selected by default&lt;/p&gt;
	&lt;/div&gt;

	&lt;div class="tabpage tdefault"&gt;

		&lt;h2&gt;this is a tab too&lt;/h2&gt;

		&lt;p&gt;this appears inside the second tab..&lt;/p&gt;
	&lt;/div&gt;

	&lt;p&gt;this is a tab set footer 
		because it is not in a tab page,
		but is in the tab set division.&lt;/p&gt;
&lt;/div&gt;
</pre>		

<h3>Tab Styling</h3>

<p>Through use of CSS, you can totally style every aspect of your tabs.<p>

<p>Please refer to the following diagram for an example of how to reference different parts of the system with CSS.</p>

<img src="images/css.png" />

<h3>Dynamic Creation</h3>

<p>We create a tabset in html. Instead of having the top division class of "tabsystem", we go for "my-system". This means it will not be picked up by the generic catch-all process. We can still reference this tabset directly in CSS by using our new top level as a "scope". ie. <code>.my-system .tabpage{..}</code></p>

<pre>
var ts=new tabSystem( document.getElementById("my-system") );

ts.invertOrder=1;
</pre>

<h3>Custom Events</h3>

<pre>
ts.addEventHandler( "onbuild", function( e ) { 

			ts.addStyleSheet( "style/custom.css" ); 
		} );

ts.addEventHandler( "onmouseover", function( e ) {
       
        ts.tabs[ 0 ].hClone.style.color="green";       

        }, ts.tabs[ 0 ] );

ts.addEventHandler( "onmouseover", function( e ) {
       
        ts.tabs[ 0 ].hClone.style.backgroundColor="blue";       

        }, ts.tabs[ 0 ], 1 ); 

ts.build( );
</pre>

<h3>Prototyping the system</h3>

<p>Say we would like to add a <code>.hideTab( )</code> method into the codebase (It is pointless for me to predict every implementation and prototype everything in the default release, so it is up to the implementor to code in some of the small things).</p>

<p>Now do we call it on tab index, or just supply a reference to the tab object as an argument. hmmmm.. Ok, Let's try the index.</p>

<p>We would study the object model, and come up with something like this:</p>

<pre>
tabSystem.prototype.hideTab = function ( index ) {

	for(var x=0;x&lt;this.tabs.length;x++) { 

		// loop through all tabs

		if(this.tabs[x].index==index) {

			// found it!!

			// .hClone is a reference to the tab
			// .tabPage is a reference to the tab page

			this.tabs[x].hClone.style.display="none";
			this.tabs[x].tabPage.style.display="none";
		}
	}
}
</pre>

<p>Maybe now we can put two and two together to create a .showTab function?</p>

<pre>
tabSystem.prototype.showTab = function ( index ) {

	for(var x=0;x&lt;this.tabs.length;x++) { 

		if(this.tabs[x].index==index) {

			this.tabs[x].hClone.style.display="block";
			this.tabs[x].tabPage.style.display="block";
		}
	}
}
</pre>

<p>Have fun with my system guys, I sure enjoyed making it. If you have any problems, don't hesitate to <a href="mailto:tim@developer-x.com">e-mail me</a>.</p>

		</div>

		<div class="tabpage scrollit">

			<h2>Obj-Model</h2>

			<p>This page will describe the object model of the tab system. </p>

			<p>(6:27 PM 6/21/2002) - Doh, this is already a little bit out of date; The events now work on three levels (Class, Instance and Tab), And there are many new methods and features. I'll update this soon, but in the mean time look at the source code and the examples.</p>

					<h3>Instance</h3>

					<p>These are the Instance Methods, Properties and Events.</p>

					<div class="tabsystem noShift">

						<div class="tabpage noborder">

							<h3>Properties</h3>

							<ul class="square">

							<li><h4>.classTrigger</h4>
							<p><em>String</em></p>
							<p>This is the generic trigger class that the catch-all system will intercept. It is "tabsystem" by default.</p></li>
	
							<li><h4>.tabPageClass</h4>
							<p><em>String</em></p>
							<p>Class trigger for the tab pages themselves. Remember that the first heading in the tab page will be used for the tab. this gives the freedom to use the system in any HTML structure.</p></li>

							<li><h4>.activeTabClass</h4>
							<p><em>String</em></p>
							<p>This is the class for you to structure the active tabs.</p></li>

							<li><h4>.tabContainerClass</h4>
							<p><em>String</em></p>
							<p>This is the block that sits above the tab page that holds the tabs. This can be set to stretch right accross the width of the tabset to add some cool styling.</p></li>

							<li><h4>.hoverTabClass</h4>
							<p><em>String</em></p>
							<p>This is the class for you to structure the hover tabs.</p></li>

							<li><h4>.normalTabClass</h4>
							<p><em>String</em></p>
							<p>This is the class for you to structure the normal tabs.</p></li>

							<li><h4>.startIndex</h4>
							<p><em>Number</em></p>
							<p>Sets the default start index of the tab set. Bare in mind, you can also use a trigger in the html class to set a default start tab.</p></li>

							<li><h4>.genericCapture</h4>
							<p><em>Boolean</em></p>
							<p>The property describes whether the tabset is a generic "catch all" one or a manually set one.</p></li>

							<li><h4>.removeHeadings</h4>
							<p><em>Boolean</em></p>
							<p>By default, the tab headings become part of the tab set, you can set here whether you may keep the heading itself in the tab page.</p></li>

							<li><h4>.styleSheetUri</h4>
							<p><em>String</em></p>
							<p>Sets the Uri to the stylesheet. You may set this property and call the main method with no arguments to change the style, or you may just call the main <code>.setStyleSheet</code> method with an argument.</p></li>

							<li><h4>.tabSelectedKeyword</h4>
							<p><em>String</em></p>
							<p>This is a keyword you can add into the class of a tab to say that it should be selected by default. ie. &lt;h2 class="tselected hello class"&gt;my tab&lt;/h2&gt;</p></li>

							<li><h4>.invertOrder</h4>
							<p><em>Boolean</em></p>
							<p>Inverts the order of the tabs.</p></li>

							<li><h4>.el</h4>
							<p><em>DOM Node</em></p>
							<p>This is a reference to the main division that houses the tab pages.</p></li>

							<li><h4>.id</h4>
							<p><em>String</em></p>
							<p>The id of the tab system, if you gave it one in the HTML.</p></li>

							<li><h4>.styleSheet</h4>
							<p><em>DOM Node</em></p>
							<p>A reference to the stylesheet (LINK element) for this tab set.</p></li>

							<li><h4>.rendered</h4>
							<p><em>Boolean</em></p>
							<p>Has the tab set been rendered? This will change if you subsequently de-construct the tab set or re-build it.</p></li>

							<li><h4>.ul</h4>
							<p><em>DOM Node</em></p>
							<p>Reference to the unordered list that the tabs are stored in. This list lives in the tabContainer div element which is inserted right at the front of the tab system division.</p></li>

							<li><h4>.tabContainer</h4>
							<p><em>DOM Node</em></p>
							<p>Reference to the container division that holds the ul/tabs.</p></li>

							<li><h4>.tabs</h4>
							<p><em>Array</em></p>
							<p>A collection of the tabs that the tab set holds.</p></li>
	
							<li><h4>.tabs.length</h4>
							<p><em>Number</em></p>
							<p>The amount of tabs in a tab set.</p></li>
	
							<li><h4>.tabs[x].hClone</h4>
							<p><em>Don Node</em></p>
							<p>This is a reference to the heading element stored in the structured list in the tab container, NOT the origional heading.</p></li>

							<li><h4>.tabs[x].index</h4>
							<p><em>Number</em></p>
							<p>Index of the current tab.</p></li>

							<li><h4>.tabs[x].selected</h4>
							<p><em>[Object Tab]</em></p>
							<p>Reference to the current selected tab.</p></li>


							<li><h4>.firstHeadingClass</h4>
							<p><em>String</em></p>
							<p>If the top header stays, this defines it's default class. (You can just set it from the HTML.)</p></li>

							<li><h4>.firstHeadingKeepTrigger</h4>
							<p><em>String</em></p>
							<p>This is a class you can make the main div part of to automatically select it for keeping the first headings.</p></li>

							</ul>

						</div>

						<div class="tabpage noborder">
	
							<h4>Methods</h4>
							
							<ul>

							<li><h4>.build( )</h4>
							<p>Render the tab system.</p></li>

							<li><h4>.takeOutOfMemory( )</h4>
							<p>Take it out of memory. (This is called automatically).</p></li>

							<li><h4>.addStyleSheet( uri : String )</h4>
							<p>Add a style sheet. If you do not provide an argument, the .styleSheetUri property will be used, this is inherited from the global version. If you add a stylesheet to a generic instance, The instance will change the generic stylesheet (affecting other generics).</p></li>

							<li><h4>.backup( )</h4>
							<p>Back up the current state of the tabsystem.</p></li>

							<li><h4>.restore( )</h4>
							<p>Restore the tab system from backup.</p></li>

							<li><h4>.styletab( tab : [Object tab] )</h4>
							<p>Set styling on the tab.</p></li>

							<li><h4>.switchTabToId( id : String )</h4>
							<p>Switch to a tab page that has this id. If no corresponding id is found, nothing will happen.</p></li>

							<li><h4>.switchtab( tabIndex : Number, firstTime : Boolean )</h4>
							<p>Switches the tab to the index supplied.  If no corresponding index is found, nothing will happen.</p></li>
		
							<li><h4>.addEventHandler( evt :String , fn :Funtion, obj :DOM-Node, once: Boolean )</h4>
							<p>Add an event handler in the instance scope. If you leave out <em>obj</em>, It will assume the <em>this</em> root.</p></li>

							<li><h4>.removeEventHandler( evt, fn, obj )</h4>
							<p>Removes an event handler in the instance scope. If you leave out <em>obj</em>, It will assume the <em>this</em> root.</p></li>

							<li><h4>.fireEventHandler( evt, obj, args )</h4>
							<p>Fires an event handler in the instance scope. If you leave out <em>obj</em>, It will assume the <em>this</em> root.</p></li>
							</ul>

						</div>

						<div class="tabpage noborder">
	
							<h4>Events</h4>

							<li><h4>tab."onclick"</h4>
							<p>Fires when the tab is clicked.</p></li>

							<li><h4>tab."onmouseover"</h4>
							<p>Fires when the tab is moused.</p></li>

							<li><h4>tab."onmouseout"</h4>
							<p>Fires when on mouse-out of the tab.</p></li>

							<li><h4>tabsystem."onbuild"</h4>
							<p>Fires when the tab system is built.</p></li>

							<li><h4>tabsystem."onunbuild"</h4>
							<p>Fires when the tab system is collapsed.</p></li>

							<li><h4>tabsystem."onstylechange"</h4>
							<p>Fires when the tab system changes stylesheet.</p></li>

						</div>
					</div>
				
					<h3>Global</h3>

					<p>These are the Global Functions and Variables.</p>

					<div class="tabsystem noShift">

						<div class="tabpage noborder">

							<h3>Properties</h3>

							<li><h4>tabSystem.classTrigger</h4></li>
							<li><h4>tabSystem.tabPageClass</h4></li>
							<li><h4>tabSystem.activeTabClass</h4></li>
							<li><h4>tabSystem.hoverTabClass</h4></li>
							<li><h4>tabSystem.normalTabClass</h4></li>
							<li><h4>tabSystem.startIndex</h4></li>
							<li><h4>tabSystem.removeHeadings</h4></li>
							<li><h4>tabSystem.styleSheetUri</h4></li>
							<li><h4>tabSystem.tabContainerClass</h4></li>
							<li><h4>tabSystem.tabSelectedKeyword</h4></li>
							<li><h4>tabSystem.invertOrder</h4>
							<li><h4>tabSystem.firstHeadingClass</h4>
							<li><h4>tabSystem.firstHeadingKeepTrigger</h4>
							<p><em>See instance properties.</em></p></li>

							<ul class="square">

							<li><h4>tabSystem.Dependancies</h4>
							<p><em>Boolean</em></p>
							<p>What Objects, Standards and Components does the tab system need to render?</p></li>
							
							<li><h4>tabSystem.instances</h4>
							<p><em>Array[ Object: Tabsystem , ..n ]</em></p>
							<p>Collection of tabsystems.</p></li>

							<li><h4>tabSystem.instances.length</h4>
							<p><em>Number</em></p>
							<p>Length of tabsystem collection array (Zero based).</p></li>
							</ul>

						</div>


						<div class="tabpage noborder">

							<h3>Functions</h3>

							<ul class="square">

							<li><h4>tabSystem.makeGlobalProps( )</h4>
							<p>This is called automatically at the beginning. Creates all the global properties.</p></li>

							<li><h4>tabSystem.takeOutOfMemory( )</h4>
							<p>Take every tab instance out of memory.</p></li>

							<li><h4>tabSystem.genericStyleSheet( Uri: String )</h4>
							<p>Add in the generic "catch-all" stylesheet.</p></li>

							<li><h4>tabSystem.construct( )</h4>
							<p>Seek and render all catch-all tab divisions.</p></li>

							<li><h4>tabSystem.getTabsetByDivision( div: [Object HTML Node] )</h4>
							<p>Get a tab system instance from an argument of a top-level tab division.</p></li>

							</ul>
				
						</div>
					</div></div>

	
		<div class="tabpage scrollit">

			<h2>Download</h2>

			<p><a href="system.zip">Download the tab system here.</a></p>
			<p><a href="view-source:http://www.developer-x.com/projects/tabs/behaviour/tabs.js">View the commented source here.</a></p>
		</div>

		<div class="tabpage scrollit">

			<h2>Examples</h2>

			<ul class="square">
			<li><a href="multi.asp">Multi-Row Tabs</a></li>
			<li><a href="other.asp">other</a></li>
			<li><a href="encana.asp">Dynamic tabs + API - NEW!</a></li>
			<li><a href="winsor.asp">user-request page (4:49 PM 8/21/2002)</a></li>
			</ul>
		</div>

	<p>This is a footer, It always stays here!</p>

	</div>

	<div id="ctrl">

		<button onclick="tabSystem.instances[0].addStyleSheet('style/luna/winxp.css')">style . winxp</button>
		<button onclick="tabSystem.instances[0].addStyleSheet('style/dev-x/devx.css')">style . dev-x</button>
		<button onclick="tabSystem.instances[0].addStyleSheet('style/another/another.css')">style . other</button>
		<button onclick="tabSystem.instances[0].addStyleSheet('style/none.css')">style . none</button>
	</div>
</div>


	<div id="after">	

		<p>Copyright &copy; 1999-2002 By <strong><a title="Send me an e-mail." href="mailto:tim@developer-x.com">Tim Scarfe</a></strong>.</p>
	</div>

<br /><br />

</body>

</html>