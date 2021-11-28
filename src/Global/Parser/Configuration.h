/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Configuration.h
 * Author: jhendl
 *
 * Created on November 27, 2021, 4:43 AM
 */

#pragma once
#include <functional>
#include <map>
#include <string>

namespace ph
{
  namespace config
  {
    class BaseCallback
    {
      public:
        virtual auto process( const void* input ) -> void = 0 ;
    };
    
    template<typename Type>
    class FunctionCallback : public BaseCallback
    {
      public:
        FunctionCallback( std::function<void(const Type&)> function ) ;
        auto process( const void* input ) -> void ;
      private:
        std::function<void(const Type&)> callback ;
    };
    
    template<typename Class, typename Type>
    class MethodCallback : public BaseCallback
    {
      public:
        MethodCallback( const Class* class_ptr, std::function<void(const Class&, const Type&)> function ) ;
        auto process( const void* input ) -> void ;
      private:
        std::function<void(const Class&, const Type&)> callback ;
        const Class*                                   ref      ;
    };
    
    
    class Configuration
    {
      public:
        template<typename Type>
        static auto subscribe( const char* key, std::function<void( const Type& )> function ) -> void ;
        
        template<typename Class, typename Type>
        static auto subscribe( const char* key, const Class* ptr, std::function<void( const Class&, const Type& )> method ) -> void ;
        
        static auto initialize( const char* config_path ) -> void ;
        
        static auto reparse() -> void ;
        
        static auto shutdown() -> void ;
      private:
        static std::multimap<std::string, BaseCallback*> callbacks ;
        Configuration() ;
    };
    
    
    template<typename Type>
    auto Configuration::subscribe( const char* key, std::function<void( const Type& )> function ) -> void
    {
      Configuration::callbacks.insert( key, new FunctionCallback<Type>( function ) ) ;
    }
    
    template<typename Class, typename Type>
    auto Configuration::subscribe( const char* key, const Class* ptr, std::function<void( const Class&, const Type& )> method ) -> void
    {
      Configuration::callbacks.insert( key, new MethodCallback<Class, Type>( ptr, method ) ) ;
    }
    
    template<typename Type>
    FunctionCallback<Type>::FunctionCallback( std::function<void(const Type&)> function )
    {
      this->callback = function ;
    }

    template<typename Type>
    auto FunctionCallback<Type>::process( const void* input ) -> void
    {
      this->callback( reinterpret_cast<const Type&>( input ) ) ;
    }

    template<typename Class, typename Type>
    MethodCallback<Class, Type>::MethodCallback( const Class* class_ptr, std::function<void(const Class&, const Type&)> function )
    {
      this->ref      = class_ptr ;
      this->callback = function  ;
    }

    template<typename Class, typename Type>
    auto MethodCallback<Class, Type>::process( const void* input ) -> void
    {
      this->callback( *this->ref, reinterpret_cast<const Type&>( input ) ) ;
    }
  }
}