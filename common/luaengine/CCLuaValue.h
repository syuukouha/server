/****************************************************************************
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
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
 ****************************************************************************/

#ifndef __CC_LUA_VALUE_H_
#define __CC_LUA_VALUE_H_

#include <string>
#include <map>
#include <list>
#include "lua.hpp"
#include <string.h>

typedef int LUA_FUNCTION;
typedef int LUA_TABLE;
typedef int LUA_STRING;
typedef void Ref;
class LuaValue;

typedef std::map<std::string, LuaValue>   LuaValueDict;
typedef LuaValueDict::const_iterator      LuaValueDictIterator;
typedef std::list<LuaValue>               LuaValueArray;
typedef LuaValueArray::const_iterator     LuaValueArrayIterator;

/// @cond
typedef enum {
    LuaValueTypeInt,
    LuaValueTypeFloat,
    LuaValueTypeBoolean,
    LuaValueTypeString,
    LuaValueTypeDict,
    LuaValueTypeArray,
    LuaValueTypeObject
} LuaValueType;
/// @endcond

/// @cond
typedef union {
    int                 intValue;
    float               floatValue;
    bool                booleanValue;
    std::string*        stringValue;
    LuaValueDict*       dictValue;
    LuaValueArray*      arrayValue;
    Ref*               ccobjectValue;
} LuaValueField;
/// @endcond

/**
 * Wrap different general types of data into a same specific type named LuaValue.
 * The general types supported as follows:int,float,bool,std::string,const char*,LuaValueDict,LuaValueArray,Ref.
 *
 * @lua NA
 * @js NA
 */
class LuaValue
{
public:
    
    /**
     * Construct a LuaValue object by a int value.
     *
     * @param intValue a int value.
     * @return a LuaValue object.
     */
    static const LuaValue intValue(const int intValue);
    
    /**
     * Construct a LuaValue object by a float value.
     *
     * @param floatValue a float value.
     * @return a LuaValue object.
     */
    static const LuaValue floatValue(const float floatValue);
    
    /**
     * Construct a LuaValue object by a boolean value.
     *
     * @param booleanValue a bool value.
     * @return a LuaValue object.
     */
    static const LuaValue booleanValue(const bool booleanValue);
    
    /**
     * Construct a LuaValue object by a string pointer.
     *
     * @param stringValue a string pointer.
     * @return a LuaValue object.
     */
    static const LuaValue stringValue(const char* stringValue);
    
    static const LuaValue stringValue(const char* stringValue, int sz);
    /**
     * Construct a LuaValue object by a std::string object.
     *
     * @param stringValue a std::string object.
     * @return a LuaValue object.
     */
    static const LuaValue stringValue(const std::string& stringValue);
    
    /**
     * Construct a LuaValue object by a LuaValueDict value.
     *
     * @param dictValue a LuaValueDict object.
     * @return a LuaValue object.
     */
    static const LuaValue dictValue(const LuaValueDict& dictValue);
    
    /**
     * Construct a LuaValue object by a LuaValueArray value.
     *
     * @param arrayValue a LuaValueArray object.
     * @return a LuaValue object.
     */
    static const LuaValue arrayValue(const LuaValueArray& arrayValue);
    
    /**
     * Construct a LuaValue object by a Ref object.
     *
     * @param ccobjectValue a Ref object.
     * @param objectTypename a string pointer point to the typename of object.
     * @return a LuaValue object.
     */
    static const LuaValue ccobjectValue(Ref* ccobjectValue/*, const char* objectTypename*/);
    
    /**
     * Default constuctor of LuaValue.
     * Set the default value for _type(LuaValueTypeInt) and _ccobjectType(nullptr),and init the _field.
     */
    LuaValue(void)
    : _type(LuaValueTypeInt)
    {
        memset(&_field, 0, sizeof(_field));
    }
    
    /**
     * Copy constructor of Data.
     */
    LuaValue(const LuaValue& rhs);
    
    /**
     * Override of operator= .
     */
    LuaValue& operator=(const LuaValue& rhs);
    
    /**
     * Destructor.
     */
    ~LuaValue(void);
    
    /**
     * Get the type of LuaValue object.
     * 
     * @return the type of LuaValue object.
     */
    const LuaValueType getType(void) const {
        return _type;
    }
    
    /**
     * Get the int value of LuaValue object.
     *
     * @return the int value.
     */
    int intValue(void) const {
        return _field.intValue;
    }
    
    /**
     * Get the float value of LuaValue object.
     *
     * @return the float value.
     */
    float floatValue(void) const {
        return _field.floatValue;
    }
    
    /**
     * Get the boolean value of LuaValue object.
     *
     * @return the boolean value.
     */
    bool booleanValue(void) const {
        return _field.booleanValue;
    }
    
    /**
     * Get the boolean value of LuaValue object.
     *
     * @return the refrence about string value.
     */
    const std::string& stringValue(void) const {
        return *_field.stringValue;
    }
    
    /**
     * Get the LuaValueDict value of LuaValue object.
     *
     * @return the LuaValueDict value.
     */
    const LuaValueDict& dictValue(void) const {
        return *_field.dictValue;
    }
    
    /**
     * Get the LuaValueArray value of LuaValue object.
     *
     * @return the LuaValueArray value.
     */
    const LuaValueArray& arrayValue(void) const {
        return *_field.arrayValue;
    }
    
    /**
     * Get the Ref object of LuaValue object.
     *
     * @return the pointer point to a Ref object.
     */
    Ref* ccobjectValue(void) const {
        return _field.ccobjectValue;
    }

    /**
     * 将值压入数据栈
     */
    void pushToStack(lua_State* L) const {
        switch(getType())
        {
        case LuaValueTypeInt:
            lua_pushinteger(L, intValue());
            break;
        case LuaValueTypeBoolean:
            lua_pushboolean(L, booleanValue());
            break;
        case LuaValueTypeFloat:
            lua_pushnumber(L, floatValue());
            break;
        case LuaValueTypeObject:
            lua_pushlightuserdata(L, ccobjectValue());
            break;
        case LuaValueTypeString:
            if (_field.stringValue != nullptr) {
              lua_pushlstring(L, _field.stringValue->c_str(), _field.stringValue->size());
            } else {
              lua_pushlstring(L, "", 0);
            }
            break;
        default:
            lua_pushnil(L);
            break;
        }
    }

    static LuaValue toValue(lua_State* L, int idx) 
    {
        double v = 0;
        switch(lua_type(L, idx)) 
        {
        case LUA_TNIL:
            return LuaValue::ccobjectValue(nullptr);
        case LUA_TBOOLEAN:
            return LuaValue::booleanValue(lua_toboolean(L, idx));
        case LUA_TNUMBER:
            v = lua_tonumber(L, idx);
            if (v == (int)v)
                return LuaValue::intValue((int)v);
            return LuaValue::floatValue(lua_tonumber(L, idx));
        case LUA_TSTRING:
            return LuaValue::stringValue(lua_tostring(L, idx));
        case LUA_TUSERDATA:
            return LuaValue::ccobjectValue(lua_touserdata(L, idx));
        default:
            break;
        }

        return LuaValue::intValue(0); // reserved
    }
private:
    LuaValueField _field;
    LuaValueType  _type;
    
    void copy(const LuaValue& rhs);
};

//NS_CC_END

// end group
/// @}
#endif // __CC_LUA_VALUE_H_
