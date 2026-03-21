1. Event driven read-write for both RequestHandler and ECU.
2. Extract sniffing from RequestHandler.
3. Add timeout on loop for controller.
4. Add state machine to RequestHandler.
5. Split OBD2 polling and display rendering into separate FreeRTOS tasks so the display stays responsive during slow OBD2 requests. Use a shared DataFrame with a queue or mutex.
