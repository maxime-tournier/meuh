#ifndef SCRIPT_REF_H
#define SCRIPT_REF_H

namespace script {
  
  template<class Value>
  struct reference {
    Value& value;
    Value get() const { return value; }
    void set(const Value& v) { value = v;} 
  };

  template<class Value>
  reference<Value> ref(Value& v) { return {v}; }
  
}



#endif
