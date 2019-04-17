<html>

<head>
    <title>OSHMI Historical Data Feed</title>
    <meta http-equiv="Cache-Control" content="no-store" />
   
    <link rel="stylesheet" href="lib/ui-1.9.2/themes/base/jquery.ui.all.css">
    <link href="lib/bootstrap.min.css" rel="stylesheet" crossorigin="anonymous">
    <script src="lib/jquery-2.1.1.js"></script>
    <script src="lib/bootstrap.min.js" crossorigin="anonymous"></script>
    <script src="lib/tableauwdc-2.3.js" type="text/javascript"></script>
    <script src="tableau_wdc.js" type="text/javascript"></script>
    <script src="lib/ui-1.9.2/jquery.ui.core.js"></script>
    <script src="lib/ui-1.9.2/jquery.ui.datepicker.js"></script>
    <script>
    
    // Script para implementar um 'SUGEST AS YOU TYPE' 
    // busca sugestão para o TAG do ponto na medida em que se digita

    var req = null;
    var inp = null;

    function isInt(myNum) {
             // get the modulus: if it's 0, then it's an integer
             var myMod = myNum % 1;

             if (myMod == 0) {
                     return true;
             } else {
                     return false;
             }
    }

    function findPosX(obj)
    {
      var curleft = 0;
      if (obj.offsetParent)
      {
        while (obj.offsetParent)
        {
          curleft += obj.offsetLeft
          obj = obj.offsetParent;
        }
      }
      else if (obj.x)
        curleft += obj.x;
      return curleft;
    }

    function findPosY(obj)
    {
      var curtop = 0;
      if (obj.offsetParent)
      {
        while (obj.offsetParent)
        {
          curtop += obj.offsetTop
          obj = obj.offsetParent;
        }
      }
      else if (obj.y)
        curtop += obj.y;
      return curtop;
    }

    function loadXMLDoc(inputn) {

       inp=inputn; 
       
       if (isInt(inp))
         return;
       
       var val = inp.value.substring(inp.value.lastIndexOf(",")+1);
       
       url='tagfind.php?q='+val+'&inp='+inp.id;
        
       // Internet Explorer
       try { req = new ActiveXObject("Msxml2.XMLHTTP"); }
       catch(e) {
          try { req = new ActiveXObject("Microsoft.XMLHTTP"); }
          catch(oc) { req = null; }
       }

       // Mozailla/Safari
       if (req == null && typeof XMLHttpRequest != "undefined") {
          req = new XMLHttpRequest();
       }
       // Call the processChange() function when the page has loaded
       if (req != null) {
          req.onreadystatechange = processChange;
          req.open("GET", url, true);
          req.send(null);
       }
    }

    function processChange(evt) {
       // The page has loaded and the HTTP status code is 200 OK
       if (req.readyState == 4) {
          if (req.status == 200) {
          // Write the contents of this URL to the searchResult layer
          getObject("lista_op").style.left = findPosX(inp) + "px"; 
          getObject("lista_op").style.top = (findPosY(inp) + inp.offsetHeight) + "px";
          getObject("lista_op").innerHTML = req.responseText;
          }
       }
    }

    function getObject(name) {
       return document.getElementById(name);
    }    
    
    $(document).ready(function() { 
    
      $( "#fromdate" ).datepicker(); 
      $( "#fromdate" ).datepicker( "option", "yearRange", "2017" );
      $( "#fromdate" ).datepicker( "option", "changeYear", true );   
      $( "#fromdate" ).datepicker( 'setDate', new Date() );    
      $( "#fromdate" ).datepicker( "option", "dateFormat", "dd-m-yy" );
      $( "#todate" ).datepicker(); 
      $( "#todate" ).datepicker( "option", "yearRange", "2017" );
      $( "#todate" ).datepicker( "option", "changeYear", true );    
      $( "#todate" ).datepicker( 'setDate', new Date() );    
      $( "#todate" ).datepicker( "option", 'dateFormat', "dd-m-yy" );

    });
    
    </script>
</head>

<body>
    <div class="container container-table">
        <div class="row vertical-center-row">
            <div class="text-left col-md-4 col-md-offset-4">
                <br>
                Tags: <input type="text" id="taglist" name="taglist" size="50" onkeyup="loadXMLDoc(this);" autocomplete="off" title="Enter Tag's list comma separated. Start typing and select..."></input><br><br>
                From Date: <input type="text" id="fromdate" name="fromdate" size="12" ></input><br><br>
                To Date: <input type="text" id="todate" name="todate" size="12"></input><br><br>
                Interval: <input type="number" id="interval" name="interval" value="0" size="5" title="Enter period between samples in minutes. Zero to return all data."></input> min<br><br>
                <br>
                <button type="button" id="submitButton" class="btn btn-success" style="margin: 10px;">Get OSHMI Data!</button>
            </div>
        </div>
    </div>
    <div id='lista_op' style='position:absolute'></div>    
</body>

</html>
