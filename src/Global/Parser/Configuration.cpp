/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Configuration.cpp
 * Author: jhendl
 * 
 * Created on November 27, 2021, 4:43 AM
 */

#include "Configuration.h"
#include "Parser.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <thread>

namespace ph
{
  namespace config
  {
    struct ConfigurationData
    {
      using Time = std::chrono::system_clock::time_point ;
      
      bool        initialized ;
      bool         loop_thread ;
      std::string  filename ;
      Time         modified ;
      json::Parser parser   ;
      json::Token  begin    ;
      json::Token  end      ;
      
      ConfigurationData() ;
      
      auto parse() -> void ;
      
      auto isModified() -> bool ;
      
      auto init( const char* config_path ) -> void ;
      
      auto loop( const std::multimap<std::string, BaseCallback*>& callbacks ) -> void ;
    };
    
    std::multimap<std::string, BaseCallback*> Configuration::callbacks ;
    
    static auto data() -> ConfigurationData&
    {
      static ConfigurationData data ;
      
      return data ;
    }
    
    ConfigurationData::ConfigurationData()
    {
      this->loop_thread = true ;
    }

    auto ConfigurationData::isModified() -> bool
    {
      if( std::filesystem::exists( data().filename ) )
      {
        auto time = std::filesystem::last_write_time( data().filename ) ;
  
        if( data().modified != time )
        {
          data().modified = time ;
          std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) ) ;
          return true ;
        }
      }
      return false ;
    }

    auto ConfigurationData::init( const char* config_path ) -> void
    {
      this->filename = config_path ;
      this->parse() ;
      
      
    }
    
    auto ConfigurationData::parse() -> void 
    {
      std::string   file   ;
      std::ifstream stream ;
      
      std::filesystem::copy_file( this->filename.c_str(), data().filename + ".tmp", std::filesystem::copy_options::skip_existing )  ;
      stream.open( data().filename + ".tmp", std::ifstream::in ) ;
      data().parser.clear() ;
      
      if( stream )
      {
        // Copy stream's contents into string.
        stream.seekg  ( 0, std::ios::end  ) ;
        file  .reserve( stream.tellg()    ) ;
        stream.seekg  ( 0, std::ios::beg  ) ;

        file.assign ( ( std::istreambuf_iterator<char>( stream ) ), std::istreambuf_iterator<char>() ) ;

        // Feed data to parser.
        data().parser.initialize( file.c_str() ) ;
        data().begin = data().parser.begin() ;
        data().end   = data().parser.end  () ;
        
        data().modified = std::filesystem::last_write_time( this->filename.c_str() ) ;
      }
      else
      {
        throw std::runtime_error( "Unable to load configuration file!" ) ;
      }

      stream.close() ;
      std::filesystem::remove( data().filename + ".tmp" ) ;
    }

    auto ConfigurationData::loop( const std::multimap<std::string, BaseCallback*>& callbacks ) -> void
    {
      while( this->loop_thread )
      {
        
        if( this->isModified() )
        {
          this->parse() ;
          for( auto iter = this->begin; iter != this->end; ++iter )
          {
            for( auto& iter2 : callbacks )
            {
              if( iter.key() == iter2.first )
              {
                iter2.second->process( reinterpret_cast<const void*>( iter.number() ) ) ;
              }
            }
          }
        }
      }
    }
    
    auto Configuration::initialize( const char* config_path ) -> void
    {
      data().init( config_path ) ;
      auto& cfg_data = data() ;
      auto thread = std::thread( &ConfigurationData::loop, std::ref( cfg_data ), Configuration::callbacks ) ;
      thread.detach() ;
    }
    
    auto Configuration::reparse() -> void
    {
      data().parse() ;
    }
    
    auto Configuration::shutdown() -> void
    {
      data().loop_thread = false ;
    }
  }
}