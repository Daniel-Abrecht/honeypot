/*
Here is an example pam configuration:

auth    [success=2 default=ignore]  pam_unix.so nullok_secure
auth    optional            pam_exec.so expose_authtok /usr/local/bin/login_logger /var/log/failed_logins
auth    requisite           pam_deny.so
auth    required            pam_permit.so


The logfile entry has the format key\0value\0key\0value\0, and each entry ends whith a newline character instant of a key

watch new login attemps as they come in:
tail -f /var/log/failed_logins | tr '\0\1' '\t '

get the most often used passwords:
grep -aoP 'password\0[^\0]*' /var/honeypot/loginfails | grep -oaP '[^\0]*$' | tr '\0\1' '\t ' | sort | uniq -c | sort -rh | head

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#define IFT( X, T, Y, Z ) _Generic( (X), T: Y, default: Z )
#define FMT_OF( P, X ) \
    IFT( (X), signed char       , P "hhd", \
    IFT( (X), unsigned char     , P "hhu", \
    IFT( (X), signed short      , P "hd" , \
    IFT( (X), unsigned short    , P "hu" , \
    IFT( (X), signed int        , P "d"  , \
    IFT( (X), unsigned int      , P "u"  , \
    IFT( (X), signed long       , P "ld" , \
    IFT( (X), unsigned long     , P "lu" , \
    IFT( (X), signed long long  , P "lld", \
    IFT( (X), unsigned long long, P "llu", \
    IFT( (X), size_t            , P "zu" , \
    IFT( (X), char*             , P "s"  , \
    IFT( (X), void*             , P "p"  , "???" \
  )))))))))))))

#define FPRINT( F, X ) fprintf( F, FMT_OF("%",(X)), (X) )
#define FPRINTP( F, X, P ) fprintf( F, FMT_OF("%0" #P,(X)), (X) )

bool fpse( const char*restrict s, FILE*restrict f ){
  if(!f)
    return false;
  if(s){
    fwrite( s, 1, strlen(s)+1, f );
  }else{
    fputc( 0, f );
  }
  return !ferror( f );
}

bool fpe( const char*restrict a, const char*restrict b, FILE*restrict f ){
  if( !a || !b || *a == '\n' )
    return false;
  return fpse( a, f ) && fpse( b, f );
}

int main( int argc, char* argv[] ){

  if( argc != 2 )
    return 1;

  char host[256] = {0};

  FILE* out = fopen(argv[1],"a");
  FILE* in = stdin;

  if( !out )
    return 2;
  if( !in )
    return 3;

  gethostname( host, sizeof(host) );

  struct timeval time;
  gettimeofday( &time, 0 );

  fpse( "date", out );
  FPRINT( out, time.tv_sec  );
  fputc( '.', out );
  FPRINTP( out, time.tv_usec, 6 );
  fputc( 0, out );

  fpe( "host"    , host                 , out );
  fpe( "user"    , getenv("PAM_USER")   , out );
  fpe( "ruser"   , getenv("PAM_RUSER")  , out );
  fpe( "rhost"   , getenv("PAM_RHOST")  , out );
  fpe( "service" , getenv("PAM_SERVICE"), out );
  fpe( "tty"     , getenv("PAM_TTY")    , out );

  fpse( "password", out );
  {
    int c;
    while( ( c = fgetc(in) ) != EOF && c )
      fputc( c, out );
  }

  fputc( 0, out );
  fputc( '\n', out );

  fclose( in );
  fclose( out );

  return 0;
}
