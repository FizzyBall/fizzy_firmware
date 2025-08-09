# Schematics
![](/resources/FiZZy_C_circuit.png)
# Programming Adapter
To program the fizzy board (at least once) a 
[TC2030 6-Pin Tag-Connect](https://www.tag-connect.com/product/tc2030-mcp-nl-6-pin-no-legs-cable-with-rj12-modular-plug-for-microchip-icd)
in combination with a [ESP-Prog adapter](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/esp-prog/user_guide.html)
can be used.

The connection between the ESP-Prog adapter (PROG connector) and the TC2030 (RJ12 connector) is like follows:

|Signal|TC2030|ESP-Prog|
|---|---|---|
|ESP-EN |1|1|
|VDD |2|2|
|ESP-TxD|3|3|
|GND|4|4|
|ESP-RxD|5|5|
|ESP-IO0|6|6|
