#include "processes.h"

Process* running;

Process* Process_Running(){
	return running;
}
void ProcessAddKeyboardHandler(KeyboardHandler handler){
	running->keyboardHandler=handler;
}
