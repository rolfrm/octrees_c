#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <iron/utils.h>
#include <iron/types.h>
#include <iron/log.h>
#include "event.h"


keysym_descr keysym_descrs[] = 
  { {KEY_BACKSPACE,'\b',"backspace"},
    {KEY_TAB,'\t',"tab"},
    {KEY_CLEAR,'#',"clear"},
    {KEY_RETURN,'\r',"return"},
    {KEY_PAUSE,'#',"pause"},
    //{KEY_ESCAPE,'^[',"escape"},
    {KEY_SPACE,' ',"space"},
    {KEY_EXCLAIM,'!',"exclaim"},
    {KEY_QUOTEDBL,'"',"quotedbl"},
    {KEY_HASH,'#',"hash"},
    {KEY_DOLLAR,'$',"dollar"},
    {KEY_AMPERSAND,'&',"ampersand"},
    {KEY_QUOTE,'\'',"quote"},
    {KEY_LEFTPAREN,'(',"left parenthesis"},
    {KEY_RIGHTPAREN,')',"right parenthesis"},
    {KEY_ASTERISK,'*',"asterisk"},
    {KEY_PLUS,'+',"plus sign"},
    {KEY_COMMA,',',"comma"},
    {KEY_MINUS,'-',"minus sign"},
    {KEY_PERIOD,'.',"period"},
    {KEY_SLASH,'/',"forward slash"},
    {KEY_0,'0',"0"},
    {KEY_1,'1',"1"},
    {KEY_2,'2',"2"},
    {KEY_3,'3',"3"},
    {KEY_4,'4',"4"},
    {KEY_5,'5',"5"},
    {KEY_6,'6',"6"},
    {KEY_7,'7',"7"},
    {KEY_8,'8',"8"},
    {KEY_9,'9',"9"},
    {KEY_COLON,':',"colon"},
    {KEY_SEMICOLON,';',"semicolon"},
    {KEY_LESS,'<',"less-than sign"},
    {KEY_EQUALS,'=',"equals sign"},
    {KEY_GREATER,'>',"greater-than sign"},
    {KEY_QUESTION,'?',"question mark"},
    {KEY_AT,'@',"at"},
    {KEY_LEFTBRACKET,'[',"left bracket"},
    {KEY_BACKSLASH,'\\',"backslash"},
    {KEY_RIGHTBRACKET,']',"right bracket"},
    {KEY_CARET,'^',"caret"},
    {KEY_UNDERSCORE,'_',"underscore"},
    {KEY_BACKQUOTE,'`',"grave"},
    {KEY_a,'a',"a"},
    {KEY_b,'b',"b"},
    {KEY_c,'c',"c"},
    {KEY_d,'d',"d"},
    {KEY_e,'e',"e"},
    {KEY_f,'f',"f"},
    {KEY_g,'g',"g"},
    {KEY_h,'h',"h"},
    {KEY_i,'i',"i"},
    {KEY_j,'j',"j"},
    {KEY_k,'k',"k"},
    {KEY_l,'l',"l"},
    {KEY_m,'m',"m"},
    {KEY_n,'n',"n"},
    {KEY_o,'o',"o"},
    {KEY_p,'p',"p"},
    {KEY_q,'q',"q"},
    {KEY_r,'r',"r"},
    {KEY_s,'s',"s"},
    {KEY_t,'t',"t"},
    {KEY_u,'u',"u"},
    {KEY_v,'v',"v"},
    {KEY_w,'w',"w"},
    {KEY_x,'x',"x"},
    {KEY_y,'y',"y"},
    {KEY_z,'z',"z"},
    {KEY_DELETE,'?',"delete"},
    //{KEY_KP0,'#',"keypad 0"},
    //{KEY_KP1,'#',"keypad 1"},
    //{KEY_KP2,'#',"keypad 2"},
    //{KEY_KP3,'#',"keypad 3"},
    //{KEY_KP4,'#',"keypad 4"},
    //{KEY_KP5,'#',"keypad 5"},
    //{KEY_KP6,'#',"keypad 6"},
    //{KEY_KP7,'#',"keypad 7"},
    //{KEY_KP8,'#',"keypad 8"},
    //{KEY_KP9,'#',"keypad 9"},
    {KEY_KP_PERIOD,'.',"keypad period"},
    {KEY_KP_DIVIDE,'/',"keypad divide"},
    {KEY_KP_MULTIPLY,'*',"keypad multiply"},
    {KEY_KP_MINUS,'-',"keypad minus"},
    {KEY_KP_PLUS,'+',"keypad plus"},
    {KEY_KP_ENTER,'\r',"keypad enter"},
    {KEY_KP_EQUALS,'=',"keypad equals"},
    {KEY_UP,'#',"up arrow"},
    {KEY_DOWN,'#',"down arrow"},
    {KEY_RIGHT,'#',"right arrow"},
    {KEY_LEFT,'#',"left arrow"},
    {KEY_INSERT,'#',"insert"},
    {KEY_HOME,'#',"home"},
    {KEY_END,'#',"end"},
    {KEY_PAGEUP,'#',"page up"},
    {KEY_PAGEDOWN,'#',"page down"},
    {KEY_F1,'#',"F1"},
    {KEY_F2,'#',"F2"},
    {KEY_F3,'#',"F3"},
    {KEY_F4,'#',"F4"},
    {KEY_F5,'#',"F5"},
    {KEY_F6,'#',"F6"},
    {KEY_F7,'#',"F7"},
    {KEY_F8,'#',"F8"},
    {KEY_F9,'#',"F9"},
    {KEY_F10,'#',"F10"},
    {KEY_F11,'#',"F11"},
    {KEY_F12,'#',"F12"},
    {KEY_F13,'#',"F13"},
    {KEY_F14,'#',"F14"},
    {KEY_F15,'#',"F15"},
    {KEY_NUMLOCK,'#',"numlock"},
    {KEY_CAPSLOCK,'#',"capslock"},
    {KEY_SCROLLOCK,'#',"scrollock"},
    {KEY_RSHIFT,'#',"right shift"},
    {KEY_LSHIFT,'#',"left shift"},
    {KEY_RCTRL,'#',"right ctrl"},
    {KEY_LCTRL,'#',"left ctrl"},
    {KEY_RALT,'#',"right alt"},
    {KEY_LALT,'#',"left alt"},
    //{KEY_RMETA,'#',"right meta"},
    //{KEY_LMETA,'#',"left meta"},
    //{KEY_LSUPER,'#',"left windows key"},
    //{KEY_RSUPER,'#',"right windows key"},
    {KEY_MODE,'#',"mode shift"},
    {KEY_HELP,'#',"help"},
    {KEY_PRINT,'#',"print-screen"},
    {KEY_SYSREQ,'#',"SysRq"},
    {KEY_BREAK,'#',"break"},
    {KEY_MENU,'#',"menu"},
    {KEY_POWER,'#',"power"},
    {KEY_EURO,'#',"euro"}
  };

keysym_descr keysym_descr_from_keysym(keysym sym){
  for(size_t i = 0; i < array_count(keysym_descrs); i++){
    if(keysym_descrs[i].sym == sym)
      return keysym_descrs[i];
  }
  // Should never be reached.
  UNREACHABLE();
  return keysym_descrs[0];
}
