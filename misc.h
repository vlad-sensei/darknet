#ifndef MISC
#define MISC

template <typename T>
T str_to_dec( const std::string &data, T default_val )
{
  if( !data.empty( ))
  {
    T ret;
    std::istringstream iss( data );
    iss >> std::dec >> ret;

    if( iss.fail( ))
    {
      return default_val;
    }
    return ret;
  }
  return T( );
}

#endif // MISC

