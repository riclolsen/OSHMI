/*
The MIT License

Copyright (c) 2007 SitePoint Pty. Ltd. http://www.sitepoint.com/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

var Core = {};

// W3C DOM 2 Events model
if (document.addEventListener)
{
  Core.addEventListener = function(target, type, listener)
  {
    target.addEventListener(type, listener, false);
  };

  Core.removeEventListener = function(target, type, listener)
  {
    target.removeEventListener(type, listener, false);
  };

  Core.preventDefault = function(event)
  {
    event.preventDefault();
  };

  Core.stopPropagation = function(event)
  {
    event.stopPropagation();
  };
}
// Internet Explorer Events model
else if (document.attachEvent)
{
  Core.addEventListener = function(target, type, listener)
  {
    // prevent adding the same listener twice, since DOM 2 Events ignores
    // duplicates like this
    if (Core._findListener(target, type, listener) != -1) return;

    // listener2 calls listener as a method of target in one of two ways,
    // depending on what this version of IE supports, and passes it the global
    // event object as an argument
    var listener2 = function()
    {
      var event = window.event;

      if (Function.prototype.call)
      {
        listener.call(target, event);
      }
      else
      {
        target._currentListener = listener;
        target._currentListener(event)
        target._currentListener = null;
      }
    };

    // add listener2 using IE's attachEvent method
    target.attachEvent("on" + type, listener2);

    // create an object describing this listener so we can clean it up later
    var listenerRecord =
    {
      target: target,
      type: type,
      listener: listener,
      listener2: listener2
    };

    // get a reference to the window object containing target
    var targetDocument = target.document || target;
    var targetWindow = targetDocument.parentWindow;

    // create a unique ID for this listener
    var listenerId = "l" + Core._listenerCounter++;

    // store a record of this listener in the window object
    if (!targetWindow._allListeners) targetWindow._allListeners = {};
    targetWindow._allListeners[listenerId] = listenerRecord;

    // store this listener's ID in target
    if (!target._listeners) target._listeners = [];
    target._listeners[target._listeners.length] = listenerId;

    // set up Core._removeAllListeners to clean up all listeners on unload
    if (!targetWindow._unloadListenerAdded)
    {
      targetWindow._unloadListenerAdded = true;
      targetWindow.attachEvent("onunload", Core._removeAllListeners);
    }
  };

  Core.removeEventListener = function(target, type, listener)
  {
    // find out if the listener was actually added to target
    var listenerIndex = Core._findListener(target, type, listener);
    if (listenerIndex == -1) return;

    // get a reference to the window object containing target
    var targetDocument = target.document || target;
    var targetWindow = targetDocument.parentWindow;

    // obtain the record of the listener from the window object
    var listenerId = target._listeners[listenerIndex];
    var listenerRecord = targetWindow._allListeners[listenerId];

    // remove the listener, and remove its ID from target
    target.detachEvent("on" + type, listenerRecord.listener2);
    target._listeners.splice(listenerIndex, 1);

    // remove the record of the listener from the window object
    delete targetWindow._allListeners[listenerId];
  };

  Core.preventDefault = function(event)
  {
    event.returnValue = false;
  };

  Core.stopPropagation = function(event)
  {
    event.cancelBubble = true;
  };

  Core._findListener = function(target, type, listener)
  {
    // get the array of listener IDs added to target
    var listeners = target._listeners;
    if (!listeners) return -1;

    // get a reference to the window object containing target
    var targetDocument = target.document || target;
    var targetWindow = targetDocument.parentWindow;

    // searching backward (to speed up onunload processing), find the listener
    for (var i = listeners.length - 1; i >= 0; i--)
    {
      // get the listener's ID from target
      var listenerId = listeners[i];

      // get the record of the listener from the window object
      var listenerRecord = targetWindow._allListeners[listenerId];

      // compare type and listener with the retrieved record
      if (listenerRecord.type == type && listenerRecord.listener == listener)
      {
        return i;
      }
    }
    return -1;
  };

  Core._removeAllListeners = function()
  {
    var targetWindow = this;

    for (id in targetWindow._allListeners)
    {
      var listenerRecord = targetWindow._allListeners[id];
      listenerRecord.target.detachEvent(
          "on" + listenerRecord.type, listenerRecord.listener2);
      delete targetWindow._allListeners[id];
    }
  };

  Core._listenerCounter = 0;
}

Core.addClass = function(target, theClass)
{
  if (!Core.hasClass(target, theClass))
  {
    if (target.className == "")
    {
      target.className = theClass;
    }
    else
    {
      target.className += " " + theClass;
    }
  }
};

Core.getElementsByClass = function(theClass)
{
  var elementArray = [];

  if (typeof document.all != "undefined")
  {
    elementArray = document.all;
  }
  else
  {
    elementArray = document.getElementsByTagName("*");
  }

  var matchedArray = [];
  var pattern = new RegExp("(^| )" + theClass + "( |$)");

  for (var i = 0; i < elementArray.length; i++)
  {
    if (pattern.test(elementArray[i].className))
    {
      matchedArray[matchedArray.length] = elementArray[i];
    }
  }

  return matchedArray;
};

Core.hasClass = function(target, theClass)
{
  var pattern = new RegExp("(^| )" + theClass + "( |$)");

  if (pattern.test(target.className))
  {
    return true;
  }

  return false;
};

Core.removeClass = function(target, theClass)
{
  var pattern = new RegExp("(^| )" + theClass + "( |$)");

  target.className = target.className.replace(pattern, "$1");
  target.className = target.className.replace(/ $/, "");
};

Core.getComputedStyle = function(element, styleProperty)
{
  var computedStyle = null;

  if (typeof element.currentStyle != "undefined")
  {
    computedStyle = element.currentStyle;
  }
  else
  {
    computedStyle = document.defaultView.getComputedStyle(element, null);
  }

  return computedStyle[styleProperty];
};

Core.start = function(runnable)
{
  Core.addEventListener(window, "load", runnable.init);
};
