# SW8-Microcontrollers
Microcontroller Code for Seawolf 8 Electronics


## Communication Protocol

This is a clone of the [AUVControlBoard protocol](https://mb3hel.github.io/AUVControlBoard/user_guide/comm_protocol/), however the UART is fixed to a 57600 baud rate.


## Messages

There are multiple types of messages

- Commands / Queries: Sent to MEB instructing it to perform some action. Will be acknowledged. Queries will have the requested data in the acknowledge message
- Acknowledgements: Response to commands. May contain data. Follow same format as [AUVControlBoard acknowledgements](https://mb3hel.github.io/AUVControlBoard/user_guide/messages/#acknowledgements)
- Status Messages: Sent from MEB to computer. Contain unrequested information.


## Commands & Queries

TODO

## Status messages

- `AHT10[temp][humid]`: Data from AHT10 temperature and humidity sensor. Each of `[temp]` and `[humid]` are a 32-bit float (little endian).
- `LEAK[status]`: Leak sensor status. `[status]` = 0 or 1 (1 = leak)
- `TARM[status]`: Thruster arm status; `[status]` = 0 or 1 (1 = armed)
