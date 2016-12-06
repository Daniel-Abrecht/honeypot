#include <stdio.h>
#include <stdbool.h>

enum state {
  STATE_NEW,
  STATE_NEW_KEY,
  STATE_KEY_START,
  STATE_KEY_ECHO,
  STATE_VALUE_ECHO,

  STATE_INIT = STATE_NEW
};

void eputc( char c ){
  switch(c){
    case '"': putchar('\\'); putchar('"'); break;
    case '\\': putchar('\\'); putchar('\\'); break;
    case '\f': putchar('\\'); putchar('f'); break;
    case '\b': putchar('\\'); putchar('b'); break;
    case '\r': putchar('\\'); putchar('r'); break;
    case '\n': putchar('\\'); putchar('n'); break;
    case '\t': putchar('\\'); putchar('t'); break;
    case '\0': putchar('\\'); putchar('0'); break;
    default: if( (unsigned char)c < ' ' ){
      printf("\\u%.4hhx",c);
    }else{
      putchar(c);
    } break;
  }
}

int main(){

  int c;
  enum state state = STATE_INIT;

  while( ( c = getchar() ) != EOF ){

    start: switch( state ){

      case STATE_NEW: {
        putchar('{');
        state = STATE_NEW_KEY;
        goto start;
      } break;

      case STATE_NEW_KEY: {
        if( c == '\n' ){
          putchar('}');
          putchar('\n');
          state = STATE_NEW;
          continue;
        }else{
          state = STATE_KEY_ECHO;
          putchar('"');
          goto start;
        }
      } break;

      case STATE_KEY_START: {
        if( c == '\n' ){
          putchar('}');
          putchar('\n');
          state = STATE_NEW;
          continue;
        }else{
          state = STATE_KEY_ECHO;
          putchar(',');
          putchar('"');
          goto start;
        }
      } break;

      case STATE_KEY_ECHO: {
        if( c == '\0' ){
          state = STATE_VALUE_ECHO;
          putchar('"');
          putchar(':');
          putchar('"');
          continue;
        }
        eputc(c);
      } break;

      case STATE_VALUE_ECHO: {
        if( c == '\0' ){
          putchar('"');
          state = STATE_KEY_START;
          continue;
        }
        eputc(c);
      } break;

    }

  }

  return 0;
}
