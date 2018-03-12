// Ventoinha Curinguiana
// 
// OSHMI/Open Substation HMI - Copyright 2008-2014 - Ricardo L. Olsen

// Requer no HTML algo assim: <span id='VENTOINHA' title='Se esta ventoinha parar, tecle [F5]!' style='font-family:courier;font-weight:bold;color:red'>(!)</span>
// Inicia no javascript assim: Ventoinha.init("VENTOINHA"); 

var Ventoinha =
{
Vent: [],
VentInd: 0,
SpanId: "VENTOINHA",
Velocidade: 750,
TimerID: 0,
First: 1,
elemVent: {},
elemImg1: {},
TemImagem: false,

init : function( span_id ) 
  {  
    if ( span_id != undefined )
      { Ventoinha.SpanId = span_id; }
     
    Ventoinha.elemVent = document.getElementById( Ventoinha.SpanId );
    Ventoinha.Vent[0] = "(|)";
    Ventoinha.Vent[1] = "(/)";
    Ventoinha.Vent[2] = "(-)";
    Ventoinha.Vent[3] = "(\\)";
  },

periodico : function ()
  {
  setInterval( Ventoinha.tick, Ventoinha.Velocidade );                   
  },

setImage : function ( img_id )
  {
  Ventoinha.elemImg1 = document.getElementById( img_id );
  Ventoinha.TemImagem = true;
  },

tick : function ()
  { 
  Ventoinha.elemVent.textContent = Ventoinha.Vent[++Ventoinha.VentInd%4];
  if ( HA_ALARMES && Ventoinha.TemImagem )
    { Ventoinha.elemImg1.style.webkitTransform = (Ventoinha.VentInd%2)? 'rotateY(180deg)' : 'rotateY(0deg)'; }
  },

pulse : function( msg ) 
  { 
    if ( msg == undefined || msg == "" )
      { msg = "(*)"; }
    else
      { msg = "(" + msg + ")"; }
      
    if ( Ventoinha.elemVent )
      { Ventoinha.elemVent.textContent = msg; }
  } 
};
