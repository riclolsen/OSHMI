/* =================================================================================================
 * Listener - by Aaron Boodman
 * 5/23/2002; Queens, NY.
 * http://www.youngpup.net/projects/dhtml/listener/
 * 
// Event listener implimentation for javascript. Works with native events as well as custom events. 
//
// Essentially, Listener hijacks a member of an object and makes it a pointer to an internal 
// Listener method. When the hijacked member is called, Listener fires any number of other methods 
// by proxy in whatever scope you (the script author) specify. All arguments passed to the source 
// event are received by all listeners automatically.
// 
// This amounts to a very powerful event listener setup similar to "signals and slots", but without 
// the confusing nomenclature and lack of distinction between methods and events.
//
//
// syntax:
//
// Listener.add(objSource, strEventName, fpListener, objScope, blnRunOnce);
//   objSource    : the object whose events you are listening for
//   strEventName : the name of the event to listen for ("onclick", "onUpdateComplete", etc)
//   fpListener   : a pointer to the function which should fire in response to 
//                  objSource[sEventName]();
//   objScope     : the object whose scope fpListener should be run in when fired.
//   blnRunOnce   : flag that indicates whether the listener should automatically be removed after 
//                  it is first fired.
//
// Listener.remove(objSource, strEventNae, fpListener, objScope);
//   *see above for args*
//
//
 * http://www.youngpup.net/
 * ============================================================================================== */

// Tim Scarfe :
// Fix to Mozilla bug, remove attrib on listener is being confused with Listener.remove (thanks to "standard" on dhtmlcentral)
// Changed delete arr[x] to arr[x]=null for IE4 support.

function Listener(fp, scope, removing) {
        this.fp = fp;
        this.scope = scope;
        this.removeing = removing;
}

Listener.add = function(oSource, sEvent, fpDest, oScope, bRunOnce) {
        if (!oSource[sEvent] || oSource[sEvent] == null || !oSource[sEvent]._listeners) {
                oSource[sEvent] = function() { Listener.fire(oSource, sEvent, arguments) };
                oSource[sEvent]._listeners = new Array();
        }

        var idx = this.findForEvent(oSource[sEvent], fpDest, oScope);
        if (idx == -1) idx = oSource[sEvent]._listeners.length;
        
        oSource[sEvent]._listeners[idx] = new Listener(fpDest, oScope, bRunOnce);
}

Listener.remove = function(oSource, sEvent, fpDest, oScope) {
        var idx = this.findForEvent(oSource[sEvent], fpDest, oScope);
        if (idx != -1) {
                var iLast = oSource[sEvent]._listeners.length - 1;
                oSource[sEvent]._listeners[idx] = oSource[sEvent]._listeners[iLast];
                oSource[sEvent]._listeners.length--;
        }
}

Listener.findForEvent = function(fpEvent, fpDest, oScope) {
        if (fpEvent._listeners) {
                for (var i = 0; i < fpEvent._listeners.length; i++) {
                        if (fpEvent._listeners[i].scope == oScope && fpEvent._listeners[i].fp == fpDest) {
                                return i;
                        }
                }
        }
        return -1;
}

Listener.fire = function(oSourceObj, sEvent, args) {

	if(oSourceObj&&oSourceObj[sEvent]&&oSourceObj[sEvent]._listeners) { // TRS 

		var lstnr, fp;
		var last = oSourceObj[sEvent]._listeners.length - 1;

		// must loop in reverse, because we might be removing elements as we go.

		for (var i = last; i >= 0; i--) {
			lstnr = oSourceObj[sEvent]._listeners[i];
			fp = lstnr.fp;
                
			fp.apply(lstnr.scope, args);

			if (lstnr.remove) Listener.remove(oSourceObj, sEvent, lstnr.fp, lstnr.scope);
		}
	}
	
	return(-1)
}

// impliment function apply for browsers which don't support it natively
if (!Function.prototype.apply) {
        Function.prototype.apply = function(oScope, args) {
                var sarg = [];
                var rtrn, call;

                if (!oScope) oScope = window;
                if (!args) args = [];

                for (var i = 0; i < args.length; i++) {
                        sarg[i] = "args["+i+"]";
                }

                call = "oScope.__applyTemp__(" + sarg.join(",") + ");";

                oScope.__applyTemp__ = this;
                rtrn = eval(call);
                delete oScope.__applyTemp__;
                return rtrn;
        }
}