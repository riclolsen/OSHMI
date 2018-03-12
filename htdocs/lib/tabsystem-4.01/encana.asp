
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN"
	"http://www.w3.org/TR/REC-html40/strict.dtd">

<html lang="en">

	<head>

		<title>EnCana UK</title>

			<meta http-equiv="Content-Type" content="text/html; charset=utf-8"> 

		<link rel="stylesheet" href="style/encana/encana-basic.css" type="text/css" title="Basic style">

		<!-- Dave's Margin fix for Internet Explorer-->

		<script type="text/javascript" src="behaviour/IEmarginFix.js"></script>

		<!-- add in behaviour w/dependancies first -->

		<script type="text/javascript" src="behaviour/listener.js"></script>
		<script type="text/javascript" src="behaviour/tabs.js"></script>


	</head>

<body>

<div id="main">

	<h1>Buzzard Project</h1>

	<p><a href="./">Go Back?</a></p>

	<div style="margin: 2em 0em; border: 1px #555 dotted; padding: 0.1 1em; border-width: 1px 0px; background: #eee;">

	<p style="text-decoration: underline;">Double click elements to evaluate them.</p>

	<form>
	
	<p><em>Try re-building and deconstructing the nested tabset too, this has an index of 1.</em></p>

		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].deconstruct( );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].build( );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].switchtab( 1 );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].switchtab( tabSystem.instances[0].startIndex );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].removeHeadings=1;tabSystem.instances[0].rebuild( );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].tabs[0].remove( );" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].addStyleSheet('style/luna/winxp2.css');" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
<input type="text" size="80" style="font: 14px verdana; border: 1px #ddd solid;cursor:hand;" title="double click me" value="tabSystem.instances[0].addStyleSheet('style/encana/encana.css');" ondblclick="try{eval(this.value)}catch(ex){alert('error!-'+ex.message)}" />
		
	</form>

	</div>

	<div class="en-tabsystem keepheading" id="my-system">

		<div class="tabpage">

			<h2>Chapter One</h2>

			<p>Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. </p>

		</div>

		<div class="tabpage" style=" overflow: visible;">

			<h2>Chapter Two</h2>

			<div class="en2-tabsystem" id="my-second-system">

				<div class="en2-tabpage" style="height: auto;">

					<h2>Test1</h2>

					<p>Li Europan lingues es membres del sam familie. </p>
				</div>

				<div class="en2-tabpage" style="height: auto;">

					<h2>Test2</h2>

					<p>It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. </p>
				</div>
			</div>

			<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exercitation ulliam corper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem veleum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel willum lunombro dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. </p>
		</div>

		<div class="tabpage">

			<h2>Chapter Three</h2>

			<p>Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. </p>
			<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exercitation ulliam corper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem veleum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel willum lunombro dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. </p>
		
		</div>

	</div>

<div style="margin: 2em 0em; border: 1px #555 dotted; padding: 0.1 1em; border-width: 1px 0px; background: #eee;">

<pre style="color: #333;">tabSystem.__load_1 = function( e ) { alert("load") }

var t1=tabSystem.instances[0];</pre>
	
<pre onclick="add1();alert('added.')" style="cursor: hand;" title="click me">t1.addEventHandler("onbuild", tabSystem.__load_1);</pre>
<pre onclick="rem1();alert('removed.')" style="cursor: hand;" title="click me">t1.removeEventHandler("onbuild", tabSystem.__load_1);</pre>
<pre style="color: #333;"><em>Remember mouseover fires when the tab is'nt active.</em></pre>
<pre onclick="mover();alert('added.')" style="cursor: hand;" title="click me">t1.tabs[0].addEventHandler("onmouseover", function( e ) {
	t1.tabs[0].hClone.firstChild.nodeValue="HAH!";
})</pre>
<pre style="color: #333;"><em>Adding a tab..</em></pre>
<pre onclick="addpage();alert('added.')" style="cursor: hand;" title="click me">
var div=document.createElement("div");
var heading=document.createElement("h2");
heading.appendChild( document.createTextNode("my-tab") );
div.appendChild(heading);
var p=document.createElement("p");
p.appendChild( document.createTextNode("this is my content") );
div.appendChild(p);
t1.addTab(div,t1.tabs[0].tabPage);
</pre>

	</div>
	<p>Hope this gives you a good idea on how to set the system up manually guys, E-mail me if you have any queries - Tim</p>


	<script type="text/javascript">

		tabSystem.tempScope=function( e ) {

			var ts=new tabSystem( document.getElementById("my-system") );

			ts.addEventHandler("onbuild", function( e ) {

				ts.addStyleSheet("style/encana/encana.css");

			}, ts, null );

			ts.build( );


			var ts2=new tabSystem( document.getElementById("my-second-system") );

			ts2.addEventHandler("onbuild", function( e ) {

				ts2.addStyleSheet("style/encana/encana2.css");

			}, ts2, null );

			ts2.build( );
		}

		tabSystem.tempScope( );


tabSystem.__load_1 = function( e ) { alert("load") }
	
function add1(e){
	tabSystem.instances[0].addEventHandler("onbuild", tabSystem.__load_1);
}
function rem1(e){
	tabSystem.instances[0].removeEventHandler("onbuild", tabSystem.__load_1);
}
function removefirsttab(e){
	if(tabSystem.instances[0].tabs[0])tabSystem.instances[0].tabs[0].remove( );
	else throw new Error("tabSystem.instances[0].tabs[0] does not exist.")
}
function mover(e){
tabSystem.instances[0].tabs[0].addEventHandler("onmouseover", function( e ) {

	tabSystem.instances[0].tabs[0].hClone.firstChild.nodeValue="HAH!";
})
}

function addpage(e){
	
	var div=document.createElement("div");
	var heading=document.createElement("h2");
	heading.appendChild( document.createTextNode("my-tab") );
	div.appendChild(heading);
	var p=document.createElement("p");
	p.appendChild( document.createTextNode("this is my content") );
	div.appendChild(p);
	tabSystem.instances[0].addTab(div,tabSystem.instances[0].tabs[0].tabPage);
}

	</script>

</div>

	<div id="after">	

		<p>Copyright &copy; 1999-2002 By <strong><a title="Send me an e-mail." href="mailto:tim@developer-x.com">Tim Scarfe</a></strong>.</p>
	</div>

<br /><br />

</body>

</html>