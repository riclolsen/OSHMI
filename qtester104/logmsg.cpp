/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010,2011,2012 Ricardo L. Olsen
 *
 * Disclaimer
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "logmsg.h"

using namespace std;

TLogMsg::TLogMsg()
{
    mMaxMsg = 1000;
    mDoLog = true;
    mRegTime = false;
    mLevel = 0;
}

void TLogMsg::setMaxMsg(unsigned int maxmsg)
{
    mMaxMsg = maxmsg;
}

void TLogMsg::setLevel(unsigned int level)
{
    mLevel = level;
}

void TLogMsg::activateLog()
{
    mDoLog = true;
}

void TLogMsg::deactivateLog()
{
    mLstLog.clear(); // clean lists
    mLstTime.clear();
    mDoLog = false;
}

void TLogMsg::doLogTime()
{
    mLstLog.clear(); // clean lists, sync
    mLstTime.clear();
    mRegTime = true;
}

void TLogMsg::dontLogTime()
{
    mRegTime = false;
}

bool TLogMsg::haveMsg()
{
    return !mLstLog.empty();
}

bool TLogMsg::isLogging()
{
    return mDoLog;
}

// coloca a mensagem na fila
void TLogMsg::pushMsg( const char * msg, unsigned int level )
{
    if ( mDoLog && ( mLstLog.size() < mMaxMsg ) && ( mLevel <= level ) ) {
        mLstLog.push_back( msg );
        if ( mRegTime ) { // coloca hora na fila, se for o caso
            mLstTime.push_back( time( NULL ) );
        }
    }
}

int TLogMsg::count()
{
    return mLstLog.size();
}

// Tira mensagem da fila
string TLogMsg::pullMsg()
{
    if ( mLstLog.empty() || !mDoLog )
        return "";

    string s = mLstLog.front();   // pega a primeira da fila
    mLstLog.pop_front();          // retira-a da fila

    // se tem registro de hora, pega a hora e formata para exibir antes da mensagem
    if (mRegTime){
        char buffer [201];
        static time_t hora_ant;
        time_t hora = mLstTime.front();
        mLstTime.pop_front();
        if (hora != hora_ant)
          {
          struct tm * timeinfo;
          timeinfo = localtime ( &hora );
          // strftime ( buffer,200,"%d/%m %H:%M:%S ",timeinfo );
          strftime ( buffer,200,"%H:%M:%S ",timeinfo );
          s = buffer + s;
          }
        else
          s = "         " + s;
        hora_ant = hora;
    }

    return s;
}

