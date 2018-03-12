
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

<div id="main">

	<h1>Dynamically create the puppies..</h1>


	<p><a href="./">Go Back?</a></p>

	<div class="custom-tabsystem" id="my-system">

		<div class="tabpage">

			<h2>Chapter One</h2>

			<p>Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. </p>
		</div>

		<div class="tabpage">

			<h2>Chapter Two</h2>

			<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exercitation ulliam corper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem veleum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel willum lunombro dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. </p>
		</div>

		<div class="tabpage">

			<h2>Chapter Three</h2>

			<p>Li Europan lingues es membres del sam familie. Lor separat existentie es un myth. Por scientie, musica, sport etc., li tot Europa usa li sam vocabularium. Li lingues differe solmen in li grammatica, li pronunciation e li plu commun vocabules. Omnicos directe al desirabilit de un nov lingua franca: on refusa continuar payar custosi traductores. It solmen va esser necessi far uniform grammatica, pronunciation e plu sommun paroles. </p>
			<p>Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exercitation ulliam corper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem veleum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel willum lunombro dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. </p>
		
		</div>
	</div>

	<script type="text/javascript">

		var ts=new tabSystem( document.getElementById("my-system") );

		ts.invertOrder=1;

		ts.addEventHandler( "onbuild", function( e ) { 

					ts.addStyleSheet( "style/custom.css" ); 
				} );


		ts.build( );

	</script>

	<button style="border: 1px black solid; font: 10px verdana; margin: 2em 0;" onclick="ts.rendered?ts.deconstruct():ts.build();">toggle render</button>

</div>


	<div id="after">	

		<p>Copyright &copy; 1999-2002 By <strong><a title="Send me an e-mail." href="mailto:tim@developer-x.com">Tim Scarfe</a></strong>.</p>
	</div>

<br /><br />

</body>

</html>