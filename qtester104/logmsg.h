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

#ifndef LOGMSG_H
#define LOGMSG_H

// Buffered  message

#include <time.h>
#include <list>
#include <string>

class TLogMsg
{
public:
    TLogMsg();
    void pushMsg(const char * msg, unsigned int level=0); // level: 0=less important
    std::string pullMsg();
    void activateLog();
    void deactivateLog();
    void doLogTime();
    void dontLogTime();
    void setMaxMsg(unsigned int maxmsg);
    bool haveMsg();
    void setLevel(unsigned int nivel); // set exibition level
    bool isLogging();
    int count();

private:
    std::list <std::string> mLstLog;
    std::list <time_t> mLstTime;
    unsigned int mMaxMsg;
    bool mDoLog;
    bool mRegTime;
    unsigned int mLevel; // exibition level 0=all, 1 an on, exibit more information progressively
};

#endif // LOGMSG_H
