#include <stdio.h>

#include "LeapConnector.h"

int main(){
	// Create a sample listener and controller
	Controller controller;
	LeapConnector LeapMotion;

	// Have the sample listener receive events from the controller
	controller.addListener(LeapMotion);

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();

	// Remove the sample listener when done
	controller.removeListener(LeapMotion);

	return 0;
}