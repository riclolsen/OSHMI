var Symbol;
if (!Symbol) {
  Symbol = (function(Object){

    // (C) WebReflection Mit Style License

    var ObjectPrototype = Object.prototype,
        defineProperty = Object.defineProperty,
        prefix = '__simbol' + Math.random() + '__',
        id = 0;

    function get(){/*avoid set w/out get prob*/}

    function Symbol() {
      var __symbol__ = prefix + id++;
      defineProperty(
        ObjectPrototype,
        this._ = __symbol__,
        {
          enumerable: false,
          configurable: false,
          get: get, // undefined
          set: function (value) {
            defineProperty(this, __symbol__, {
              enumerable: false,
              configurable: true,
              writable: true,
              value: value
            });
          }
        }
      );
    }

    defineProperty(Symbol.prototype, 'toString', {
      enumerable: false,
      configurable: false,
      writable: false,
      value: function toString() {
        return this._;
      }
    });

    return Symbol;

  }(Object));
}

