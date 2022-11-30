
typedef enum
{
  COM_Init = 0,
  COM_Instance = 1,  
  COM_Function = 2,
  COM_Params = 3
} COMMAND_States;

const int maxLen = 31;
const int maxParams = 31;

COMMAND_States com_state = COM_Init;
char com_instance[maxLen + 1];
char com_function[maxLen + 1];
int com_ii = 0;
int com_fi = 0;
int com_pi = 0;
double com_params[maxParams + 1];
int com_decimal = 0;
bool com_neg = false;

void initSercom() {
  for (int i = 0; i <= maxLen; i++) com_instance[i] = 0;
  for (int i = 0; i <= maxLen; i++) com_function[i] = 0;
  for (int i = 0; i <= maxParams; i++) com_params[i] = 0.0;
  com_ii = 0;
  com_fi = 0;
  com_pi = 0;
  com_decimal = 0;
  com_neg = false;
  com_state = COM_Init;
}

void handleChar(char c) {
	if (c == ';') {
		com_state = COM_Init;
		return;
	}
	
    switch (com_state) {
		case COM_Init: // wait for digit
		  if (c <= ' ' || c > '~') break;
		case COM_Instance: // instance name or (
		  com_state = COM_Instance;
		  if (c == '.') com_state = COM_Function;
		  else if (c == '(' || c == '=') com_state = COM_Params;
		  else if (c > ' ' && c <= '~' && com_ii < maxLen) com_instance[com_ii++] = c;
		  break;
		case COM_Function: // function name or (
		  if (c == '(' || c == '=') com_state = COM_Params;
		  else if (c > ' ' && c <= '~' && com_fi < maxLen) com_function[com_fi++] = c;
		  break;
		case COM_Params: // Params or )
		  if (c == ',' || c == ')') {
			while (--com_decimal > 0) com_params[com_pi] /= 10.0;
			if (com_neg) com_params[com_pi] = -com_params[com_pi];
			com_neg = false;
			if (com_pi < maxParams) com_pi++;
			if (c == ')') {
			  //Serial.print(com_instance);
			  //if (com_fi > 0) { Serial.print("."); Serial.print(com_function); }
			  //Serial.print("(");
			  //for (int i = 0; i < com_pi; i++) { if (i > 0) Serial.print(", "); Serial.print(com_params[i], 5); }
			  //Serial.println(");");
			  handleCommand();
			  initSercom();
			}
		  }
		  else if (c == '-') com_neg = true;
		  else if (c == '.') com_decimal = 1;
		  else if (c >= '0' && c <= '9') {
			com_params[com_pi] = (com_params[com_pi] * 10.0) + (double)(c - '0');
			if (com_decimal > 0) com_decimal++;
		  }
		  break;  
	}
}

void handleSercom() {
  for (int i = 0; Serial.available() && i < 32; i++) handleChar((char)Serial.read());
}