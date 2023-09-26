# SW7-Microcontrollers
Microcontroller Code for Seawolf 8 Electronics

Designed for MSP430 Launchpad boards. Currently supported boards:

- MSP-EXP430G2ET w/ G2553
- MSP-EXP430FR2433

Use this Arduino core: [Arduino-Core-MSP430](https://github.com/MB3hel/Arduino-Core-MSP430)

Official energia may work (though is abandoned at this point), but is known to have issues with various boards.


## Communication Protocol

This is a clone of the [AUVControlBoard protocol](https://mb3hel.github.io/AUVControlBoard/user_guide/comm_protocol/), however the UART is fixed to a 57600 baud rate.

Note that the max length of a message is also much shorter (16 bytes) because the MSP430G2553 has only 512 bytes of RAM.

## Messages

There are multiple types of messages

- Commands / Queries: Sent to MEB instructing it to perform some action. Will be acknowledged. Queries will have the requested data in the acknowledge message
- Acknowledgements: Response to commands. May contain data. Follow same format as [AUVControlBoard acknowledgements](https://mb3hel.github.io/AUVControlBoard/user_guide/messages/#acknowledgements)
- Status Messages: Sent from MEB to computer. Contain unrequested information.


## Commands & Queries

None yet


## Status messages

- `AHT10[temp][humid]`: Data from AHT10 temperature and humidity sensor. Each of `[temp]` and `[humid]` are a 32-bit float (little endian).
- `LEAK[status]`: Leak sensor status. `[status]` = 0 or 1 (1 = leak)
- `TARM[status]`: Thruster arm status; `[status]` = 0 or 1 (1 = armed)
- `VSYS[voltage]`: System voltage measurement (using voltage divider). `[voltage]` is a 32-bit float (little endian).
- `SDOWN[cause]`: System is shutting down. `[cause]` is an unsigned 8-bit integer indicating the cause.

## D2 stuff

To render in real time do this:
'''d2 -w energia_flowchart.d2 energia_flowchart.svg -l elk'''
