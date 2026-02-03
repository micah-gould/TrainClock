To use at your own stop use the correct stop id and set it.

If you only want times from a spesific platform, find the lattitude of the platform and set it.

When you wire your own pico, you might have to change the PinMap to get the displays to show the currect numbers.

Dont forget to set your WiFi name and Password in the config.h file.

Error codes:

0 - Faliue to Initialize

1 - Faliure to connect to WiFi

2 - Error with API

3 - Error with the server (Nothing present)

4 - Error with the server (Data avalible, but missing "nextTrainInMinutes")

