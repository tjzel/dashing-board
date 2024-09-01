1. Create a mock "offline" CommunicationInterface.
   1. Remove header handling from RequestHandler - now I'm not sure about this.
   2. `CommunicatorMock` has too much handling of the input, juggling between class instances and raw data. Streamline this approach.
2. Create an ESP32 CommunicationInterface implementation.
3. Link the controller to the ESP32 CommunicationInterface.
4. Create a mock ECU using the controller.
