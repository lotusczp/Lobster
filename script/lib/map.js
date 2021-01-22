/*
 * interfaces£º
 * size()               Returns the number of (key, value) pairs in the map.
 * isEmpty()            Returns true if the map contains no items; otherwise returns false.
 * clear()              Removes all items from the map.
 * put(key, value)      insert pair£¨key, value) into the map 
 * remove(key)          Removes all the items that have the key key from the map, 
 *                      return true if succeed, otherwise return false
 * get(key)             return of value of the key "key", return NULL if fail
 * element(index)       return the pair of index
 *                      use element.key£¬element.value to get kay and value, return NULL if fail
 * containsKey(key)     Returns true if the map contains an item with key "key"; otherwise returns false.
 * containsValue(value) Returns true if the map contains an item with value "value"; otherwise returns false.
 * values()             Returns an array containing all the values in the map,
 * keys()               Returns an array containing all the keys in the map,
 *
 * example£º
 * var map = new Map();
 *
 * map.put("key", "value");
 * var val = map.get("key")
 * ¡­¡­
 *
 */
function Map() {
    this.elements = new Array();
 
    this.size = function() {
        return this.elements.length;
    };
 
    this.isEmpty = function() {
        return (this.elements.length < 1);
    };
 
    this.clear = function() {
        this.elements = new Array();
    };
 
    this.put = function(_key, _value) {
        this.elements.push( {
            key : _key,
            value : _value
        });
    };
 
    this.remove = function(_key) {
        var bln = false;
        try {
            for (var i = 0; i < this.elements.length; i++) {
                if (this.elements[i].key == _key) {
                    this.elements.splice(i, 1);
                    return true;
                }
            }
        } catch (e) {
            bln = false;
        }
        return bln;
    };
 
    this.get = function(_key) {
        try {
            for (var i = 0; i < this.elements.length; i++) {
                if (this.elements[i].key == _key) {
                    return this.elements[i].value;
                }
            }
        } catch (e) {
            return null;
        }
    };
 
    this.element = function(_index) {
        if (_index < 0 || _index >= this.elements.length) {
            return null;
        }
        return this.elements[_index];
    };
 
    this.containsKey = function(_key) {
        var bln = false;
        try {
            for (var i = 0; i < this.elements.length; i++) {
                if (this.elements[i].key == _key) {
                    bln = true;
                }
            }
        } catch (e) {
            bln = false;
        }
        return bln;
    };
 
    this.containsValue = function(_value) {
        var bln = false;
        try {
            for (var i = 0; i < this.elements.length; i++) {
                if (this.elements[i].value == _value) {
                    bln = true;
                }
            }
        } catch (e) {
            bln = false;
        }
        return bln;
    };
 
    this.values = function() {
        var arr = new Array();
        for (var i = 0; i < this.elements.length; i++) {
            arr.push(this.elements[i].value);
        }
        return arr;
    };
 
    this.keys = function() {
        var arr = new Array();
        for (var i = 0; i < this.elements.length; i++) {
            arr.push(this.elements[i].key);
        }
        return arr;
    };
}
