QTester104 Copyright © 2010-2019 Ricardo L. Olsen.

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

This software implements an IEC 60870-5-104 protocol tester.
IEC 60870-5-104 is a commonly used protocol for data acquisition and control of power substations.

Directories:
src - multiplatform qt sources, compile with QT 5.13.1 or later on any supported platform.
bin - win32 binaries built with QT 5.13.1.
conf - configuration file

Can be used to poll substation data and issuing commands.
The base protocol classes are pure C++ without any platform or non-standard library dependencies, can be used to develop software with other compiler/library/sdk.
The software is part of a full substation HMI project (OSHMI - Open Substation HMI), it functions as a protocol module for the HMI, but can be used as a standalone protocol tester.

It's possible to send a General Interrogation ([GI] button) and log messages on the screen. 
At the right panel is shown a table with the following data columns:
Protocol [Address] | Last acquired [Value] | ASDU [Type] | [Cause] of transmission | Quality [Flags] | [Count] of transmissions.

The protocol quality flags are:
on / off / tra (double point transition) / ind (double point indefinite) / bl (blocked) / iv (invalid) / nt (not topical) / sb (substituted) / ov (overflow) / t (reg.step transition).

An optional configuration file can be used to store basic parameters, in the form of a INI file (qtester104.ini):

[BDTR]
; 0 (default): Normal operation mode
; 1: Force to be primary when in redundant mode
; FORCE_PRIMARY=0

[IEC104] 
PRIMARY_ADDRESS=1        ; link address of the primary station (computer)

[RTU1]                   ; communicates with only one RTU in this version
SECONDARY_ADDRESS=2      ; protocol link address of the RTU
IP_ADDRESS=192.168.1.1   ; IP address of the RTU
; IP_ADDRESS_BACKUP=192.168.1.2   ; IP address of the BACKUP RTU
ALLOW_COMMANDS=1         ; 1=allow sending commands, 0=don't permit commands
; PORT=2404              ; Protocol port (default=2404)
; GI_PERIOD = 330        ; time period in seconds for general interrogations (default=330s)

Thank you for the interest, please fell free to comment and contribute.


 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
