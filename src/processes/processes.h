#pragma once

#include "../devices/keyboard.h"
typedef void (*KeyboardHandler)(KeyCode code);

typedef struct P{
	int pid;
	KeyboardHandler keyboardHandler;
} Process;

Process* Process_Running();

void ProcessAddKeyboardHandler(KeyboardHandler handler);
