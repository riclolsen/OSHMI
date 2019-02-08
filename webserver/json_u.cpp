//---------------------------------------------------------------------------
// Simple UDP JSON Protocol Driver Interface
// Receive / update values (by tag, key or address)
// Register new points
// Send commands
/*
OSHMI - Open Substation HMI
	Copyright 2008-2019 - Ricardo L. Olsen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/  

#include <vcl.h>
#pragma hdrstop

#include "json_u.h"
#include "jsmn.h"
#include "config.h"
#include "eventos_u.h"
#include "bcolocal.h"
#include "vedados.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdUDPBase"
#pragma link "IdUDPServer"
#pragma resource "*.dfm"
TfmJSON *fmJSON;
//---------------------------------------------------------------------------
__fastcall TfmJSON::TfmJSON(TComponent* Owner)
        : TForm(Owner)
{
if ( UDP_JSON_PORT != 0 )
  {
  IdUDPServer1->DefaultPort=UDP_JSON_PORT;
  IdUDPServer1->Active=true;
  }
else
  {
  IdUDPServer1->Active=false;
  }
}

void TfmJSON::logaln( String msg )
{
if ( cbJSONLog->Checked )
  {
  if ( mmJSONLog->Lines->Count > 1500 )
   for ( int i = 0; i < 50; i++ )
      mmJSONLog->Lines->Delete(0);

  mmJSONLog->Lines->Add( msg.c_str() );
  }
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if ( /* tok->type == JSMN_STRING && */ (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

//---------------------------------------------------------------------------
void __fastcall TfmJSON::IdUDPServer1UDPRead(TObject *Sender,
      TStream *AData, TIdSocketHandle *ABinding)
{
if ( IHM_EstaFinalizando() ) return;

// test message origin (must be from one of the configured endpoints) 
if (  ABinding->PeerIP != UDP_JSON_ENDPOINT1 &&
      ABinding->PeerIP != UDP_JSON_ENDPOINT2
   )
  {
  logaln( (String)"> Unauthorized endpoint: " + ABinding->PeerIP );
  return;
  }

char buffer[3000];
int bytesread = AData->Read( buffer, sizeof(buffer) );

if ( bytesread < 5 )
  {
  logaln( (String)"> Message too short from: " + ABinding->PeerIP );
  return;
  }

buffer[bytesread] = 0; // force string termination
String Message=buffer;
Message.Trim();

if ( Message[1] != '{' && Message[1] != '[' ) {
  logaln( (String)"> Invalid JSON Message from: " + ABinding->PeerIP );
  return;
}

if (cbJSONLog->Checked)
  {
  logaln( (String)"> Message from: " + ABinding->PeerIP );
  logaln( Message );
  }

int i;
int r;
jsmn_parser p;
jsmntok_t t[512]; /* We expect no more than that tokens */

jsmn_init(&p);
r = jsmn_parse(
               &p,
               Message.c_str(),
               Message.Length(),
               t,
               sizeof(t)/sizeof(t[0])
               );
if (r < 0)
  {
  logaln( (String)"> Parse error on JSON message from: " + ABinding->PeerIP );
  return;
  }

fmVeDados->PulseJSON();  

String S;
String tag="";
int point_key=0;
unsigned int utr=0;
int address=0;
float value=0;
bool qualif_ok=true;
bool isBinary=false;

/* Loop over all keys of the root object */
for (i = 0; i < r; i++) {

  switch ( t[i].type ) {
    case JSMN_STRING:
      if (jsoneq(Message.c_str(), &t[i], "tag") == 0) {
        // found a tag definition
        // if next is string then it is the tag
        if ( i+1<r && t[i+1].type == JSMN_STRING ) {
          tag.sprintf("%.*s", t[i+1].end-t[i+1].start,  Message.c_str() + t[i+1].start);
          i++; // jump
          }
         else {
           // invalid
         }
      }
      else if (jsoneq(Message.c_str(), &t[i], "point_key") == 0) {
        // shall find point by key (nponto)
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {
          point_key = atol(Message.c_str() + t[i+1].start);
          i++; // jump
          }
        else {
          // invalid
          }
      }
      else if (jsoneq(Message.c_str(), &t[i], "address") == 0) {
        // shall find point by address/rtu
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {
          address = atol(Message.c_str() + t[i+1].start);
          i++;
        }
      }
      else if (jsoneq(Message.c_str(), &t[i], "value") == 0) {
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {
          isBinary=false;
          // value can be a float or true/false boolean
          if ( jsoneq(Message.c_str(), &t[i+1], "true") == 0 )
            { value = 1; isBinary=true; }
          else
          if ( jsoneq(Message.c_str(), &t[i+1], "false") == 0 )
            { value = 0; isBinary=true; }
          else
            value = atof(Message.c_str() + t[i+1].start);

          i++;
          }
      }
      else if (jsoneq(Message.c_str(), &t[i], "rtu") == 0) {
        // shall find point by address/rtu
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {
          utr = atol(Message.c_str() + t[i+1].start);
          i++;
        }
      }
      else if (jsoneq(Message.c_str(), &t[i], "failed") == 0) {
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {
          // failed can be true/false boolean
          if ( jsoneq(Message.c_str(), &t[i+1], "true") == 0 )
            qualif_ok = false;
          else
            qualif_ok = true;

          i++;
          }
      }
      else if (jsoneq(Message.c_str(), &t[i], "timestamp") == 0) {
      }
      else if (jsoneq(Message.c_str(), &t[i], "ms") == 0) {
      }
      else if (jsoneq(Message.c_str(), &t[i], "time_failed") == 0) {
      }
      else {
        // if next token is primitive then it is the value
        //  and current token is the tag
        if ( i+1<r && t[i+1].type == JSMN_PRIMITIVE ) {

          tag.sprintf("%.*s", t[i].end-t[i].start,  Message.c_str() + t[i].start);

          isBinary=false;
          // value can be a float or true/false boolean
          if ( jsoneq(Message.c_str(), &t[i+1], "true") == 0 )
            { value = 1; isBinary=true; }
          else
          if ( jsoneq(Message.c_str(), &t[i+1], "false") == 0 )
            { value = 0; isBinary=true; }
          else
            value = atof(Message.c_str() + t[i+1].start);

          // process tag and value then
          process_point(point_key, tag.c_str(), address, utr, value, qualif_ok, isBinary);

          i++; // jump
          tag="";
          point_key=0;
          value=0;
          utr=0;
          address=0;
          }
      }
      break;
    case JSMN_PRIMITIVE:
      // invalid
      break;

    case JSMN_ARRAY:
    case JSMN_OBJECT:
      // process tag and value then
      // register tag if not found, as binary or analog
      if ( tag != "" || point_key != 0 || address !=0 ) {
        process_point(point_key, tag.c_str(), address, utr, value, qualif_ok, isBinary);
        tag="";
        value=0;
        point_key=0;
        utr=0;
        address=0;
      }
      break;
    case JSMN_UNDEFINED:
    default:
      break;
    }

  }
if ( tag != "" || point_key != 0 || address != 0 )
  process_point(point_key, tag.c_str(), address, utr, value, qualif_ok, isBinary);

}
//---------------------------------------------------------------------------

void TfmJSON::process_point( int nponto, char * tagstr, unsigned int address, unsigned int utr, float valor, bool quality_ok, bool is_binary )
{
   TFA_Qual qual;
   qual.Byte = 0;
   qual.Tipo = 0;
   bool found = false;
   char tag[TPONTO_TAG_SIZE];

   if ( tagstr[0] != 0 ) // addressed by tag
     {
     // limit tag sring size
     strncpy( tag, tagstr, TPONTO_TAG_SIZE - 1 );
     tag[TPONTO_TAG_SIZE-1]=0;

     if (cbJSONLog->Checked)
       logaln( (String)"point_key: " + (String) nponto + (String)" tag: " + (String)tag + (String)" value: " + (String)valor + (String)" qual_ok: " + (String)(int)quality_ok );

     // Test if point is registered in the database
     TPonto pt = BL.GetRefPontoByTag(tag, found);
     if ( found )
       {
       if ( pt.EhComando() )
         {
         // If point is of command,  treat it as an ACK
         BL.SetAckCmd( pt.NPonto, quality_ok ? 0 : 1, valor ? 2 : 1 );
         return;
         }

       if ( is_binary )
         {
         qual.Estado = !( (int)valor ); // simples: vai para o evento como se fosse duplo 01 ou 10
         qual.EstadoH = !qual.Estado;
         qual.Falha = !quality_ok;
         BL.EscrevePonto( pt.NPonto, qual.Estado, qual.Byte, 0, 1, 0, 1 );
         }
       else
         {
         qual.Tipo = 1;
         qual.Falha = !quality_ok;
         BL.EscrevePonto( pt.NPonto, valor, qual.Byte, 0, 1, 0, 1 );
         }
       }
     else
       { // point not found, create in database
       
       map <int, TPonto> mp = BL.GetMapaPontos();
       int nextpoint = mp.rbegin()->second.NPonto + 1; // greater key from map + 1
       if ( is_binary )
         {
         qual.Estado = !( (int)valor ); // simples: vai para o evento como se fosse duplo 01 ou 10
         qual.EstadoH = !qual.Estado;
         qual.Falha = !quality_ok;
         // call EscrevePonto 2 times: 1 for register and 1 for update
         BL.EscrevePonto(nextpoint, qual.Estado, qual.Byte, 1);
         BL.EscrevePonto(nextpoint, qual.Estado, qual.Byte, 1);
         TPonto &pt = BL.GetRefPonto(nextpoint);
         pt.TipoAD = 'D';
         strcpy(pt.Modulo, "DIGITALS");
         strcpy(pt.EstadoOff, "OFF");
         strcpy(pt.EstadoOn, "ON");
         pt.Endereco = nextpoint;
         strcpy(pt.Estacao, "JSON");
         strncpy( pt.Tag, tag, sizeof(pt.Tag) - 1 );
         pt.Tag[sizeof(pt.Tag)-1] = '\0';
         strncpy( pt.Descricao, tag, sizeof(pt.Descricao) - 1 );
         BL.MapNPontoPorTag[pt.Tag] = nextpoint;
         }
       else
         {
         qual.Tipo = 1;
         qual.Falha = !quality_ok;
         // call EscrevePonto 2 times: 1 for register and 1 for update
         BL.EscrevePonto(nextpoint, valor, qual.Byte, 1);
         BL.EscrevePonto(nextpoint, valor, qual.Byte, 1);
         TPonto &pt = BL.GetRefPonto(nextpoint);
         pt.TipoAD = 'A';
         strcpy(pt.Modulo, "ANALOGS");
         strcpy(pt.Unidade, "Unit");
         pt.Endereco = nextpoint;
         strcpy(pt.Estacao, "JSON");
         strncpy( pt.Tag, tag, sizeof(pt.Tag) - 1 );
         pt.Tag[sizeof(pt.Tag)-1] = '\0';
         strncpy( pt.Descricao, tag, sizeof(pt.Descricao) - 1 );
         BL.MapNPontoPorTag[pt.Tag] = nextpoint;
         }
       BL.ListaSEs.insert("JSON");
       }
     }
   else
   if ( nponto > 0 || address > 0 ) // update point by numeric key
     {
     if ( address > 0 && nponto == 0 )
       {
       // converte endereço físico para nponto
       nponto = BL.NPontoPorEndUTR( address, utr );
       if ( nponto == 0 )
         return; // not found
       }

     // Test if point is registered in the database
     TPonto pt = BL.GetRefPonto(nponto, found);
     if ( found )
       {
       if ( pt.EhComando() )
         {
         // If point is of command,  treat it as an ACK
         BL.SetAckCmd( pt.NPonto, quality_ok ? 1 : 0, valor ? 2 : 1 );
         return;
         }

       if ( is_binary )
         {
         qual.Estado = !( (int)valor ); // simples: vai para o evento como se fosse duplo 01 ou 10
         qual.EstadoH = !qual.Estado;
         qual.Falha = !quality_ok;
         BL.EscrevePonto( pt.NPonto, qual.Estado, qual.Byte, 0, 1, 0, 1 );
         }
       else
         {
         qual.Tipo = 1;
         qual.Falha = !quality_ok;
         BL.EscrevePonto( pt.NPonto, valor, qual.Byte, 0, 1, 0, 1 );
         }
       }
     else
       { // point not found, create in database
       
       int nextpoint = nponto;
       if ( is_binary )
         {
         qual.Estado = !( (int)valor ); // simples: vai para o evento como se fosse duplo 01 ou 10
         qual.EstadoH = !qual.Estado;
         qual.Falha = !quality_ok;
         // call EscrevePonto 2 times: 1 for register and 1 for update
         BL.EscrevePonto(nextpoint, qual.Estado, qual.Byte, 1);
         BL.EscrevePonto(nextpoint, qual.Estado, qual.Byte, 1);
         TPonto &pt = BL.GetRefPonto(nextpoint);
         pt.TipoAD = 'D';
         strcpy(pt.Modulo, "DIGITALS");
         strcpy(pt.EstadoOff, "OFF");
         strcpy(pt.EstadoOn, "ON");
         pt.Endereco = nextpoint;
         strcpy(pt.Estacao, "JSON");
         sprintf( pt.Tag, "JSON_%d", nextpoint );
         strncpy( pt.Descricao, pt.Tag, sizeof(pt.Descricao) - 1 );
         BL.MapNPontoPorTag[pt.Tag] = nextpoint;
         if ( address > 0 )
           BL.MapNPontoPorEndUTR[ (utr << 24) | address ] = nextpoint;
         }
       else
         {
         qual.Tipo = 1;
         qual.Falha = !quality_ok;
         // call EscrevePonto 2 times: 1 for register and 1 for update
         BL.EscrevePonto(nextpoint, valor, qual.Byte, 1);
         BL.EscrevePonto(nextpoint, valor, qual.Byte, 1);
         TPonto &pt = BL.GetRefPonto(nextpoint);
         pt.TipoAD = 'A';
         strcpy(pt.Modulo, "ANALOGS");
         strcpy(pt.Unidade, "Unit");
         pt.Endereco = nextpoint;
         strcpy(pt.Estacao, "JSON");
         sprintf( pt.Tag, "JSON_%d", nextpoint );
         strncpy( pt.Descricao, pt.Tag, sizeof(pt.Descricao) - 1 );
         BL.MapNPontoPorTag[pt.Tag] = nextpoint;
         if ( address > 0 )
           BL.MapNPontoPorEndUTR[ (utr << 24) | address ] = nextpoint;
         }
       BL.ListaSEs.insert("JSON");
       }

     }

}

// Send command via UDP/JSON
// For binary command val: 0=OFF=false 1=ON=true 
int TfmJSON::Command( TPonto &pt, double val )
{
char buff[1400];

if ( pt.TipoAD=='D' || (pt.CodOCR == CODOCR_TAP && pt.CodTpEq == CODTPEQ_TAP) ) 
  sprintf( buff,
           "{ \"tag\": \"%s\", \"point_key\": %d, \"address\": %d, \"rtu\": %d, \"asdu\": %d, \"sbo\": %s, \"value\": %d, \"logic_val\": %s, \"action\": \"%s\" }",
           pt.Tag,
           pt.NPonto,
           pt.Endereco,
           pt.UTR,
           pt.CmdASDU,
           pt.CmdSBO?"true":"false",
           (int)val,
           (((int)val)==0)?"false":"true",
           (((int)val)==0)?pt.EstadoOff:pt.EstadoOn
           );
else
  sprintf( buff,
           "{ \"tag\": \"%s\", \"point_key\": %d, \"address\": %d, \"rtu\": %d, \"asdu\": %d, \"sbo\": %s, \"value\": %f, \"unit\": \"%s\" }",
           pt.Tag,
           pt.NPonto,
           pt.Endereco,
           pt.UTR,
           pt.CmdASDU,
           pt.CmdSBO?"true":"false",
           val,
           pt.Unidade
           );

// send message to Endpoint(s)
IdUDPServer1->SendBuffer( UDP_JSON_ENDPOINT1, UDP_JSON_PORT_CMD, buff, strlen(buff)+1 );
if ( UDP_JSON_ENDPOINT2 != "" )
  {
  IdUDPServer1->SendBuffer( UDP_JSON_ENDPOINT2, UDP_JSON_PORT_CMD, buff, strlen(buff)+1 );
  }

if (cbJSONLog->Checked)
  {
  logaln( (String)"< Command: " + (String)buff );
  }

return 0;
}

