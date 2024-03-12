
typedef enum
{
  COM_Init = 0,
  COM_Instance,
  COM_Function,
  COM_Params,
  COM_Value,
} COMMAND_States;

const int maxLen = 63;
const int maxParams = 63;

COMMAND_States com_state = COM_Init;
char com_instance[maxLen + 1];
char com_function[maxLen + 1];
int com_ii = 0;
int com_fi = 0;
int com_vi = 0;
int com_pi = 0;
bool com_hd = false;
char com_value[maxLen + 1];
float com_params[maxParams + 1];
int com_decimal = 0;
bool com_neg = false;
bool com_escape = false;

void initSercom()
{
  for (int i = 0; i <= maxLen; i++) com_instance[i] = 0;
  for (int i = 0; i <= maxLen; i++) com_function[i] = 0;
  for (int i = 0; i <= maxLen; i++) com_value[i] = 0;
  for (int i = 0; i <= maxParams; i++) com_params[i] = 0.0;
  com_ii = 0;
  com_fi = 0;
  com_vi = 0;
  com_pi = 0;
  com_hd = false;
  com_decimal = 0;
  com_neg = false;
  com_escape = false;
  com_state = COM_Init;
}

void handleChar(char c)
{
  switch (com_state)
  {
		case COM_Init: // wait for digit
		  if (c == ';' || c <= ' ' || c > '~') break;
		case COM_Instance: // instance name or (
		  com_state = COM_Instance;
		  if (c == '.') com_state = COM_Function;
      else if (c == ';') initSercom();
		  else if (c == '(' || c == '=') com_state = COM_Params;
		  else if (c > ' ' && c <= '~' && com_ii < maxLen) com_instance[com_ii++] = c;
		  break;
		case COM_Function: // function name or (
		  if (c == '(' || c == '=') com_state = COM_Params;
      else if (c == ';') initSercom();
		  else if (c > ' ' && c <= '~' && com_fi < maxLen) com_function[com_fi++] = c;
		  break;
		case COM_Params:
		  if (c == '"') com_state = COM_Value;
		  else if (c == ',' || c == ')' || c == ';')
		  {
  			while (--com_decimal > 0) com_params[com_pi] /= 10.0;
  			if (com_neg) com_params[com_pi] = -com_params[com_pi];
        if (com_hd && com_pi < maxParams) com_pi++;
        com_hd = false;
  			com_neg = false;
  			if (c == ')' || c == ';')
  			{
  			  handleCommand();
  			  initSercom();
  			}
		  }
		  else if (c == '-') com_neg = true;
		  else if (c == '.') com_decimal = 1;
		  else if (c >= '0' && c <= '9')
		  {
        com_hd = true;
			  com_params[com_pi] = (com_params[com_pi] * 10.0) + (double)(c - '0');
			  if (com_decimal > 0) com_decimal++;
		  }
		  break;
    case COM_Value:
      if (!com_escape)
      {
        if (c == '"') com_state = COM_Params;
        else if (c == '\\') com_escape = true;
        else if (com_vi < maxLen) com_value[com_vi++] = c;      
      }
      else
      {
        switch (c)
        {
          case 'r': c = '\r'; break;
          case 'n': c = '\n'; break;
          case 't': c = '\t'; break;
          case '\\':
          case '"':
            break;
          default: c = '?'; break;
        }
        if (com_vi < maxLen) com_value[com_vi++] = c;
        com_value[com_vi] = 0;
        com_escape = false;
      }
      break;        
	}
}

void loadPreset(const char* fname)
{
  File f = SD.open(fname);
  if (f)
  {
    initSercom();
    while (f.available()) handleChar((char)f.read());
    f.close();      
  }
  else Serial.printf("File %s not found\r\n", fname);
}

void listPresets()
{
  File dir = SD.open("/");
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry) break;
    Serial.println(entry.name());    
  }  
}

void dumpPreset(const char* fname)
{
  File f = SD.open(fname);
  if (f)
  {
    while (f.available()) Serial.print((char)f.read());
    f.close();      
  }
  else Serial.printf("File %s not found\r\n", fname);
}

void handleSercom()
{
  for (int i = 0; Serial.available() && i < 512; i++) handleChar((char)Serial.read());
}
