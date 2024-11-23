
#ifndef BYTEIO_DOT_H
#define BYTEIO_DOT_H

#include <type_traits>

//
// Definition of a family of template classes used 
// for byte oriented output. Speicialized classes
// need to implement putByte()
//

template<typename T, typename Enable = void>
class output_bytes 
{
public :
  //
  // If you try to instantiate an output_bytes<T>
  // object for a type that doesn't have a specialization,
  // you will get an error.
  //
  output_bytes(...) {
    static_assert( !std::is_void<Enable>::value, "Instantiating output_bytes<> without a specialization" );
  };
public :
  void putByte(char){}
};

template<typename T>
class output_bytes<T,typename std::enable_if<std::is_base_of<std::ostream, T>::value>::type>
{
public :
  output_bytes(T &stream) : m_stream(stream){}
  void putByte(char c)
  {
    m_stream.put(c);
  }
private :
  T &m_stream;
};

//
// Specialization of output_bytes for FILE *
//
template<>
class output_bytes<FILE *>
{
public :
  output_bytes(FILE *pFile)
  : m_pFile(pFile)
  {}
  void putByte(char c)
  {
    putc(c,m_pFile);
  }
private :
  FILE *m_pFile;
};


//
// Definition of a family of template classes used 
// for byte oriented input. Speicialized classes
// need to implement getByte()
//

template<typename T,typename Enable = void>
class input_bytes
{
public :
  //
  // If you try to instantiate an input_bytes<T>
  // object for a type that doesn't have a specialization,
  // you will get an error indicating that you are 
  // trying to use this private constructor. 
  //
  input_bytes(...) {
    static_assert( !std::is_void<Enable>::value, "Instantiating input_bytes<> without a specialization" );
  };
public :
  int getByte();
};

//
// Specialization of input_bytes for class istream
//
template<typename T>
class input_bytes<T,typename std::enable_if<std::is_base_of<std::istream, T>::value>::type>
{
public :
  input_bytes(T &stream) 
  : m_stream(stream)
  {
  }
  int getByte()
  {
    return m_stream.get();
  }
private :
  T &m_stream;
};

//
// Specialization of input_bytes for FILE *
//
template<>
class input_bytes<FILE *>
{
public :
  input_bytes(FILE *pFile) 
  : m_pFile( pFile )
  {}
  int getByte() {
    return getc(m_pFile);
  }
private :
  FILE *m_pFile;
};

#endif //BYTEIO_DOT_H
